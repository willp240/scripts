#include <RAT/PositionTimeDirectionLikelihood.hh>
#include <RAT/DS/FitResult.hh>
#include <RAT/DS/FitVertex.hh>
#include <RAT/DS/EV.hh>
#include <RAT/Optimiser.hh>
#include <RAT/PMTSelector.hh>
#include <RAT/PDF.hh>
#include <RAT/Processor.hh>

using namespace RAT;
using namespace RAT::Methods;
using namespace RAT::DS;

#include <iostream>
#include <cmath>

#include <TVector3.h>

void
PositionTimeDirectionLikelihood::Initialise( const std::string& )
{
  fPolarDirection = false;
  firstPass = true;
}


void
PositionTimeDirectionLikelihood::SetI( const std::string& param, const int value )
{
  if( param=="polarDirection" )
    {
      if( value>0 )
        fPolarDirection = true;
      else
        fPolarDirection = false;
    }
  else
    throw Processor::ParamUnknown( param );
}


void
PositionTimeDirectionLikelihood::DefaultSeed()
{
  FitVertex vertex;
  // Initialise the SeedResult at the centre and with arbitrary
  // 3000 mm errors in all directions (note units are mm and ns)
  const TVector3 seedPosition( 0.0, 0.0, 0.0 );
  const ROOT::Math::XYZVectorF seedPositionError( 3000.0, 3000.0, 3000.0 );
  // 230 ns is roughly the peak from ET1D and GV1D PDFs
  const double seedTime = 230.0;
  const double seedTimeError = 100.0;
  // Take the direction seed as +ve x direction unit vector and arbitrary error.
  const TVector3 seedDirection( 1.0, 0.0, 0.0 );
  const TVector3 seedDirectionError( 1.0, 1.0, 1.0 );
  vertex.SetPosition( seedPosition, false, true );
  vertex.SetPositionErrors( seedPositionError, false, true );
  vertex.SetTime( seedTime, false, true );
  vertex.SetTimeErrors( seedTimeError, false, true );
  vertex.SetDirection( seedDirection, false, true );
  vertex.SetDirectionErrors( seedDirectionError, false, true );
  fSeedResult.SetVertex( 0, vertex );
}

FitResult
PositionTimeDirectionLikelihood::GetBestFit()
{

  fFitResult.Reset();
  if( fPMTData.empty() )
    return fFitResult;
  CopySeedToResult();

  SelectPMTData( fFitResult.GetVertex( 0 ) );
  fFitResult.SetFOM( "SelectedNHit", static_cast<double>( fSelectedPMTData.size() ) );
  if( fSelectedPMTData.empty() )
    throw MethodFailError( "PositionTimeDirectionLikelihood: No hits to fit" );

  fOptimiser->SetComponent( this );
  // Call the designated optimiser to fit and save the fom

  const double logL = fOptimiser->Maximise();
  fFitResult.SetFOM( "LogL", logL );
  // Now save the optimised values

  //  DirectionLLHScan();

  SetParams( fOptimiser->GetParams() );
  SetPositiveErrors( fOptimiser->GetPositiveErrors() );
  SetNegativeErrors( fOptimiser->GetNegativeErrors() );

  // The direction should be a unit vector and the errors
  // should be scaled accordingly.
  // This is not performed in SetParams as the optimiser adjusts and
  // uses via that function; the method should not force the optimisers decisions.
  FitVertex vertex = fFitResult.GetVertex( 0 );
  double scaling = vertex.GetDirection().Mag();

  bool validity = fOptimiser->GetValid();
  if( scaling == 0.0 )
    {
      // Cannot have a direction with no magnitude
      // Leave as 0, 0, 0 but set as invalid
      validity = false;
      // Ensure we don't divide by 0
      scaling = 1.0;
    }
  //  std::cout << vertex.GetPosition().x() << " " << vertex.GetPosition().y() << " " << vertex.GetPosition().z() << " " << vertex.GetDirection().x() << " " << vertex.GetDirection().y() << " " << vertex.GetDirection().z() << std::endl;
  vertex.SetDirection( vertex.GetDirection().Unit(), validity );
  vertex.SetPositiveDirectionError( TVector3( vertex.GetPositiveDirectionError().X() / scaling,
                                              vertex.GetPositiveDirectionError().Y() / scaling,
                                              vertex.GetPositiveDirectionError().Z() / scaling ),
                                    validity );
  vertex.SetNegativeDirectionError( TVector3( vertex.GetNegativeDirectionError().X() / scaling,
                                              vertex.GetNegativeDirectionError().Y() / scaling,
                                              vertex.GetNegativeDirectionError().Z() / scaling ),
                                    validity );
  fFitResult.SetVertex( 0, vertex );

  return fFitResult;
}


