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

int drawbias() {

  // Read in file
  TFile *infile = TFile::Open("nov9MultiPDF_MultiPath_LogL_XX.root");
  TFile* outfile = new TFile("nov9MultiPDF_MultiPath_LogL_XXPlots.root", "RECREATE");

  std::string names[3] = {"hScaledXPDF", "hScaledYPDF", "hScaledZPDF"};
  std::string titles[3] = {"MultiPDF X", "MultiPDF Y", "MultiPDF Z"};
  std::string axistitles[3] = {"Bias in X, mm", "Bias in Y, mm", "Bias in Z, mm"}; 
  //double canvindex[3] = {1,5,2,6,3,7,4,8};

  gStyle->SetPadRightMargin(0.2);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadBottomMargin(0.2);
  gStyle->SetOptStat(0);
  TCanvas* c1 = new TCanvas("c1", "c1", 1500,400);
  c1->Divide(3);

  for(int i=0; i<3; i++){
    std::cout << "Getting " << names[i] << std::endl;
    TH2D *h0 = (TH2D*)(infile->Get(names[i].c_str()));
    h0->SetTitle(titles[i].c_str());
    h0->GetXaxis()->SetTitle("Scaled LogL");
    h0->GetYaxis()->SetTitle(axistitles[i].c_str());
    h0->GetYaxis()->SetTitleOffset(1.9);
    h0->GetZaxis()->SetTitleOffset(1.6);
    h0->GetZaxis()->SetTitle("Events");
    gStyle->SetPalette(51);
    c1->cd(i+1);
    h0->Draw("colz");
  }
}
