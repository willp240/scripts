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


void DrivePlotter(){

  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Mar6_dirpos_fitposPDF_fitPos_20bins25eve+Tree.root");
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree1");

  TCanvas* c1 = new TCanvas("c1", "c1", 1000,600);

  tree1->Draw("( ((xFit-xTrue)*xdirTrue) + ((yFit-yTrue)*ydirTrue) + ((zFit-zTrue)*zdirTrue))/(sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue))");
  TH1F *htemp1 = (TH1F*)gPad->GetPrimitive("htemp");
  htemp1->GetXaxis()->SetTitle("Drive, mm");
  htemp1->GetYaxis()->SetTitle("Events");
  htemp1->GetYaxis()->SetTitleOffset(1.3);
  htemp1->GetXaxis()->SetRangeUser(-800,800);
  htemp1->SetTitle("6 MeV e- at Center (Random Directions): True Position in PDF, Fit Position Seed");
  htemp1->SetLineColor(kGreen);
  htemp1->SetLineWidth(2);
  //  TPaveText *title = (TPaveText*)gPad->GetPrimitive("title");
  // title->SetBorderSize(2);
  //gPad->Update();
}

void PlotCorr(){

  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetPalette(55);

  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Feb21_dirpos_trueposPDF_fitPos_smalltresid_Tree.root");
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree1");

  TCanvas* c1 = new TCanvas("c1", "c1", 1000,600);

  tree1->Draw("(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) ): ( ((xFit-xTrue)*xdirTrue) + ((yFit-yTrue)*ydirTrue) + ((zFit-zTrue)*zdirTrue))/(sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue))","","colz");
  TH1F *htemp1 = (TH1F*)gPad->GetPrimitive("htemp");
  htemp1->GetXaxis()->SetTitle("Drive, mm");
  htemp1->GetYaxis()->SetTitle("cos #alpha");
  htemp1->GetYaxis()->SetTitleOffset(1.3);
  htemp1->GetXaxis()->SetRangeUser(-800,800);
  htemp1->GetYaxis()->SetRangeUser(-1,1);
  htemp1->SetTitle("6 MeV e- at Center (Random Directions): True Position in PDF, Fit Position Seed");
  htemp1->SetLineColor(kGreen);
  htemp1->SetLineWidth(2);
  //  TPaveText *title = (TPaveText*)gPad->GetPrimitive("title");
  // title->SetBorderSize(2);
  //gPad->Update();
}
