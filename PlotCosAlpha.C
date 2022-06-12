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


void PlotCosAlpha(){

  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Jun7_6MeV_fullvolume_6MeV_wScattering_jointFit_Tree.root");
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree1");

  TCanvas* c1 = new TCanvas("c1", "c1", 1000,600);

  tree1->Draw("(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) )");
  TH1F *htemp1 = (TH1F*)gPad->GetPrimitive("htemp");
  htemp1->GetXaxis()->SetTitle("cos (#alpha)");
  htemp1->GetYaxis()->SetTitle("Events");
  htemp1->GetYaxis()->SetTitleOffset(1.3);
  htemp1->GetXaxis()->SetRangeUser(-1,1);
  //htemp1->SetTitle("6 MeV e- at Center (Random Directions) 2g/l: True Position in PDF, Fit Position Seed");
  htemp1->SetTitle("Scattering: Joint Fit");
  htemp1->SetLineColor(kGreen);
  htemp1->SetLineWidth(2);
  //  TPaveText *title = (TPaveText*)gPad->GetPrimitive("title");
  // title->SetBorderSize(2);
  //gPad->Update();
}

void RCosAlpha(){

  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/May13_scaled1p0_centred_fit_Tree.root");
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree1");

  TCanvas* c1 = new TCanvas("c1", "c1", 1000,600);

  tree1->Draw("sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) : (xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) )", "", "colz");
  TH2F *htemp1 = (TH2F*)gPad->GetPrimitive("htemp");
  htemp1->GetXaxis()->SetTitle("cos (#alpha)");
  htemp1->GetYaxis()->SetTitle("True Radius, mm");
  htemp1->GetYaxis()->SetTitleOffset(1.3);
  htemp1->GetXaxis()->SetRangeUser(-1,1);
  htemp1->GetXaxis()->SetRangeUser(0,6000);
  //htemp1->SetTitle("6 MeV e- at Center (Random Directions) 2g/l: True Position in PDF, Fit Position Seed");
  htemp1->SetTitle("6 MeV e- at Random Positions (Random Directions) Scaled 2g/l: Direction Only Fit (Fit Position in PDF)");
  htemp1->SetLineColor(kGreen);
  htemp1->SetLineWidth(2);
  //  TPaveText *title = (TPaveText*)gPad->GetPrimitive("title");
  // title->SetBorderSize(2);
  //gPad->Update();
}
