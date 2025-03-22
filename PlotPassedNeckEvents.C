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


void PlotPassedNeckEvents(){

  gStyle->SetPalette(51);
  gStyle->SetLineWidth(2);
  gStyle->SetOptStat(0);

  std::string plotprefix = "alpha_energy_passed_";

  std::string axs[4] = {"xFit:xTrue", "yFit:yTrue", "zFit-184.4:zTrue-184.4", "sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)):sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4))"};
  std::string title = "Reconstructed";

  std::string coord[4] = {"X", "Y", "Z", "R"};

  std::string cut1 = "sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)) < 6000 && neckcut == 1";
  std::string cut2 = "&& scaledLLH > 12.0 && neckHit/nhits < 0.01 && itr > 0.2 && highOwl / nhits < 0.01";//"&& scaledLLH > 12.0 && neckHit/nhits < 0.01 && nearAV > 0.1 && itr > 0.18 && highOwl/nhits < 0.005";
  
  TFile *fFile = TFile::Open("/home/parkerw/Scripts/Feb5_alpha_neckcuttree.root");
  //TFile *fFile = TFile::Open("/home/parkerw/Scripts/Jan8_NearAV_neckcuttree.root");
  //TFile *fFile = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/Dec21_alpha_owlTree.root");
  TTree* fTree = (TTree*)fFile->Get("eveTree")->Clone("fTree");

 
  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->SetRightMargin(0.13);
  c1->Divide(2,2);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);  

  TH2F *htemp = new TH2F("htemp","htemp", 140, 0, 14000, 100, 0, 10);
  fTree->Draw("ETrue:sqrt(xTrue*xTrue + yTrue*yTrue + (zTrue-184.4)*(zTrue-184.4)) >> htemp", (cut1+cut2).c_str(), "colz");
  std::string xtitle = "True Radius, mm";
  std::string ytitle = "True Energy, MeV";
  htemp->GetXaxis()->SetTitle(xtitle.c_str());
  htemp->GetYaxis()->SetTitle(ytitle.c_str());
  htemp->SetTitle("");
  htemp->GetXaxis()->SetTitleSize(0.05);
  htemp->GetYaxis()->SetTitleSize(0.05);
  c1->SetLogz();
  c1->SaveAs((plotprefix+"vsTrueR.pdf").c_str());
  
}

