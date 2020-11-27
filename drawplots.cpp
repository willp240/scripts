#include "TMath.h"
#include <TH2D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <iostream>
#include <vector>
#include <exception>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "TLegend.h"

int drawplots() {

  // Read in file
  TFile *infile = TFile::Open("nov9MultiPDF_MultiPath_LogL_XX.root");
  TFile* outfile = new TFile("nov9MultiPDF_MultiPath_LogL_XXPlots.root", "RECREATE");

  std::string names[8] = {"hScaledXPath", "hScaledXPDF", "hScaledYPath", "hScaledYPDF", "hScaledZPath", "hScaledZPDF", "hScaledRPath", "hScaledRPDF"};
  std::string titles[8] = {"MultiPath X", "MultiPDF X", "MultiPath Y", "MultiPDF Y", "MultiPath Z", "MultiPDF Z", "MultiPath R", "MultiPDF R",};
  std::string axistitles[8] = {"X, mm", "X, mm", "Y, mm", "Y, mm", "Z, mm", "Z, mm", "R, mm", "R, mm"}; 
  double canvindex[8] = {1,5,2,6,3,7,4,8};

  gStyle->SetPadRightMargin(0.18);
  gStyle->SetPadLeftMargin(0.13);
  gStyle->SetPadBottomMargin(0.18);
  gStyle->SetOptStat(0);
  TCanvas* c1 = new TCanvas("c1", "c1", 1500,700);
  c1->Divide(4,2);

  for(int i=0; i<8; i++){
    std::cout << "Getting " << names[i] << std::endl;
    TH2D *h0 = (TH2D*)(infile->Get(names[i].c_str()));
    h0->SetTitle(titles[i].c_str());
    h0->GetXaxis()->SetTitle("Scaled LogL");
    h0->GetYaxis()->SetTitle(axistitles[i].c_str());
    h0->GetYaxis()->SetTitleOffset(1.6);
    h0->GetZaxis()->SetTitleOffset(1.6);
    if(i>3||i<6)
      h0->GetYaxis()->SetRangeUser(0,6000);
    h0->GetZaxis()->SetTitle("Events");
    gStyle->SetPalette(51);
    c1->cd(canvindex[i]);
    h0->Draw("colz");
  }
}
