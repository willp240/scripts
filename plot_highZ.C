
#include <RAT/DU/DSReader.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/MC.hh>
#include <RAT/DS/EV.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/FitResult.hh>
#include <RAT/DS/PMTSet.hh>

#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>

#include <string>
#include <math.h>

void plot_highZ(  )
{
  // TCanvas c1;
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  RAT::DB::Get()->SetAirplaneModeStatus(true);

  TString hname = "Neck Ring";
  TString htitle = "Hits";
  TH2D* h = new TH2D(hname, htitle, 200, -6000, 14000, 1000, 0, 0.5);
  h->GetXaxis()->SetTitle("Event Z, mm");
  h->GetYaxis()->SetTitle("Top Hits");
  h->GetYaxis()->SetTitleOffset(1.3);
  h->SetLineWidth(2);
  
  std::string fname = "/data/snoplus3/parkerw/ratSimulations/Mar31_nearAV_3.0MeV/nearav_10.root";

  std::cout << fname << std::endl;
  RAT::DU::DSReader dsReader( fname );

  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
    {

      int num_ring = 0;

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

	  if(pmtz > 0 && sqrt(pmtPosition.X()*pmtPosition.X() + pmtPosition.Y()*pmtPosition.Y() ) < 3000)
	    num_ring ++;
        }

      double ring_ratio = (double) num_ring/(double)calibratedPMTs.GetCount();
      //      std::cout << eventz << " " << num_ring << " " << calibratedPMTs.GetCount() << " " << ring_ratio << std::endl;

      h->Fill(eventz,ring_ratio);

    }
  TCanvas* c1 = new TCanvas("c1", "c1", 800,600);
  h->Draw("colz");
 
}
