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

#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>

#include <string>

#include <TH2D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <TF1.h>
#include <iostream>


int DrawNumScintPhotons( )
{

  std::string fileName2 = "/data/snoplus/parkerw/ratSimulations/batch/Sep28_nondirectional/nondirectional_0.root";
  std::string fileName1 = "/data/snoplus/parkerw/ratSimulations/Sep29_gauss12_nondirectional/nondirectional_0.root";

  TH1D* hNumScint1 = new TH1D( "hNumScint1", "Number of Scintillation Photons per Event", 100, 10000, 22000 );
  TH1D* hNumScint2 = new TH1D( "hNumScint2", "Number of Scintillation Photons per Event", 100, 10000, 22000 );

  TF1* f1 = new TF1("f1", "gaus");
  TF1* f2 = new TF1("f2", "gaus");
  f2->SetLineColor(kRed);

  RAT::DU::DSReader dsReader1( fileName1 );
  for( size_t iEntry = 0; iEntry < dsReader1.GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS = dsReader1.GetEntry( iEntry );
      hNumScint1->Fill( rDS.GetMC().GetNScintPhotons() );
    }
  hNumScint1->GetYaxis()->SetTitle( "Counts" );
  hNumScint1->GetXaxis()->SetTitle( "Number of Scintillation Photons per Event" );
  hNumScint1->Fit(f1);
  //  hNumScint1->Draw();

  RAT::DU::DSReader dsReader2( fileName2 );
  for( size_t iEntry = 0; iEntry < dsReader2.GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS2 = dsReader2.GetEntry( iEntry );
      hNumScint2->Fill( rDS2.GetMC().GetNScintPhotons() );
    }
  hNumScint2->SetLineColor(kRed);
  hNumScint2->Fit(f2);
  hNumScint1->Draw();
  hNumScint2->Draw("same");
}


int DrawNHits( )
{
  RAT::DB::Get()->SetAirplaneModeStatus(true);
  std::string fileName2 = "/home/parkerw/Software/rat_master/nobismuth.root";
  std::string fileName1 = "/home/parkerw/Software/rat_master/bismuth.root";

  TH1D* hNHits1 = new TH1D( "hNHits1", "NHits", 100, 0, 1500 );
  TH1D* hNHits2 = new TH1D( "hNHits2", "Nhits", 100, 0, 1500 );

  TF1* f1 = new TF1("f1", "gaus",0,1500);
  TF1* f2 = new TF1("f2", "gaus",0,1500);
  f2->SetLineColor(kRed);

  RAT::DU::DSReader dsReader1( fileName1 );
  for( size_t iEntry = 0; iEntry < dsReader1.GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS = dsReader1.GetEntry( iEntry );
      for( size_t iMCEvent = 0; iMCEvent < rDS.GetMCEVCount(); iMCEvent++ )
	hNHits1->Fill( rDS.GetMCEV(iMCEvent).GetMCHits().GetAllCount() );
    }
  hNHits1->GetYaxis()->SetTitle( "Counts" );
  hNHits1->GetXaxis()->SetTitle( "NHits per Event" );
  hNHits1->Fit(f1,"R");
  //  hNumScint1->Draw();

  RAT::DU::DSReader dsReader2( fileName2 );
  for( size_t iEntry = 0; iEntry < dsReader2.GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS2 = dsReader2.GetEntry( iEntry );
      for( size_t iMCEvent = 0; iMCEvent < rDS2.GetMCEVCount(); iMCEvent++ ){
	hNHits2->Fill( rDS2.GetMCEV(iMCEvent).GetMCHits().GetAllCount() );
      }
    }
  hNHits2->SetLineColor(kRed);
  hNHits2->Fit(f2,"R");
  hNHits1->Draw();
  hNHits2->Draw("same");

  TLegend* legend = new TLegend(0.1,0.7,0.48,0.9);
  legend->AddEntry(hNHits1,"With bisMSB","l");
  legend->AddEntry(hNHits2,"No bisMSB","l");
  legend->Draw();
  std::cout << hNHits1->Integral() << " " << hNHits2->Integral() << std::endl;
}
