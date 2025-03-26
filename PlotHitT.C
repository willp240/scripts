#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"
#include <TH2D.h>

#include <string>
#include <iostream>

void PlotHitT(){

  gStyle->SetPalette(51);
  gStyle->SetLineWidth(2);
  gStyle->SetOptStat(0);

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  TFile *_file1 = TFile::Open("/home/parkerw/Software/rat_master/example/root/alphaav_thitpmtz.root");

  TH2D* h1 = (TH2D*)_file1->Get("hHitTimeResidualsFit")->Clone("h1");
  h1->SetTitle("Hit Time, ns");
  h1->RebinY(2);
  h1->GetXaxis()->SetTitle("Hit Time, ns");
  h1->GetYaxis()->SetTitle("PMT Z, mm");
  h1->GetXaxis()->SetRangeUser(100,500);
  h1->SetLineWidth(2);
  h1->SetLineColor(kBlue);
  h1->Rebin(5);
  h1->Scale(1/h1->Integral());


  TFile *_file2 = TFile::Open("/home/parkerw/Software/rat_master/example/root/alphaneck_thitpmtz.root");

  TH2D* h2 = (TH2D*)_file2->Get("hHitTimeResidualsFit")->Clone("h2");

  h2->RebinY(2);
  h2->GetXaxis()->SetTitle("Hit Time, ns");
  h2->GetYaxis()->SetTitle("PMT Z, mm");
  h2->GetXaxis()->SetRangeUser(100,500);
  h2->SetLineWidth(2);
  h2->SetLineColor(kRed);
  h2->Rebin(5);
  h2->Scale(1/h2->Integral());

  h1->ProjectionX()->SetLineWidth(2);
  h2->ProjectionX()->SetLineWidth(2);
  h1->ProjectionX()->Draw("hist");
  h2->ProjectionX()->Draw("histsame");
  //h2->Draw("colz");

  TLegend* t1 = new TLegend( 0.6, 0.6, 0.8, 0.8 );
  t1->AddEntry( h1, "R < 6.0m", "l" );
  t1->AddEntry( h2, "R > 8.5m", "l" );
  t1->Draw();

  c1->SaveAs("alphathit.pdf");

}
