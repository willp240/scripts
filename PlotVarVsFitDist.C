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

void PlotVarVsFitDist(){

  gStyle->SetPalette(51);
  gStyle->SetLineWidth(2);
  gStyle->SetOptStat(0);

  std::string var = "scaledLLH";
  std::string ytitle = "Scaled LLH";
  std::string pdfname = "scaledllh";
 
  std::string cut1 = "sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)) < 6000 && neckcut == 1";
  std::string cut2 = "&& sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4)) < 6000";// "&& neckHit / nhits < 0.01 && sqrt( (xSeedPosErr-xSeedNegErr)*(xSeedPosErr-xSeedNegErr) + (ySeedPosErr-ySeedNegErr)*(ySeedPosErr-ySeedNegErr) + (zSeedPosErr-zSeedNegErr)*(zSeedPosErr-zSeedNegErr)) < 250 &&  sqrt( (xFitPosErr-xFitNegErr)*(xFitPosErr-xFitNegErr) + (yFitPosErr-yFitNegErr)*(yFitPosErr-yFitNegErr) + (zFitPosErr-zFitNegErr)*(zFitPosErr-zFitNegErr)) < 900 && sqrt( (xFit-xSeed)*(xFit-xSeed)+(yFit-ySeed)*(yFit-ySeed)+(zFit-zSeed)*(zFit-zSeed) ) < 1000 && scaledLLH > 12.0 && itr > 0.2";
  //std::string cut2 = "&&  neckHit / nhits < 0.01 && sqrt( (xSeedPosErr-xSeedNegErr)*(xSeedPosErr-xSeedNegErr) + (ySeedPosErr-ySeedNegErr)*(ySeedPosErr-ySeedNegErr) + (zSeedPosErr-zSeedNegErr)*(zSeedPosErr-zSeedNegErr)) < 250 && sqrt( (xFitPosErr-xFitNegErr)*(xFitPosErr-xFitNegErr) + (yFitPosErr-yFitNegErr)*(yFitPosErr-yFitNegErr) + (zFitPosErr-zFitNegErr)*(zFitPosErr-zFitNegErr)) < 900 && sqrt( (xFit-xSeed)*(xFit-xSeed)+(yFit-ySeed)*(yFit-ySeed)+(zFit-zSeed)*(zFit-zSeed) ) < 1000 ";//"&& scaledLLH > 12.0 && neckHit/nhits < 0.01 && itr > 0.2 && highOwl / nhits < 0.01";
  // std::string cut2 = "&& scaledLLH > 12.0 && neckHit/nhits < 0.01 && itr > 0.2 && highOwl / nhits < 0.01";

  TFile *eFile = TFile::Open("/home/parkerw/Scripts/Mar26_quad_e_tspread_Tree.root");
  TTree* eTree = (TTree*)eFile->Get("eveTree")->Clone("eTree");

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->SetRightMargin(0.13);
  c1->Divide(2,2);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);  

  TH2F *htemp = new TH2F("htemp","htemp", 100, 0, 5000, 100, 5, 14);
  std::string drawcommand = var + ":sqrt((xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue)) >> htemp";
  eTree->Draw(drawcommand.c_str(), (cut1+cut2).c_str(), "colz");

  std::string xtitle = "Fit - True Position, mm";
  htemp->GetXaxis()->SetTitle(xtitle.c_str());
  htemp->GetYaxis()->SetTitle(ytitle.c_str());
  htemp->SetTitle("");
  htemp->GetYaxis()->SetTitleOffset(0.8);
  htemp->GetXaxis()->SetTitleOffset(0.8);
  htemp->GetXaxis()->SetTitleSize(0.06);
  htemp->GetYaxis()->SetTitleSize(0.06);
  c1->SetLogz();
  c1->SaveAs(("e_"+pdfname+"_fitdist.pdf").c_str());

  TCanvas* c2 = new TCanvas("c2", "c2", 1500,800);
  c2->SetRightMargin(0.13);
  c2->Divide(2,2);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  
  TFile *alphaFile = TFile::Open("/home/parkerw/Scripts/Mar26_quad_alpha_tspread_Tree.root");
  TTree* alphaTree = (TTree*)alphaFile->Get("eveTree")->Clone("alphaTree");

  TH2F *htemp2 = new TH2F("htemp2","htemp2", 100, 0, 5000, 100, 5, 14);
  drawcommand = var + ":sqrt((xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue)) >> htemp2";
  alphaTree->Draw(drawcommand.c_str(), (cut1+cut2).c_str(), "colz");

  htemp2->GetXaxis()->SetTitle(xtitle.c_str());
  htemp2->GetYaxis()->SetTitle(ytitle.c_str());
  htemp2->SetTitle("");
  htemp2->GetYaxis()->SetTitleOffset(0.8);
  htemp2->GetXaxis()->SetTitleOffset(0.8);
  htemp2->GetXaxis()->SetTitleSize(0.06);
  htemp2->GetYaxis()->SetTitleSize(0.06);
  c2->SetLogz();
  c2->SaveAs(("alpha_"+pdfname+"_fitdist.pdf").c_str());
}
