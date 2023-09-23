
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

void plot_nhit(  )
{
  // TCanvas c1;
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  RAT::DB::Get()->SetAirplaneModeStatus(true);

  TString hname = "NeckHits";
  TString htitle = "NHits";
  TH1D* h = new TH1D(hname, htitle, 100, 2000, 3000);
  h->GetXaxis()->SetTitle("Nhits");
  h->GetYaxis()->SetTitle("Events");
  h->GetYaxis()->SetTitleOffset(1.3);
  h->SetLineWidth(2);
  
  std::string fname = "/data/snoplus3/parkerw/ratSimulations/Apr9_10MeV_SEVRound1/*.root";

  std::cout << fname << std::endl;
  RAT::DU::DSReader dsReader( fname );

  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
    {

      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
      const TVector3 eventPosition = rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed
      double eventz = eventPosition.Z();
      if(rDS.GetEVCount() < 1 || eventPosition.Mag() > 6000)
        continue;

      const RAT::DS::EV& rEV = rDS.GetEV( 0 );
      const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();

      //      std::cout << calibratedPMTs.GetCount() << std::endl;

      h->Fill(calibratedPMTs.GetCount());

    }
  TCanvas* c1 = new TCanvas("c1", "c1", 800,600);
  h->Fit("gaus");
  h->Draw("");

}
