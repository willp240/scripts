
#include <RAT/DU/DSReader.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/MultiPDFFull.hh>
#include <RAT/DU/GroupVelocity.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/MC.hh>
#include <RAT/DS/EV.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/FitResult.hh>
#include <RAT/FitterPMT.hh>
#include <RAT/PDFFactory.hh>
#include <RAT/PDFMethod.hh>
#include <RAT/PDF.hh>
#include <RAT/DU/LightPathStraightScint.hh>

#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"

#include <string>
using namespace std;

double
GetProbability( const RAT::FitterPMT& pmt,
                const RAT::DS::FitVertex& vertex );

double
CalcProb( double distance,
          double timeresid );

TH1D* fHitTimeResiduals = new TH1D( "fHitTimeResidualsMC", "Hit time residuals using the MC position", 401, -100.5, 300.5 );

TH1D* fDistance = new TH1D( "fDistance", "PMT - Vertex Distance Distribution", 200, 0, 20 );
TH1D* fDistance2 = new TH1D( "fDistance2", "PMT - Vertex Distance Distribution", 200, 0, 20 );
void* CompareHitDists( const string& fileName )
{
  RAT::DB::Get()->SetAirplaneModeStatus(true);

  RAT::DU::DSReader dsReader( fileName );
  RAT::DS::Run run = dsReader.GetRun();
  RAT::DU::LightPathStraightScint::BeginOfRun();

  double LLH=0;
  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry < 1; iEntry++ )//dsReader.GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
      const TVector3 startPos(0,0,0);//rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed
      double eventTime = rDS.GetMC().GetMCParticle(0).GetTime();
      for( size_t iEV = 0; iEV < 1; iEV++)//rDS.GetEVCount(); iEV++ )
        {
          const RAT::DS::EV& rEV = rDS.GetEV( iEV );
	  RAT::DS::FitVertex rVertex = rEV.GetFitResult("scintFitter").GetVertex(0);
          const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
	  for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
	    //for( vector<RAT::FitterPMT>::const_iterator iPMT = calibratedPMTs.begin(); iPMT != calibratedPMTs.end(); ++iPMT )
	    { 
	      RAT::FitterPMT fitPMT = calibratedPMTs.GetPMT(iPMT);
	      LLH += log( GetProbability( fitPMT, rVertex ) );
            }
        }
    }
  std::cout << LLH << std::endl;
  fHitTimeResiduals->Draw();
  fHitTimeResiduals->GetXaxis()->SetTitle("Hit Time Residuals, ns");
  fHitTimeResiduals->GetYaxis()->SetTitle("Hits");
  fDistance->Draw();
  fDistance2->SetLineColor(kRed);
  fDistance2->Draw("same");
  fDistance->GetXaxis()->SetTitle("PMT-Vertex Distance, m");
  fDistance->GetYaxis()->SetTitle("Hits");
  TFile *outfile = TFile::Open("./fit5100.root","RECREATE");
  fDistance->Write();
  fHitTimeResiduals->Write();
}

double
GetProbability( const RAT::FitterPMT& pmt,
	        const RAT::DS::FitVertex& vertex )
{
  //Get seed vertex position
  TVector3 startPos;
  double eventTime;
  try
    {
      startPos = vertex.GetPosition();
      eventTime = vertex.GetTime();
    }
  catch( RAT::DS::FitVertex::NoValueError& error )
    {
      std::cout << "MultiPDFFull::GetProbability: This PDF does nothing without a proposed position & time" << std::endl;;
      return 0.0;
    }

  const double mag2 = startPos.Mag2();
  if( std::isinf( mag2 ) || std::isnan( mag2 ) )
    return 0.0;
  startPos={5100,0,0};
  TVector3 startPos2;
  startPos2={5100,0,0};


  //get path from vertex to pmt
  const TVector3 pmtPos = RAT::DU::Utility::Get()->GetPMTInfo().GetPosition( pmt.GetID() );
  double distInAV = 0.0;
  double distInWater = 0.0;
  double distInTarget = 0.0;

  RAT::DU::LightPathStraightScint::GetPath(pmtPos, startPos, distInTarget, distInWater);

  float transitTime = RAT::DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( distInTarget, distInAV, distInWater );
  float correctedTime = pmt.GetTime() - transitTime - eventTime;

  double distance = (pmtPos - startPos).Mag();
  double distance2 = (pmtPos - startPos2).Mag();

  fHitTimeResiduals->Fill(correctedTime);
  fDistance->Fill(distance/1000);
  fDistance2->Fill(distance2/1000);

  return CalcProb(distance/1000, correctedTime);
}

