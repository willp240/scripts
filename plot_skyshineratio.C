
#include <RAT/DU/DSReader.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/MC.hh>
#include <RAT/DS/EV.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/FitResult.hh>

#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>

#include <string>
#include <math.h>

void plot_skyshineratio(  )
{
  // TCanvas c1;
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  RAT::DB::Get()->SetAirplaneModeStatus(true);

  TString hname = "SkyShineRatio";
  TString htitle = "SkyShine Ratio";
  TH2D* h = new TH2D(hname, htitle, 200, -6000, 14000, 100, 0, 15);
  h->GetXaxis()->SetTitle("Event Z, mm");
  h->GetYaxis()->SetTitle("SkyShine Ratio");
  h->GetYaxis()->SetTitleOffset(1.3);
  h->SetLineWidth(2);

  std::string fname = "/data/snoplus3/parkerw/ratSimulations/Mar31_nearAV_3.0MeV/nearav_*.root";

  std::cout << fname << std::endl;
  RAT::DU::DSReader dsReader( fname );

  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
    {

      int nSide = 0;
      int nSignal = 0;

	    const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
	    const TVector3 eventPosition = rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed
      double eventz = eventPosition.Z();
      if(rDS.GetEVCount() < 1)
        continue;
	    const RAT::DS::EV& rEV = rDS.GetEV( 0 );
 	    const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
	    for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
	      {
	        const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );
	        TVector3 pmtPosition = pmtInfo.GetPosition( pmtCal.GetID() );
          double pmtz = pmtPosition.Z();
          //std::cout << pmtz << std::endl;
          if( pmtz > 0 && pmtz < 4000 )
	          nSignal++;
          else if( pmtz > -4000 && pmtz < 0 )
            nSide++;
        }
      double ratio = (double)nSide / (double)nSignal;
      //std::cout << "Filling " << eventz << " " << ratio << std::endl;
      h->Fill(eventz, ratio);
    }

  h->Draw("colz");

}