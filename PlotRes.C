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


void PlotRes(){
  
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/mpdf_over6to8500_Tree.root");
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree");

  TCanvas* c1 = new TCanvas("c1", "c1", 800,600);
  c1->SetLeftMargin(0.13);
  c1->SetGrid();

  tree1->Draw("sqrt(xFit*xFit+yFit*yFit+zFit*zFit) - sqrt(xTrue*xTrue+yTrue*yTrue+zTrue*zTrue)");
  TH1F *htemp1 = (TH1F*)gPad->GetPrimitive("htemp");
  htemp1->GetXaxis()->SetTitle("Fitted - True Radius, mm");
  htemp1->GetYaxis()->SetTitle("Events");
  htemp1->GetYaxis()->SetTitleOffset(1.4);
  htemp1->GetXaxis()->SetRangeUser(-12000,12000);
  htemp1->SetTitle("Radial Resolution");
  //  htemp1->SetTitle("Scattering: Joint Fit");
  htemp1->SetLineColor(kBlue+2);
  htemp1->SetLineWidth(2);
  //  TPaveText *title = (TPaveText*)gPad->GetPrimitive("title");
  // title->SetBorderSize(2);
  //gPad->Update();
}