double
CalcProb( double distance,
          double timeresid )
{
 
  TF2* fPDFfunc = new TF2("fPDFfunc", "100000 * ((0.2) * (1/(([4]*y*y*y + [5]*y*y + [6]*y + [7]) * sqrt(2*TMath::Pi()))) * exp(-0.5*((x-([0]*y*y*y + [1]*y*y + [2]*y + [3]))/([4]*y*y*y + [5]*y*y + [6]*y + [7])*((x-([0]*y*y*y + [1]*y*y + [2]*y + [3]))/([4]*y*y*y + [5]*y*y + [6]*y + [7])))) + (0.2) * (1/(([12]*y*y*y + [13]*y*y + [14]*y + [15]) * sqrt(2*TMath::Pi()))) * exp(-0.5*((x-([8]*y*y*y + [9]*y*y + [10]*y + [11]))/([12]*y*y*y + [13]*y*y + [14]*y + [15])*((x-([8]*y*y*y + [9]*y*y + [10]*y + [11]))/([12]*y*y*y + [13]*y*y + [14]*y + [15])))) + (0.2) * (1/(([20]*y*y*y + [21]*y*y + [22]*y + [23]) * sqrt(2*TMath::Pi()))) * exp(-0.5*((x-([16]*y*y*y + [17]*y*y + [18]*y + [19]))/([20]*y*y*y + [21]*y*y + [22]*y + [23])*((x-([16]*y*y*y + [17]*y*y + [18]*y + [19]))/([20]*y*y*y + [21]*y*y + [22]*y + [23])))) + (0.2) * (1/(([28]*y*y*y + [29]*y*y + [30]*y + [31]) * sqrt(2*TMath::Pi()))) * exp(-0.5*((x-([24]*y*y*y + [25]*y*y + [26]*y + [27]))/([28]*y*y*y + [29]*y*y + [30]*y + [31])*((x-([24]*y*y*y + [25]*y*y + [26]*y + [27]))/([28]*y*y*y + [29]*y*y + [30]*y + [31])))) + (0.2) * (1/(([36]*y*y*y + [37]*y*y + [38]*y + [39]) * sqrt(2*TMath::Pi()))) * exp(-0.5*((x-([32]*y*y*y + [33]*y*y + [34]*y + [35]))/([36]*y*y*y + [37]*y*y + [38]*y + [39])*((x-([32]*y*y*y + [33]*y*y + [34]*y + [35]))/([36]*y*y*y + [37]*y*y + [38]*y + [39])))))", -100,300,0,20);

  double Pars[40] = {0.00204014,-0.0536985,0.614027,0.580781,0.00056154,-0.0135426,0.215268,1.19425,0.0101883,-0.245022,2.39863,-1.13178,0.00142124,-0.029282,0.439138,1.06026,0.0155742,-0.366303,4.12082,-2.53441,0.00267831,-0.0777945,1.24862,0.00248356,-0.0243384,0.455121,1.23821,5.11949,0.0034488,-0.253894,4.05532,-3.86752,-0.0496268,0.402598,8.99794,-1.46508,0.00850524,-0.44958,6.38404,6.20506 };

  fPDFfunc->SetParameters(Pars);

 //  distance = 7;
  if(distance<2||distance>15)
    return 0.5;

  double prob = fPDFfunc->Eval(timeresid, distance);
  //  double prob = f->Eval(timeresid);

  //if prob < 0, prob = 0
  if(prob<0.5) prob=0.5;

  //  delete f;
  //  delete f2;
  //delete f3;
  //delete f4;

  return prob;

}
