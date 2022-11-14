#include <RAT/PositionTimePoissonLikelihood.hh>
#include <RAT/DS/FitResult.hh>
#include <RAT/DS/FitVertex.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/Optimiser.hh>
#include <RAT/PMTSelector.hh>
#include <RAT/PDF.hh>
#include <RAT/Log.hh>

using namespace RAT;
using namespace RAT::Methods;
using namespace RAT::DS;

#include <string>
#include <cmath>

using std::vector;
using std::string;

void
PositionTimePoissonLikelihood::DefaultSeed()
{
  FitVertex vertex;
  // Initialise the SeedResult at the centre and with arbitrary
  // 3000 mm errors in all directions (note units are mm and ns)
  const TVector3 seedPosition( 0.0, 0.0, 0.0 );
  const ROOT::Math::XYZVectorF seedPositionError( 3000.0, 3000.0, 3000.0 );
  // 230 ns is roughly the peak from ET1D and GV1D PDFs
  const double seedTime = 230.0;
  const double seedTimeError = 100.0;
  vertex.SetPosition( seedPosition, false, true );
  vertex.SetPositionErrors( seedPositionError, false, true );
  vertex.SetTime( seedTime, false, true );
  vertex.SetTimeErrors( seedTimeError, false, true );
  fSeedResult.SetVertex( 0, vertex );
}

void PositionTimePoissonLikelihood::SetI( const std::string& param, const int value ) {
  if ( param == std::string("nhit_cut") ) {
    fNHitCut = value;
  } else
    throw Method::MethodFailError( "Unknown positionTimePoissonLikelihood parameter " + param );
}

FitResult
PositionTimePoissonLikelihood::GetBestFit()
{
  fFitResult.Reset();
  if( fPMTData.empty() )
    return fFitResult;
  CopySeedToResult();

  SelectPMTData( fFitResult.GetVertex( 0 ) );
  fFitResult.SetFOM( "SelectedNHit", static_cast<double>( fSelectedPMTData.size() ) );
  if( fSelectedPMTData.empty() )
    throw MethodFailError( "PositionTimePoissonLikelihood: No hits to fit" );

  // As agreed on discussions
  if (fSelectedPMTData.size() < fNHitCut) {
    std::ostringstream msg;
    msg << "PositionTimePoissonLikelihood: Number of hits below threshold of " << fNHitCut;
    throw MethodFailError(msg.str());
  }

  fPDFHist = (TH1D*)fPDF->GetPDF()->Clone("fpdfhist");
  //fPDFHist->Rebin(8);
  fPDFHist->Scale(1/fPDFHist->Integral() );
  fTResidDist = (TH1D*)fPDF->GetPDF()->Clone("ftresidhist");
  //  fTResidDist->Rebin(8);
  fTResidDist->Reset("");

  TFile* outscan1 = new TFile("outscan1.root","UPDATE");
  fPDFHist->Write();
  outscan1->Close();

  fOptimiser->SetComponent( this );
  // Call the designated optimiser to fit and save the fom
  const double logL = fOptimiser->Minimise();
  fFitResult.SetFOM( "LogL", logL );

  LLHScan();

  // Now save the optimised values
  SetParams( fOptimiser->GetParams() );
  SetPositiveErrors( fOptimiser->GetPositiveErrors() );
  SetNegativeErrors( fOptimiser->GetNegativeErrors() );
  return fFitResult;
}

// Following function calculates the log-likelihood value given the current
// fit parameter values in params.  Called by the optimiser.
double PositionTimePoissonLikelihood::operator()( const std::vector<double>& params )
{
  /*
  SetParams(params);
  double logLike = 0.0;
  // loglike sum from pdf
  //  std::cout << "NumPDFs " << fSelectedPMTData.size() << std::endl;

  for( std::vector<FitterPMT>::const_iterator iPMT = fSelectedPMTData.begin(); iPMT != fSelectedPMTData.end(); ++iPMT ){
    double prob = fPDF->GetProbability( *iPMT, fFitResult.GetVertex( 0 ) );
    logLike += log( prob );
    FitterPMT pm = *iPMT;
    //std::cout << pm.GetID() << " " << log( prob ) << " " << logLike << std::endl;
  }
  return logLike;
  */
  SetParams( params );
  double logLike = 0.0;
  fTResidDist->Reset();
  for( vector<FitterPMT>::const_iterator iPMT = fSelectedPMTData.begin(); iPMT != fSelectedPMTData.end(); ++iPMT )
    {
      double timeresid = fPDF->GetProbability( *iPMT, fFitResult.GetVertex( 0 ) );
      //      if(timeresid < -100)
      //timeresid = -100;
      //if(timeresid > 300)
      //timeresid = 300;
      fTResidDist->Fill(timeresid);
    }
  //  fTResidDist->Smooth(4);
  //fTResidDist->Scale(1/fTResidDist->Integral());
  for( int ibin = 1; ibin < fTResidDist->GetXaxis()->GetNbins(); ibin++){
    double mc = fPDFHist->GetBinContent(ibin);
    double dat = fTResidDist->GetBinContent(ibin);
    double llhbin = mc - dat * log(mc);
    if(dat>0)
      logLike += llhbin;
    //std::cout << mc << " " << dat << " " << llhbin << " " << logLike << std::endl;
  }
  //  std::cout << logLike << "<<<<<<<<<<<<<<<<<<<<" << std::endl;
  if(firstTime==true){
    firstTime=false;
    TFile* outscan2 = new TFile("outscan2.root","UPDATE");
    fTResidDist->Write();
    outscan2->Close();
  }
  return logLike;
}

