#include <TH1D.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"
#include <algorithm>
#include <cctype>
#include <string>
#include <iostream>

#include <string>


void plotLLH(std::string filename) {

  TFile *_file0 = TFile::Open(filename.c_str());
  TTree* t1 = (TTree*)_file0->Get("t1");
  //  TH2F *h1 = new TH2F("h1","h1",40,-537.091-2000,-537.091+2000,40,5853.63-2000,5853.63+2000);
  TH2F *h1 = new TH2F("h1", "h1", 40, -10000, 10000, 40, -10000, 10000);
  float fx, fy, fz, L;

  TCanvas *c1 = new TCanvas("c1","c1",800,600);
  c1->SetRightMargin(0.13);
  gStyle->SetOptStat(0);
  
  t1->SetBranchAddress("fx",&fx);
  t1->SetBranchAddress("fy",&fy);
  t1->SetBranchAddress("fz",&fz);
  t1->SetBranchAddress("L",&L);

  for(int i=0; i<t1->GetEntries();i++){
    t1->GetEntry(i);
    if(fy==0)
      h1->Fill(fx,fz,L);
  }
  //  h1->GetXaxis()->SetRangeUser(-6000,6000);
  //h1->GetYaxis()->SetRangeUser(-6000,6000);
  h1->GetYaxis()->SetTitleOffset(1.3);
  h1->GetXaxis()->SetTitle("X, mm");
  h1->GetYaxis()->SetTitle("Z, mm");
  h1->Draw("colz");
  //  h1->ProjectionY()->Draw();
}

