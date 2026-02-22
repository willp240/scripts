
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

void PlotScintPhotons(  )
{
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  
  RAT::DB::Get()->SetAirplaneModeStatus(true);

  TString hname = "hname";
  TString htitle = "Number of Scintillation Photons: 600 GeV #mu";
  //  TH1D* h = new TH1D(hname, htitle, 100, 0, 100000000);
  TH1D* h = new TH1D(hname, htitle, 100, 0, 100000);
  h->GetXaxis()->SetTitle("Number of Scintillation Photons");
  h->GetYaxis()->SetTitle("Events");
  h->GetYaxis()->SetTitleOffset(1.1);
  h->SetLineWidth(2);
  h->SetLineColor(kBlue+2);
  
  std::string fname = "/data/snoplus3/parkerw/ratSimulations/Dec5_e_notrack/*.root";

  std::cout << fname << std::endl;
  RAT::DU::DSReader dsReader( fname );

  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
    {

      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
      const RAT::DS::MC& mc = rDS.GetMC();

      int nScint = mc.GetNScintPhotons();
      h->Fill(nScint);

    }

  h->Draw("");
  c1->SaveAs("escintphotons.pdf");
}