vector<double>
PositionTimePoissonLikelihood::GetParams() const
{
  vector<double> params;
  FitVertex vertex = fFitResult.GetVertex(0);
  params.push_back( vertex.GetPosition().x() );
  params.push_back( vertex.GetPosition().y() );
  params.push_back( vertex.GetPosition().z() );
  params.push_back( vertex.GetTime() );
  return params;
}

vector<double>
PositionTimePoissonLikelihood::GetPositiveErrors() const
{
  vector<double> errors;
  FitVertex vertex = fFitResult.GetVertex(0);
  errors.push_back( vertex.GetPositivePositionError().x() );
  errors.push_back( vertex.GetPositivePositionError().y() );
  errors.push_back( vertex.GetPositivePositionError().z() );
  errors.push_back( vertex.GetPositiveTimeError() );
  return errors;
}

vector<double>
PositionTimePoissonLikelihood::GetNegativeErrors() const
{
  vector<double> errors;
  FitVertex vertex = fFitResult.GetVertex(0);
  errors.push_back( vertex.GetNegativePositionError().x() );
  errors.push_back( vertex.GetNegativePositionError().y() );
  errors.push_back( vertex.GetNegativePositionError().z() );
  errors.push_back( vertex.GetNegativeTimeError() );
  return errors;
}

void
PositionTimePoissonLikelihood::SetParams( const std::vector<double>& params )
{
  FitVertex vertex = fFitResult.GetVertex(0);
  vertex.SetPosition( TVector3( params[0], params[1], params[2] ), fOptimiser->GetValid() );
  vertex.SetTime( params[3], fOptimiser->GetValid() );
  fFitResult.SetVertex( 0, vertex );
}

void
PositionTimePoissonLikelihood::SetPositiveErrors( const std::vector<double>& errors )
{
  FitVertex vertex = fFitResult.GetVertex(0);
  vertex.SetPositivePositionError( ROOT::Math::XYZVectorF(errors[0], errors[1], errors[2]), fOptimiser->GetValid() );
  vertex.SetPositiveTimeError( errors[3], fOptimiser->GetValid() );
  fFitResult.SetVertex( 0, vertex );
}

void
PositionTimePoissonLikelihood::SetNegativeErrors( const std::vector<double>& errors )
{
  FitVertex vertex = fFitResult.GetVertex(0);
  vertex.SetNegativePositionError( ROOT::Math::XYZVectorF(errors[0], errors[1], errors[2]), fOptimiser->GetValid() );
  vertex.SetNegativeTimeError( errors[3], fOptimiser->GetValid() );
  fFitResult.SetVertex( 0, vertex );
}


void PositionTimePoissonLikelihood::LLHScan()
{
  std::cout << "in scan " << std::endl;
  //declare histo of dot prod to LLH,  and file
  TFile* outscan = new TFile("outscan.root","UPDATE");
  TGraph* hScanX = new TGraph();//new TH1D("LLHScan","LLHScan",50,-1,1);
  hScanX->SetName("LLHScanX");
  TGraph* hScanY = new TGraph();//new TH1D("LLHScan","LLHScan",50,-1,1);
  hScanY->SetName("LLHScanY");
  TGraph* hScanZ = new TGraph();//new TH1D("LLHScan","LLHScan",50,-1,1);
  hScanZ->SetName("LLHScanZ");

  //vector in form PosX, PosY, PosZ, Time, DirX, DirY, DirZ
  std::vector<double> params = fOptimiser->GetParams();

  //set position to origin (true position)
  params[0] = 0.0;
  params[1] = 0.0;
  params[2] = 0.0;
  params[3] = 230;

  int count=0;
  for(int x=-1000; x<1000; x+= 20){
 
	params[0] = x;
	params[1] = 0.0;
	params[2] = 0.0;
	SetParams( params );
	double LLH = calcLLH( params );
	outscan->cd();
	hScanX->SetPoint(count, x, LLH);

        params[0] = 0.0;
	params[1] = x;
        params[2] = 0.0;
        SetParams( params );
        LLH = calcLLH( params );
	hScanY->SetPoint(count, x, LLH);

	params[0] = 0.0;
	params[1] = 0.0;
        params[2] = x;
        SetParams( params );
        LLH = calcLLH( params );
	hScanZ->SetPoint(count, x, LLH);

	count++;
  }
  hScanX->Write();
  hScanY->Write();
  hScanZ->Write();
}

// Following function calculates the log-likelihood value given the current
// fit parameter values in params.  Called by the optimiser.
double PositionTimePoissonLikelihood::calcLLH( const std::vector<double>& params )
{

  SetParams( params );
  double logLike = 0.0;
  fTResidDist->Reset();
  for( vector<FitterPMT>::const_iterator iPMT = fSelectedPMTData.begin(); iPMT != fSelectedPMTData.end(); ++iPMT )
    {
      double timeresid = fPDF->GetProbability( *iPMT, fFitResult.GetVertex( 0 ) );
      if(timeresid < -100)
        timeresid = -100;
      if(timeresid > 300)
        timeresid = 300;
      fTResidDist->Fill(timeresid);
    }
  fTResidDist->Smooth();
  fTResidDist->Scale(1/fTResidDist->Integral());
  for( int ibin = 1; ibin < fPDFHist->GetXaxis()->GetNbins(); ibin++){
    double mc = fPDFHist->GetBinContent(ibin);
    double dat = fTResidDist->GetBinContent(ibin);
    double llhbin = mc - dat * log(dat/mc);
    if(dat>0)
      logLike += llhbin;
    //std::cout << mc << " " << dat << " " << llhbin << " " << logLike << std::endl;
  }
  //  std::cout << logLike << "<<<<<<<<<<<<<<<<<<<<" << std::endl;
  
  return logLike;

}
