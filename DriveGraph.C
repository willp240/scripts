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


void DriveGraph(){
  
  gStyle->SetOptStat(1);
  {
    Int_t n = 6;
    Double_t x[6] = {1,2,2.5,5,7,10};
    Double_t xerr[6] = {0,0,0,0,0,0};
    //Double_t y[6] = {38.84, 54.19, 56.48, 74.68, 84.78, 97.62};
    //Double_t yerr[6] = {0.28, 0.20, 0.18, 0.15, 0.16, 0.19};
    Double_t y[6] = {44.43, 59.71, -999, 80.99, 91.37, 104.50};
    Double_t yerr[6] = {0.28, 0.20, 0.18, 0.13, 0.12, 0.10};
    // create graph
    TGraphErrors *gr  = new TGraphErrors(n,x,y,xerr,yerr);
    gr->SetTitle("Drive vs Energy");
    gr->SetMarkerStyle(2);
    gr->SetMarkerColor(kBlue);
    gr->SetLineColor(kBlue);

    Double_t x2[6] = {1,2,2.5,5,7,10};
    Double_t xerr2[6] = {0,0,0,0,0,0};
    //    Double_t y2[6] = {2.72,5.00,6.67,13.96,20.47,31.55};
    //Double_t yerr2[6] = {0.27,0.19,0.17,0.15,0.17,0.20};
    Double_t y2[6] = {2.58, 5.51, -9999, 14.07, 20.25, 29.44};
    Double_t yerr2[6] = {0.28, 0.19, 0.17, 0.12, 0.11, 0.10};
    // create graph
    TGraphErrors *gr2  = new TGraphErrors(n,x2,y2,xerr2,yerr2);
    gr2->SetMarkerStyle(2);
    gr2->SetMarkerColor(kRed);
    gr2->SetLineColor(kRed);

    TCanvas *c1 = new TCanvas("c1","", 200,10,600,400);

    gr->Draw("AP");
    gr2->Draw("same P");

    TLegend* legend = new TLegend(0.1,0.7,0.48,0.9);
    legend->AddEntry(gr,"With Cerenkov","p");
    legend->AddEntry(gr2,"No Cerenkov","p");
    legend->Draw();

    Double_t x3[6] = {1,2,2.5,5,7,10};
    Double_t xerr3[6] = {0,0,0,0,0,0};
    Double_t y3[6];
    Double_t yerr3[6];
    for(int i=0; i<6; i++){
      y3[i] = y[i] - y2[i];
      yerr3[i] = sqrt(yerr[i]*yerr[i] + yerr2[i]*yerr2[i]);
    }
    // create graph
    //    TGraphErrors *gr3  = new TGraphErrors(n,x3,y3,xerr3,yerr3);
    //    gr3->SetMarkerStyle(2);
    //gr3->SetMarkerColor(kBlack);
    //gr3->SetLineColor(kBlack);

    //gr3->Draw("AP");

    TF1 *f1 = new TF1("f1", "2.5*x", 0,10);
    f1->SetLineColor(kBlack);
    f1->SetLineStyle(2);
    f1->Draw("same");
    gr->GetYaxis()->SetRangeUser(0,120);
    gr->GetXaxis()->SetTitle("Event Energy, MeV");
    gr->GetYaxis()->SetTitle("Mean Drive in X, mm");
  }

}
