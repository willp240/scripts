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


void plotratios(){

  gStyle->SetPalette(51);
  gStyle->SetLineWidth(2);
  gStyle->SetOptStat(0);

  TFile *fFile = TFile::Open("/home/parkerw/Software/rat-tools_fresh/FitCoordination/NearAVAngular/Coordinate_DebuggingPlots_Done.root");
  
  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  c1->Divide(2,2);

  for(int i=0; i<4; i++){
    c1->cd(i+1);
    gPad->SetFrameLineWidth(2);
    gPad->SetGrid(1);

    TString hname = Form("ratioVsRadius_nhits%d",i*300);
    TString htitle = Form("%d - %d Nhits", i*300, (i+1)*300);
    TGraph* g1 = (TGraph*)fFile->Get(hname);
    g1->GetYaxis()->SetRangeUser(0,0.7);
    g1->GetXaxis()->SetRangeUser(5200, 6100);
    g1->GetYaxis()->SetTitle("NearAV Ratio");
    g1->SetTitle(htitle);
    g1->SetLineWidth(2);
    g1->SetMarkerColor(kBlue);
    g1->Draw("AP");
  }

  TCanvas* c2 = new TCanvas("c2", "c2", 1500,800);
  c2->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  c2->Divide(2,2);

  for(int i=0; i<4; i++){
    c2->cd(i+1);
    gPad->SetFrameLineWidth(2);
    gPad->SetGrid(1);

    TString hname = Form("ratioVsRadius_nhits%d",(i+4)*300);
    TString htitle = Form("%d - %d Nhits", (i+4)*300, (i+5)*300);
    TGraph* g1 = (TGraph*)fFile->Get(hname);
    g1->GetYaxis()->SetRangeUser(0,0.7);
    g1->GetXaxis()->SetRangeUser(5200, 6100);
    g1->GetYaxis()->SetTitle("NearAV Ratio");
    g1->SetTitle(htitle);
    g1->SetLineWidth(2);
    g1->SetMarkerColor(kBlue);
    g1->Draw("AP");
  }

  TCanvas* c3 = new TCanvas("c3", "c3", 1500,800);
  c3->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  c3->Divide(2,2);

  for(int i=0; i<4; i++){
    c3->cd(i+1);
    gPad->SetFrameLineWidth(2);
    gPad->SetGrid(1);

    TString hname = Form("ratioVsRadius_nhits%d",(i+8)*300);
    TString htitle = Form("%d - %d Nhits", (i+8)*300, (i+9)*300);
    TGraph* g1 = (TGraph*)fFile->Get(hname);
    g1->GetYaxis()->SetRangeUser(0,0.7);
    g1->GetXaxis()->SetRangeUser(5200, 6100);
    g1->GetYaxis()->SetTitle("NearAV Ratio");
    g1->SetTitle(htitle);
    g1->SetLineWidth(2);
    g1->SetMarkerColor(kBlue);
    g1->Draw("AP");
  }

  TCanvas* c4 = new TCanvas("c4", "c4", 1500,800);
  c4->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  c4->Divide(2,2);

  for(int i=0; i<2; i++){
    c4->cd(i+1);
    gPad->SetFrameLineWidth(2);
    gPad->SetGrid(1);

    TString hname = Form("ratioVsRadius_nhits%d",(i+12)*300);
    TString htitle = Form("%d - %d Nhits", (i+12)*300, (i+13)*300);
    TGraph* g1 = (TGraph*)fFile->Get(hname);
    g1->GetYaxis()->SetRangeUser(0,0.7);
    g1->GetXaxis()->SetRangeUser(5200, 6100);
    g1->GetYaxis()->SetTitle("NearAV Ratio");
    g1->SetTitle(htitle);
    g1->SetLineWidth(2);
    g1->SetMarkerColor(kBlue);
    g1->Draw("AP");
  }
  
  c1->SaveAs("nhitratio_1.pdf");
  c2->SaveAs("nhitratio_2.pdf");
  c3->SaveAs("nhitratio_3.pdf");
  c4->SaveAs("nhitratio_4.pdf");
}

