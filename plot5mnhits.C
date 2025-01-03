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


void plot5mnhits(){

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

    TString hname = Form("ratioValues_nhits%d",i*300);
    TString htitle = Form("%d - %d Nhits", i*300, (i+1)*300);
    TH1D* h1 = (TH1D*)fFile->Get(hname);
    h1->GetXaxis()->SetRangeUser(0.1,0.7);
    h1->GetXaxis()->SetTitle("NearAV Ratio");
    h1->SetTitle(htitle);
    h1->SetLineWidth(2);
    h1->Draw();
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

    TString hname = Form("ratioValues_nhits%d",(i+4)*300);
    TString htitle = Form("%d - %d Nhits", (i+4)*300, (i+5)*300);
    TH1D* h1 = (TH1D*)fFile->Get(hname);
    h1->GetXaxis()->SetRangeUser(0.1,0.7);
    h1->GetXaxis()->SetTitle("NearAV Ratio");
    h1->SetTitle(htitle);
    h1->SetLineWidth(2);
    h1->Draw();
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

    TString hname = Form("ratioValues_nhits%d",(i+8)*300);
    TString htitle = Form("%d - %d Nhits", (i+8)*300, (i+9)*300);
    TH1D* h1 = (TH1D*)fFile->Get(hname);
    h1->GetXaxis()->SetRangeUser(0.1,0.7);
    h1->GetXaxis()->SetTitle("NearAV Ratio");
    h1->SetTitle(htitle);
    h1->SetLineWidth(2);
    h1->Draw();
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

    TString hname = Form("ratioValues_nhits%d",(i+12)*300);
    TString htitle = Form("%d - %d Nhits", (i+12)*300, (i+13)*300);
    TH1D* h1 = (TH1D*)fFile->Get(hname);
    h1->GetXaxis()->SetRangeUser(0.1,0.7);
    h1->GetXaxis()->SetTitle("NearAV Ratio");
    h1->SetTitle(htitle);
    h1->SetLineWidth(2);
    h1->Draw();
  }
  
  c1->SaveAs("nhit5m_1.pdf");
  c2->SaveAs("nhit5m_2.pdf");
  c3->SaveAs("nhit5m_3.pdf");
  c4->SaveAs("nhit5m_4.pdf");
}

