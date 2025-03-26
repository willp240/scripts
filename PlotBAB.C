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

void PlotBAB(){

  gStyle->SetPalette(51);
  gStyle->SetLineWidth(2);
  gStyle->SetOptStat(0);

  std::string plotprefix = "bab_";


  std::string cut1 = "sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)) < 6000 && neckcut == 1";
  std::string cut2 = " && sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4)) < 6000 && babretrigger == 0";

  TFile *fFile = TFile::Open("/home/parkerw/Scripts/Feb9_e_Tree.root"); 
  TTree* fTree = (TTree*)fFile->Get("eveTree")->Clone("fTree");
  TFile *fFileAlpha = TFile::Open("/home/parkerw/Scripts/Feb9_alpha_Tree.root");
  TTree* fTreeAlpha = (TTree*)fFileAlpha->Get("eveTree")->Clone("fTree");

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->SetRightMargin(0.13);
  c1->Divide(2,2);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);  


  TH1F *htemp = new TH1F("htemp","htemp", 20, -0.1, 0.1);
  fTree->Draw("bab/babnhits >> htemp", (cut1+cut2).c_str(), "");
  htemp->SetLineColor(kBlue);
  
  TH1F *htempAlpha = new TH1F("htempAlpha","htempAlpha", 20, -0.1, 0.1);
  fTreeAlpha->Draw("bab/babnhits >> htempAlpha", (cut1+cut2).c_str(), "same");
  htempAlpha->SetLineColor(kRed);
  
  std::string xtitle = "BAB";
  std::string ytitle = "Events";
  htemp->GetXaxis()->SetTitle(xtitle.c_str());
  htemp->GetYaxis()->SetTitle(ytitle.c_str());
  htemp->SetTitle("");

  htemp->GetXaxis()->SetTitleSize(0.05);
  htemp->GetYaxis()->SetTitleSize(0.05);
  c1->SaveAs((plotprefix+".pdf").c_str());


}
