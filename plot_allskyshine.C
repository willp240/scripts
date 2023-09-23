
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

void plot_allskyshine(  )
{
  // TCanvas c1;
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  RAT::DB::Get()->SetAirplaneModeStatus(true);

  TString hname = "NeckHits";
  TString htitle = "NHits";
  TH2D* h = new TH2D(hname, htitle, 200, -6000, 14000, 100, 0, 1500 );
  h->GetXaxis()->SetTitle("Event Z, mm");
  h->GetYaxis()->SetTitle("NHits");
  h->GetYaxis()->SetTitleOffset(1.3);
  h->SetLineWidth(2);
  

  TString hname2 = "OwlHits";
  TString htitle2 = "Owl Hits";
  TH2D* h2 = new TH2D(hname2, htitle2, 200, -6000, 14000, 1000, 0, 0.1);
  h2->GetXaxis()->SetTitle("Event Z, mm");
  h2->GetYaxis()->SetTitle("High Owl Hits / Total Hits");
  h2->GetYaxis()->SetTitleOffset(1.3);
  h2->SetLineWidth(2);

  TString hname3 = "CleanedHits";
  TString htitle3 = "Cleaned Hits";
  TH2D* h3 = new TH2D(hname3, htitle3, 200, -6000, 14000, 1000, 0.995, 1);
  h3->GetXaxis()->SetTitle("Event Z, mm");
  h3->GetYaxis()->SetTitle("Ratio Cleaned NHits");
  h3->GetYaxis()->SetTitleOffset(1.3);
  h3->SetLineWidth(2);

  std::string fname = "/data/snoplus3/parkerw/ratSimulations/Mar31_nearAV_3.0MeV/nearav_*.root";

  std::cout << fname << std::endl;
  RAT::DU::DSReader dsReader( fname );

  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
    {

	    const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
	    const TVector3 eventPosition = rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed
      double eventz = eventPosition.Z();
      if(rDS.GetEVCount() < 1)
        continue;
	    const RAT::DS::EV& rEV = rDS.GetEV( 0 );
 	    const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
	    /*for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
	      {
	        const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );
	        TVector3 pmtPosition = pmtInfo.GetPosition( pmtCal.GetID() );
          double pmtz = pmtPosition.Z();
        }*/

      int nOwl = 0;
      int nOwlhigh = 0;
      const RAT::DS::UncalPMTs& pmts = rEV.GetUncalPMTs();
      for (size_t iowl = 0; iowl < pmts.GetOWLCount(); ++iowl) {
        const RAT::DS::PMTUncal& owl = pmts.GetOWLPMT(iowl);
        const TVector3 owlPos = pmtInfo.GetPosition(owl.GetID());
        nOwl++;
        double z = owlPos.Z();
        if (z > 8000) ++nOwlhigh;
      }
      double owlhighratio = (double)nOwlhigh/(double)nOwl;
      //double owlratio = (double)nOwl/(double)calibratedPMTs.GetCount();
      double neckratio = (double)pmts.GetNeckCount()/(double)calibratedPMTs.GetCount();
      //std::cout << owlhighratio << " " << neckratio << " " << pmts.GetOWLCount() << std::endl;

      //    h->Fill(eventz,calibratedPMTs.GetCount())
      double ratio = (double)rEV.GetNhitsCleaned() / (double)rEV.GetNhits();
      //      std::cout << ratio << std::endl;
      h3->Fill(eventz, ratio);
      //h2->Fill(eventz, neckratio);

    }
  TCanvas* c1 = new TCanvas("c1", "c1", 800,600);
  h3->Draw("colz");
  //TCanvas* c2 = new TCanvas("c2", "c2", 800,600);
  //c2->cd();
  //h2->Draw("colz");
}
