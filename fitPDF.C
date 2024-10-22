#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"
#include <TMath.h>

#include <string>

void fitPDF( )
{
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TCanvas* c = new TCanvas("c", "c", 800,600);
  c->SetGrid(1);

  TFile *_fileAll = TFile::Open("/data/snoplus3/parkerw/ratSimulations/Jul16_bisMSB_posrecoord/plots/MultiPDF_Plots_Round2.root");
  
  TH1D* h = (TH1D*) _fileAll->Get("PDF_0");
  h->GetXaxis()->SetTitle("Hit time residuals [ns]");
  h->GetYaxis()->SetTitle("Normalised Counts");
  h->GetYaxis()->SetTitleOffset(1.3);
  h->Scale(1/h->Integral());
  h->GetXaxis()->SetRangeUser(-100,300);
  h->GetYaxis()->SetRangeUser(0.0,0.06);
  //  h->Draw("hist");

  //  TF1* f = new TF1("f", "[0] *( (e^(-x/[1]) - e^(-x/[2])) / ([1] - [2]) ) + [3] *( (e^(-x/[4]) - e^(-x/[2])) / ([4] - [2]) ) + [5] *( (e^(-x/[6]) - e^(-x/[2])) / ([6] - [2]) ) + [7] *( (e^(-x/[8]) - e^(-x/[2])) / ([8] - [2]) )  ", 0,30);
  //double pars[9] = { 0.45, 5.5, 1.22, 0.145, 41.0, 0.05, 60.0, 0.05, 500.0 };

  TF1* f = new TF1("f", "[0] *( (e^(-x/[1]) - e^(-x/[2])) / ([1] - [2]) ) ", -1,100);
  //  double pars[3] = { 0.45, 5.5, 1.22 };
  double pars[3] = { 0.767, 10.8268, 1.6376};
  
  f->SetParameters(pars);
  f->SetLineColor(kBlue);
  f->SetRange(-1,100);
  //f->Draw("");
  //h->Fit(f);
  h->Draw("hist");
  f->Draw("same");
  
  c->SaveAs("timeresidfit.pdf");
}