double
PositionTimeDirectionLikelihood::operator()( const std::vector<double>& params )
{
  SetParams(params);
  double logLike = 0.0;
  // loglike sum from pdf

  for( std::vector<FitterPMT>::const_iterator iPMT = fSelectedPMTData.begin(); iPMT != fSelectedPMTData.end(); ++iPMT )
    logLike += log( fPDF->GetProbability( *iPMT, fFitResult.GetVertex( 0 ) ) );

  return logLike;
}

double
PositionTimeDirectionLikelihood::calcLLH( const std::vector<double>& params )
{
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
}

void PositionTimeDirectionLikelihood::DirectionLLHScan() 
{

  //  std::cout << "Start LLH Scan " << std::endl;

  if(!firstPass)
   return;
  firstPass=false;
  
  //declare histo of dot prod to LLH,  and file 
  TFile* outscan = new TFile("outscan.root","UPDATE");
  TGraph* hScan = new TGraph();//new TH1D("LLHScan","LLHScan",50,-1,1);
  hScan->SetName("LLHScan");
  hScan->SetTitle("LLHScan;cos(#theta_{Fit}-#theta_{True}); LLH");
  
  //vector in form PosX, PosY, PosZ, Time, DirX, DirY, DirZ
  std::vector<double> params = fOptimiser->GetParams();
  TVector3 eventDirection(1,1,1);
  eventDirection=eventDirection.Unit();
  TRandom3* random = new TRandom3();

  //set position to origin (true position)
  params[0] = 0.0;
  params[1] = 0.0;
  params[2] = 0.0;
  params[3] = 230;

  //now loop over directions
  for(int i=0;i<500;i++){

    //Generate random direction
    double phi = random->Uniform(0,TMath::Pi()*2);
    //double costheta = random->Uniform(-1,1);
    double theta = random->Uniform(0,TMath::Pi());
    //double cosphi = random->Uniform(-1,1);
    //double theta = acos(costheta);
    //double phi = acos(cosphi);

    double x = sin(theta)*cos(phi);
    double y = sin(theta)*sin(phi);
    double z = cos( theta );
    TVector3 proposed_eventDirection(x,y,z);
    proposed_eventDirection=proposed_eventDirection.Unit();
    params[4] = x;
    params[5] = y;
    params[6] = z;
    SetParams( params );

    //calc LLH from these xyz dir and time
    double LLH = calcLLH( params ); //(*this)( params );

    double cosTheta_Recon = proposed_eventDirection.Dot(eventDirection);

    //fill histogram of dot prod and LLH
    outscan->cd();

    //    std::cout << cosTheta_Recon << " " << LLH << std::endl;
    hScan->SetPoint(i, cosTheta_Recon, LLH);
  }

  //save histogram in file (which allows more events)

  hScan->Write();
  //  std::cout << "End LLH Scan " << std::endl;
  //  outscan->Close();
}


std::vector<double>
PositionTimeDirectionLikelihood::GetParams() const
{
  std::vector<double> params;

  FitVertex vertex = fFitResult.GetVertex(0);
  params.push_back( vertex.GetPosition().x() );
  params.push_back( vertex.GetPosition().y() );
  params.push_back( vertex.GetPosition().z() );
  params.push_back( vertex.GetTime() );

  if( fPolarDirection == true )
    {
      TVector3 direction = vertex.GetDirection().Unit(); // should always be a unit vector
      params.push_back( direction.Theta() );
      params.push_back( direction.Phi() );
    }
  else
    {
      params.push_back( vertex.GetDirection().x() );
      params.push_back( vertex.GetDirection().y() );
      params.push_back( vertex.GetDirection().z() );
    }

  return params;
}


