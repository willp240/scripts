
#include <RAT/DU/DSReader.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/DU/LightPathCalculator.hh>
#include <RAT/DU/GroupVelocity.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/MC.hh>
#include <RAT/DS/EV.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/FitResult.hh>
#include <RAT/LightPathStraightScint.hh>
#include <RAT/DU/TimeResidualCalculator.hh>
#include <RAT/SunUtil.hh>

#include <TFile.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TMath.h>

#include <string>

void* checkDir( const std::string& fileName)
{

  gStyle->SetOptStat(0);

  TH1D* htheta = new TH1D( "htheta", "cos #theta_{Sun}", 202, -1.01, 1.01);

  // If this is being done on data that does not require remote database connection
  // eg.: a simple simulation with default run number (0)
  // We can disable the remote connections:
  //
  // NOTE: Don't do this if you are using real data!!!
  RAT::DB::Get()->SetAirplaneModeStatus(true);

  RAT::DU::DSReader dsReader( fileName );

  RAT::DU::TimeResidualCalculator timeResCalc = RAT::DU::Utility::Get()->GetTimeResidualCalculator(); // To calculate the light's path
  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry<dsReader.GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
      for( size_t iEV = 0; iEV < rDS.GetEVCount(); iEV++ )
        {
	  if(iEV>0)// man don't want no retriggers
	    continue;
          const RAT::DS::EV& rEV = rDS.GetEV( iEV );

	  if(rDS.GetMC().GetMCParent(0).GetPDGCode() == 12 || rDS.GetMC().GetMCParent(0).GetPDGCode() == 14){
	    
	    TVector3 trueDir = rDS.GetMC().GetMCParent(0).GetMomentum().Unit();
	    RAT::DS::UniversalTime fTime = rEV.GetUniversalTime();
	    TVector3 solarDirection = RAT::SunDirection(fTime.GetDays(),fTime.GetSeconds(),fTime.GetNanoSeconds()).Unit();
	    double cosTheta = -solarDirection.Dot(trueDir);
	    std::cout << "filling " << cosTheta << std::endl;
	    htheta->Fill( cosTheta );
	  }
	  else
	    std::cout << "Steady will, pdg " << rDS.GetMC().GetMCParent(0).GetPDGCode() << std::endl;
	}
    }

  TCanvas* c1 = new TCanvas();
  htheta->GetXaxis()->SetTitle( "cos #theta_{Sun}, ns" );
  htheta->GetYaxis()->SetTitle( "Events" );
  htheta->Draw();

  TFile* file = new TFile("htheta1.root","RECREATE");
  htheta->Write();

}
