#include <TH2F.h>
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

void RCosAlpha(){

  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetPalette(55);

  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/May12_2p0_dist_distpdf5.0m_Tree.root");
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree1");

  TCanvas* c1 = new TCanvas("c1", "c1", 1000,600);
  c1->SetRightMargin(0.13);
  tree1->Draw("sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) : (xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) )", "", "colz");
  TH2F *htemp1 = (TH2F*)gPad->GetPrimitive("htemp");
  htemp1->GetXaxis()->SetTitle("cos (#alpha)");
  htemp1->GetYaxis()->SetTitle("True Radius, mm");
  htemp1->GetYaxis()->SetTitleOffset(1.3);
  htemp1->GetXaxis()->SetRangeUser(-1.0,1.0);
  htemp1->GetYaxis()->SetRangeUser(0,5000);
  //htemp1->SetTitle("6 MeV e- at Center (Random Directions) 2g/l: True Position in PDF, Fit Position Seed");
  htemp1->SetTitle("6 MeV e- at Random Positions (Random Directions) 0.5g/l: Direction Only Fit (Fit Position in PDF)");
 
  //  TPaveText *title = (TPaveText*)gPad->GetPrimitive("title");
  // title->SetBorderSize(2);
  //gPad->Update();
}
