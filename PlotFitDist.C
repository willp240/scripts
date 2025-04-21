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

void PlotFitDist(){

  gStyle->SetPalette(51);
  gStyle->SetLineWidth(2);
  gStyle->SetOptStat(0);

  std::string plotprefix = "alpha_";

  std::string cut1 = "sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)) < 6000 && neckcut == 1 && sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4)) < 6000";
  std::string cut2 = "&& neckHit / nhits < 0.01 && sqrt( (xSeedPosErr-xSeedNegErr)*(xSeedPosErr-xSeedNegErr) + (ySeedPosErr-ySeedNegErr)*(ySeedPosErr-ySeedNegErr) + (zSeedPosErr-zSeedNegErr)*(zSeedPosErr-zSeedNegErr)) < 250 &&  sqrt( (xFitPosErr-xFitNegErr)*(xFitPosErr-xFitNegErr) + (yFitPosErr-yFitNegErr)*(yFitPosErr-yFitNegErr) + (zFitPosErr-zFitNegErr)*(zFitPosErr-zFitNegErr)) < 1200 && sqrt( (xFit-xSeed)*(xFit-xSeed)+(yFit-ySeed)*(yFit-ySeed)+(zFit-zSeed)*(zFit-zSeed) ) < 1000 && scaledLLH > 12.0 && itr > 0.2";


  TFile *fFile = TFile::Open("/home/parkerw/Scripts/Mar26_quad_alpha_tspread_Tree.root");
  TTree* fTree = (TTree*)fFile->Get("eveTree")->Clone("fTree");

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp = new TH2F("htemp","htemp", 140, 0, 5000, 100, 0, 0.1);
  //fTree->Draw("sqrt( (xFitPosErr-xFitNegErr)*(xFitPosErr-xFitNegErr) + (yFitPosErr-yFitNegErr)*(yFitPosErr-yFitNegErr) + (zFitPosErr-zFitNegErr)*(zFitPosErr-zFitNegErr)):sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue)) >> htemp", (cut1.c_str()), "colz");
  fTree->Draw(" (1-(neckHit / nhits)) * (sqrt( (xSeedPosErr-xSeedNegErr)*(xSeedPosErr-xSeedNegErr) + (ySeedPosErr-ySeedNegErr)*(ySeedPosErr-ySeedNegErr) + (zSeedPosErr-zSeedNegErr)*(zSeedPosErr-zSeedNegErr)) / 1000) * (sqrt( (xFitPosErr-xFitNegErr)*(xFitPosErr-xFitNegErr) + (yFitPosErr-yFitNegErr)*(yFitPosErr-yFitNegErr) + (zFitPosErr-zFitNegErr)*(zFitPosErr-zFitNegErr)) / 1000) * (sqrt((xFit-xSeed)*(xFit-xSeed)+(yFit-ySeed)*(yFit-ySeed)+(zFit-zSeed)*(zFit-zSeed) )/1000) * (1-(scaledLLH / 20)) * itr   : sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue) ) >> htemp", (cut1).c_str(), "colz");
  std::string ytitle = "Combined";
  std::string xtitle = "Fit Distance, mm";
  htemp->GetXaxis()->SetTitle(xtitle.c_str());
  htemp->GetYaxis()->SetTitle(ytitle.c_str());
  htemp->SetTitle("");
  //htemp2->GetYaxis()->SetTitleOffset(1.0);
  htemp->GetXaxis()->SetTitleSize(0.05);
  htemp->GetYaxis()->SetTitleSize(0.05);
  c1->SetLogz();
  c1->SaveAs((plotprefix+"vsCombined.pdf").c_str());


  TCanvas* c2 = new TCanvas("c2", "c2", 1500,800);
  c2->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp2 = new TH2F("htemp2","htemp2", 140, 0, 5000, 100, 0, 0.05);
  fTree->Draw("neckHit / nhits:sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue)) >> htemp2", (cut1.c_str()), "colz");
 
  ytitle = "Neck Hits / Nhits";
  xtitle = "Fit Distance, mm";
  htemp2->GetXaxis()->SetTitle(xtitle.c_str());
  htemp2->GetYaxis()->SetTitle(ytitle.c_str());
  htemp2->SetTitle("");
  htemp2->GetXaxis()->SetTitleSize(0.05);
  htemp2->GetYaxis()->SetTitleSize(0.05);
  c2->SetLogz();
  c2->SaveAs((plotprefix+"vsNeckHits.pdf").c_str());


  TCanvas* c3 = new TCanvas("c3", "c3", 1500,800);
  c3->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp3 = new TH2F("htemp3","htemp3", 140, 0, 5000, 100, 0, 500);
  fTree->Draw("sqrt( (xSeedPosErr-xSeedNegErr)*(xSeedPosErr-xSeedNegErr) + (ySeedPosErr-ySeedNegErr)*(ySeedPosErr-ySeedNegErr) + (zSeedPosErr-zSeedNegErr)*(zSeedPosErr-zSeedNegErr)):sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue)) >> htemp3", (cut1.c_str()), "colz");
  ytitle = "Quad Uncertainty, mm";
  xtitle = "Fit Distance, mm";
  htemp3->GetXaxis()->SetTitle(xtitle.c_str());
  htemp3->GetYaxis()->SetTitle(ytitle.c_str());
  htemp3->SetTitle("");
  htemp3->GetXaxis()->SetTitleSize(0.05);
  htemp3->GetYaxis()->SetTitleSize(0.05);
  c3->SetLogz();
  c3->SaveAs((plotprefix+"vsQuadErr.pdf").c_str());

  TCanvas* c4 = new TCanvas("c4", "c4", 1500,800);
  c4->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp4 = new TH2F("htemp4","htemp4", 140, 0, 5000, 100, 0, 5000);
  fTree->Draw("sqrt( (xFitPosErr-xFitNegErr)*(xFitPosErr-xFitNegErr) + (yFitPosErr-yFitNegErr)*(yFitPosErr-yFitNegErr) + (zFitPosErr-zFitNegErr)*(zFitPosErr-zFitNegErr)):sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue)) >> htemp4", (cut1.c_str()), "colz");
  ytitle = "MultiPDF Uncertainty, mm";
  xtitle = "Fit Distance, mm";
  htemp4->GetXaxis()->SetTitle(xtitle.c_str());
  htemp4->GetYaxis()->SetTitle(ytitle.c_str());
  htemp4->SetTitle("");
  htemp4->GetXaxis()->SetTitleSize(0.05);
  htemp4->GetYaxis()->SetTitleSize(0.05);
  c4->SetLogz();
  c4->SaveAs((plotprefix+"vsMultiPDFErr.pdf").c_str());


  TCanvas* c5 = new TCanvas("c5", "c5", 1500,800);
  c5->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp5 = new TH2F("htemp5","htemp5", 140, 0, 5000, 100, 0, 2000);
  fTree->Draw("sqrt( (xFit-xSeed)*(xFit-xSeed)+(yFit-ySeed)*(yFit-ySeed)+(zFit-zSeed)*(zFit-zSeed) ):sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue)) >> htemp5", (cut1.c_str()), "colz");
  ytitle = "MultiPDF - Quad Distance, mm";
  xtitle = "Fit Distance, mm";
  htemp5->GetXaxis()->SetTitle(xtitle.c_str());
  htemp5->GetYaxis()->SetTitle(ytitle.c_str());
  htemp5->SetTitle("");
  htemp5->GetXaxis()->SetTitleSize(0.05);
  htemp5->GetYaxis()->SetTitleSize(0.05);
  c5->SetLogz();
  c5->SaveAs((plotprefix+"vsMultiPDFQuadDist.pdf").c_str());


  TCanvas* c6 = new TCanvas("c6", "c6", 1500,800);
  c6->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp6 = new TH2F("htemp6","htemp6", 140, 0, 5000, 100, 10, 14);
  fTree->Draw("scaledLLH:sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue)) >> htemp6", (cut1.c_str()), "colz");
  ytitle = "MultiPDF FOM / Nhits";
  xtitle = "Fit Distance, mm";
  htemp6->GetXaxis()->SetTitle(xtitle.c_str());
  htemp6->GetYaxis()->SetTitle(ytitle.c_str());
  htemp6->SetTitle("");
  htemp6->GetXaxis()->SetTitleSize(0.05);
  htemp6->GetYaxis()->SetTitleSize(0.05);
  c6->SetLogz();
  c6->SaveAs((plotprefix+"vsScaledLLH.pdf").c_str());

  TCanvas* c7 = new TCanvas("c7", "c7", 1500,800);
  c7->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp7 = new TH2F("htemp7","htemp7", 140, 0, 5000, 100, 0, 0.5);
  fTree->Draw("itr:sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue)) >> htemp7", (cut1.c_str()), "colz");
  ytitle = "ITR";
  xtitle = "Fit Distance, mm";
  htemp7->GetXaxis()->SetTitle(xtitle.c_str());
  htemp7->GetYaxis()->SetTitle(ytitle.c_str());
  htemp7->SetTitle("");
  htemp7->GetXaxis()->SetTitleSize(0.05);
  htemp7->GetYaxis()->SetTitleSize(0.05);
  c7->SetLogz();
  c7->SaveAs((plotprefix+"vsITR.pdf").c_str());
  
}

