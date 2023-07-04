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
#include <TGraph.h>

#include <string>


void PlotNhitvsE(){
  
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TCanvas* c1 = new TCanvas("c1", "c1", 800,600);
  gPad->SetGrid(1);

  Int_t n = 4;
  Double_t x[4] = {2.5,5.0,7.5,10.0};
  Double_t xerr[4] = {0,0,0,0};
  
  Double_t y[6] = {782.4, 1484.3, 2111.3, 2673.8};
  Double_t yerr[6] = {0.1, 0.2, 0.2, 0.1};
  // create graph
  TGraphErrors *gr  = new TGraphErrors(n,x,y,xerr,yerr);
  gr->SetTitle("Nhits vs Energy");
  gr->SetMarkerStyle(1);
  gr->SetMarkerColor(kBlue);
  gr->SetLineColor(kBlue);
  gr->SetMarkerSize(2);
  gr->SetLineWidth(2);

  gr->GetXaxis()->SetRangeUser(0,12);
  gr->GetYaxis()->SetRangeUser(0,3000);
  
  gr->GetXaxis()->SetTitle("Event Energy, MeV");
  gr->GetYaxis()->SetTitle("Nhits");
  gr->GetYaxis()->SetTitleOffset(1.3);

  TF1 *g1 = new TF1("g1","pol1");
  g1->SetLineWidth(2);
  g1->SetLineColor(kRed);
  gr->Fit("g1");

  gr->Draw("AP");

}
