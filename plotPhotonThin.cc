#include <TH1D.h>
#include <TFile.h>
#include <TVector3.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TStyle.h>

void fullPlot()
{

  gStyle->SetOptStat(0);
  TFile *infile = new TFile("Jul21_Thinning_PoissonQ.root","READ");

  TH1D* h1 =(TH1D*)infile->Get("MCPEhisto")->Clone();

  h1->GetXaxis()->SetRangeUser(0,9.5);
  h1->GetYaxis()->SetRangeUser(-0.35,0.05);
  h1->GetYaxis()->SetTitleOffset(1.3);
  h1->Draw();
}

void zoomPlot()
{

  gStyle->SetOptStat(0);
  TFile *infile = new TFile("Jul21_Thinning_PoissonQ.root","READ");

  TH1D* h1 =(TH1D*)infile->Get("MCPEhisto")->Clone();

  h1->GetXaxis()->SetRangeUser(0.5,3.5);
  h1->GetYaxis()->SetRangeUser(-0.025,0.01);
  h1->GetYaxis()->SetTitleOffset(1.3);
  TF1  *f1 = new TF1("f1","pol1",0.5,3.5);
  f1->SetLineColor(kRed);
  h1->Fit(f1);
  h1->Draw();
}
