#include <TStyle.h>

void compAsmv() {

  gStyle->SetOptStat(0);
  gStyle->SetPalette(51);

  TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);

  //  TFile *File1 = new TFile("/data/snoplus/parkerw/bb_sigex/Jul26_noescale/asimov.root", "OPEN");
  // TFile *File2 = new TFile("/data/snoplus/parkerw/bb_sigex/Jul26_escale/asimov.root", "OPEN");
  
  TFile *File1 = new TFile("/data/snoplus/parkerw/bb_sigex/Aug3_3Bg_1.00/fd_escale1.00.root", "OPEN");
  TFile *File2 = new TFile("/data/snoplus/parkerw/bb_sigex/Aug3_3Bg_1.05/fd_escale1.05.root", "OPEN");

  TIter next1(File1->GetListOfKeys());
  TKey *key1;
  
  while ((key1 = (TKey*)next1())) {
    TClass *cl1 = gROOT->GetClass(key1->GetClassName());
    if (!cl1->InheritsFrom("TH1")) continue;
    TH2D *h1 = (TH2*)key1->ReadObj()->Clone();
    h1->SetLineColor(kBlue);
  }
  TH1D* h1x = (TH1D*)h1->ProjectionX()->Clone();
  TH1D* h1y = (TH1D*)h1->ProjectionY()->Clone();  

  TIter next2(File2->GetListOfKeys());
  TKey *key2;

  while ((key2 = (TKey*)next2())) {
    TClass *cl2 = gROOT->GetClass(key2->GetClassName());
    if (!cl2->InheritsFrom("TH1")) continue;
    TH2D *h2 = (TH2*)key2->ReadObj()->Clone();
    h2->SetLineColor(kRed);
  }
  
  TH1D* h2x = (TH1D*)h2->ProjectionX()->Clone();
  TH1D* h2y = (TH1D*)h2->ProjectionY()->Clone();

  h1y->GetYaxis()->SetTitle("Events");
  h1y->GetYaxis()->SetTitleOffset(1.4);
  h1y->Draw();
  h2y->Draw("same");

  //TLegend* t1 = new TLegend( 0.5, 0.6, 0.88, 0.88);
  TLegend* t1 = new TLegend( 0.2, 0.6, 0.58, 0.88);
  t1->AddEntry( h1x, "Energy Scale = 1.0", "l" );
  t1->AddEntry( h2x, "Energy Scale = 1.05", "l" );
  t1->Draw();

  h1->Draw("colz");
}
