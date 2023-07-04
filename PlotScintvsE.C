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


void PlotScintvsE(){
  
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TCanvas* c1 = new TCanvas("c1", "c1", 800,600);
  gPad->SetGrid(1);

  Int_t n = 4;
  Double_t x[4] = {2.5,5.0,7.5,10.0};
  Double_t xerr[4] = {0,0,0,0};
  
  // Double_t y[6] = {185.954830522, 185.25569251, 184.486478183, 183.710922616};
  //Double_t yerr[6] = {0.326,0.346,0.396,0.456};
  Double_t y[6] = {185.252, 184.430, 183.609, 182.829 };
  Double_t yerr[6] = {0.326,0.346,0.396,0.456};
  // create graph
  TGraphErrors *gr  = new TGraphErrors(n,x,y,xerr,yerr);
  gr->SetTitle("Effective Velocity vs Energy");
  gr->SetMarkerStyle(1);
  gr->SetMarkerColor(kBlue);
  gr->SetLineColor(kBlue);
  gr->SetMarkerSize(2);
  gr->SetLineWidth(2);

  gr->GetXaxis()->SetRangeUser(0,12);
  gr->GetYaxis()->SetRangeUser(180,190);
  
  gr->GetXaxis()->SetTitle("Event Energy, MeV");
  gr->GetYaxis()->SetTitle("Effective Velocity mm/ns");

  TF1 *g1 = new TF1("g1","pol1");
  g1->SetLineWidth(2);
  g1->SetLineColor(kRed);
  gr->Fit("g1");

  gr->Draw("AP");

}
