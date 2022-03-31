#include <TH1D.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"
#include "TTree.h"
#include <algorithm>
#include <cctype>
#include <string>

#include <string>


void PlotPDFs(){

  gStyle->SetOptStat(1);
  TCanvas *c = new TCanvas("canv", "canv", 900, 600);

  gStyle->SetOptStat(0);
  gStyle->SetLineWidth(2);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  TFile *file1 = TFile::Open("/data/snoplus/parkerw/ratSimulations/pdfs/FinalisedPDFs/MultiPDF_2gl.root");

  TFile *file2 = TFile::Open("/data/snoplus/parkerw/ratSimulations/pdfs/FinalisedPDFs/MultiPDF_1.1gl.root");
 
  TH1D* h1 = file1->Get("PDF_0");
  TH1D* h2 = file2->Get("PDF_0");

  double int1 = h1->Integral();
  double int2 = h2->Integral();

  h1->Scale(1/int1);
  h2->Scale(1/int2);

  h1->GetYaxis()->SetTitle("Probability");
  h1->GetXaxis()->SetTitle("Time Residual, ns");

  h1->SetLineWidth(2);
  h2->SetLineWidth(2);

  h1->Draw();
  h2->Draw("same");

  TLegend *leg = new TLegend(0.6, 0.65, 0.85, 0.85);
  leg->AddEntry(h1, "2 g/l", "l");
  leg->AddEntry(h2, "1.1 g/l", "l");
  leg->Draw();

}


