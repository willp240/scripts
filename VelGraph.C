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


void VelGraph(){
  
  gStyle->SetOptStat(0);

  Int_t n = 5;
  //Double_t x[12] = {250, 750, 1250, 1750, 2250, 2750, 3250, 3750, 4250, 4750, 5250, 5759};
  //Double_t xerr[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
  Double_t x[5] = {1000, 2500, 3500, 4500, 5500};
  Double_t xerr[5] = {1000,500,500,500,500};
  
  //Double_t y[12] = {159.13, 180.15, 184.54, 185.76, 186.32, 186.37, 186.35, 186.19, 186.00, 185.812, 185.48, 184.9};
  //Double_t yerr[12] = {0,0,0,0,0,0,0,0,0,0,0,0 };
  Double_t y[5] = {184.695, 185.907, 185.773, 185.422, 184.674 };
  Double_t yerr[5] = {0,0,0,0,0 };
  
  //  Double_t y2[12] = {160.75, 180.73, 184.61, 185.49, 185.75, 185.80, 185.63, 185.42, 185.09, 184.68, 184.17, 183.65};
  Double_t y2[5] = {183.230, 183.819, 183.288, 182.270, 181.424 };
  //  Double_t y3[12] = {162.41, 180.35, 183.54, 184.32, 184.57, 184.46, 184.16, 183.85, 183.36, 182.69, 182.22, 182.01};
  
  // create graph
  TGraphErrors *gr  = new TGraphErrors(n,x,y,xerr,yerr);
  gr->SetTitle("Effective Velocity vs Radius");
  gr->SetMarkerStyle(2);
  gr->SetMarkerColor(kBlue+2);
  gr->SetLineColor(kBlue+2);
  gr->SetMarkerSize(2);
  
  TGraphErrors *gr2  = new TGraphErrors(n,x,y2,xerr,yerr);
  gr2->SetTitle("Effective Velocity vs Radius");
  gr2->SetMarkerStyle(2);
  gr2->SetMarkerColor(kGreen+2);
  gr2->SetLineColor(kGreen+2);
  gr2->SetMarkerSize(2);
  
  //  TGraphErrors *gr3  = new TGraphErrors(n,x,y3,xerr,yerr);
  //gr3->SetTitle("Effective Velocity vs Radius");
  //gr3->SetMarkerStyle(2);
  //gr3->SetMarkerColor(kRed+2);
  //gr3->SetLineColor(kRed+2);
  //gr3->SetMarkerSize(2);
  
  
  TCanvas *c1 = new TCanvas("c1","", 200,10,600,400);
  gPad->SetGrid(1);
  gPad->SetFrameLineWidth(2);
    
  TLine* curr_vel = new TLine(0, 186.63, 6600, 186.63);
  curr_vel->SetLineStyle(2);
  curr_vel->SetLineColor(kBlack);
  curr_vel->SetLineWidth(2);
  
  gr->Draw("AP");
  gr2->Draw("P same");
  //gr3->Draw("P same");
  
  curr_vel->Draw("same");
    
  gr->GetXaxis()->SetTitle("Radius, mm");
  gr->GetYaxis()->SetTitle("Effective Velocity, mm/ns");
  gr->GetYaxis()->SetRangeUser(150,190);
  
  TLegend* t1 = new TLegend( 0.3, 0.3, 0.6, 0.5 );
  t1->AddEntry( gr, "2.5 MeV", "l" );
  t1->AddEntry( gr2, "10.0 MeV", "l" );
  //t1->AddEntry( gr3, "10.0 MeV", "l");
  t1->AddEntry( curr_vel, "Current RAT Velocity", "l");
  t1->SetLineWidth(2);
  t1->Draw("same");
}
