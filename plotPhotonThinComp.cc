#include <TH1D.h>
#include <TFile.h>
#include <TVector3.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TStyle.h>


void plotPhotonThinComp()
{

  gStyle->SetOptStat(0);
  TFile *infile = new TFile("Jul15_Thinning_Poisson30Comp.root","READ");

  TH1D* h1 =(TH1D*)infile->Get("hMCPEs")->Clone();
  TH1D* h2 =(TH1D*)infile->Get("hMCPEs2")->Clone();

  h1->GetXaxis()->SetRangeUser(1500,2500);
  //  h1->GetYaxis()->SetTitleOffset(1.3);
  TF1  *f1 = new TF1("f1","gaus");
  f1->SetLineColor(kBlack);
  TF1  *f2 = new TF1("f2","gaus");
  f2->SetLineColor(kRed);
  h1->Fit(f1);
  h2->Fit(f2);
  h1->Draw();
  h2->Draw("same");
}
