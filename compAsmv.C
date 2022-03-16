#include <TStyle.h>

void compAsmv() {

  gStyle->SetOptStat(0);
  gStyle->SetPalette(51);

  TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);

  //  TFile *File1 = new TFile("/data/snoplus/parkerw/bb_sigex/Jul26_noescale/asimov.root", "OPEN");
  // TFile *File2 = new TFile("/data/snoplus/parkerw/bb_sigex/Jul26_escale/asimov.root", "OPEN");
  
  //  TFile *File1 = new TFile("/data/snoplus/parkerw/bb_sigex/Sep6_3bg_shift0.5/fd_noshift.root", "OPEN");
  //TFile *File2 = new TFile("/data/snoplus/parkerw/bb_sigex/Jan10_Asmv_wGen/Jan10_mcmc1m_noBB/Jan10_mcmc1m_noBB_96/scaled_dists/postfitdist.root", "OPEN");
  TFile *File2 = new TFile("/data/snoplus/parkerw/bb_sigex/Jan12/asimovdata.root", "OPEN");
  TFile *File1 = new TFile("/data/snoplus/parkerw/bb_sigex/Jan10_Asmv_wGen/asimovdata.root");


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

  h1x->GetYaxis()->SetTitle("Events");
  h1x->GetYaxis()->SetTitleOffset(1.4);
  h1x->Draw();
  h2x->SetLineStyle(2);
  h2x->Draw("same");

  TLegend* t1 = new TLegend( 0.5, 0.6, 0.88, 0.88);
  //TLegend* t1 = new TLegend( 0.2, 0.6, 0.58, 0.88);
  t1->AddEntry( h1x, "Prev", "l" );
  t1->AddEntry( h2x, "New", "l" );
  t1->Draw();

  //  h2->Draw("colz");
}
