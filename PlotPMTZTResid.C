#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"
#include <TH2D.h>

#include <string>
#include <iostream>

void PlotPMTZTResid(){

  gStyle->SetPalette(51);
  gStyle->SetLineWidth(2);
  gStyle->SetOptStat(0);

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  TFile *_file1 = TFile::Open("/home/parkerw/Software/rat_master/example/root/alphaneck_thitpmtz.root");

  TH2D* h1 = (TH2D*)_file1->Get("hHitTimeResidualsFit")->Clone("h1");

  h1->SetTitle("Hit Times vs PMT Z");
  
  h1->Draw("colz");
  c1->SaveAs("alphaneck_trespmtz.pdf");
  
}