std::vector<double>
PositionTimeDirectionLikelihood::GetPositiveErrors() const
{
  std::vector<double> errors;

  FitVertex vertex = fFitResult.GetVertex(0);
  errors.push_back( vertex.GetPositivePositionError().x() );
  errors.push_back( vertex.GetPositivePositionError().y() );
  errors.push_back( vertex.GetPositivePositionError().z() );
  errors.push_back( vertex.GetPositiveTimeError() );

  if( fPolarDirection == true )
    {
      errors.push_back( vertex.GetPositiveDirectionError().Theta() );
      errors.push_back( vertex.GetPositiveDirectionError().Phi() );
    }
  else
    {
      errors.push_back( vertex.GetPositiveDirectionError().x() );
      errors.push_back( vertex.GetPositiveDirectionError().y() );
      errors.push_back( vertex.GetPositiveDirectionError().z() );
    }

  return errors;
}


std::vector<double>
PositionTimeDirectionLikelihood::GetNegativeErrors() const
{
  std::vector<double> errors;

  FitVertex vertex = fFitResult.GetVertex(0);
  errors.push_back( vertex.GetNegativePositionError().x() );
  errors.push_back( vertex.GetNegativePositionError().y() );
  errors.push_back( vertex.GetNegativePositionError().z() );
  errors.push_back( vertex.GetNegativeTimeError() );

  if( fPolarDirection == true )
    {
      errors.push_back( vertex.GetNegativeDirectionError().Theta() );
      errors.push_back( vertex.GetNegativeDirectionError().Phi() );
    }
  else
    {
      errors.push_back( vertex.GetNegativeDirectionError().x() );
      errors.push_back( vertex.GetNegativeDirectionError().y() );
      errors.push_back( vertex.GetNegativeDirectionError().z() );
    }

  return errors;
}


void
PositionTimeDirectionLikelihood::SetParams( const std::vector<double>& params )
{
  FitVertex vertex = fFitResult.GetVertex(0);
  vertex.SetPosition( TVector3( params[0], params[1], params[2] ), fOptimiser->GetValid() );
  vertex.SetTime( params[3], fOptimiser->GetValid() );

  if( fPolarDirection == true )
    {
      TVector3 dir;
      // unit vector for the direction
      dir.SetMagThetaPhi(1.0, params[4], params[5]);
      vertex.SetDirection( dir, fOptimiser->GetValid() );
    }
  else
    vertex.SetDirection( TVector3( params[4], params[5], params[6] ), fOptimiser->GetValid() );

  fFitResult.SetVertex( 0, vertex );
}

void
PositionTimeDirectionLikelihood::SetPositiveErrors( const std::vector<double>& errors )
{
  FitVertex vertex = fFitResult.GetVertex(0);
  vertex.SetPositivePositionError( ROOT::Math::XYZVectorF(errors[0], errors[1], errors[2]), fOptimiser->GetValid() );
  vertex.SetPositiveTimeError( errors[3], fOptimiser->GetValid() );

  if( fPolarDirection == true )
    {
      TVector3 dir;
      // unit vector for the direction
      dir.SetMagThetaPhi(1.0, errors[4], errors[5]);
      vertex.SetPositiveDirectionError( dir, fOptimiser->GetValid() );
    }
  else
    vertex.SetPositiveDirectionError( TVector3(errors[4], errors[5], errors[6]) );

  fFitResult.SetVertex( 0, vertex );
}


void
PositionTimeDirectionLikelihood::SetNegativeErrors( const std::vector<double>& errors )
{
  FitVertex vertex = fFitResult.GetVertex(0);
  vertex.SetNegativePositionError( ROOT::Math::XYZVectorF(errors[0], errors[1], errors[2]), fOptimiser->GetValid() );
  vertex.SetNegativeTimeError( errors[3], fOptimiser->GetValid() );

  if( fPolarDirection == true )
    {
      TVector3 dir;
      dir.SetMagThetaPhi(1.0, errors[4], errors[5]);
      vertex.SetNegativeDirectionError( dir, fOptimiser->GetValid() );
    }
  else
    vertex.SetNegativeDirectionError( TVector3(errors[4], errors[5], errors[6]) );

  fFitResult.SetVertex( 0, vertex );
}
