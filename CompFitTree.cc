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


void CompFitTree(){
  
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetPalette(51);

  //TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/Oct12_ReprocBiPo_compTree.root");
  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/Oct12MPDF2vsSF_compTree.root");
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree");

  TCanvas* c1 = new TCanvas("c1", "c1", 800,600);
  c1->SetRightMargin(0.14);
  c1->SetLeftMargin(0.12);
  c1->SetGrid();

  //tree1->Draw("scaledLLHMPDF : sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue)", "", "colz");
  //  tree1->Draw("  sqrt((xMPDF-xSF)*(xMPDF-xSF) + (yMPDF-ySF)*(yMPDF-ySF) + (zMPDF-zSF)*(zMPDF-zSF)) : sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue)", " sqrt(xMPDF*xMPDF + yMPDF*yMPDF + zMPDF*zMPDF) < 10000 && sqrt(xSF*xSF + ySF*ySF + zSF*zSF) < 10000 ", "colz");
  tree1->Draw("sqrt(xMPDF*xMPDF + yMPDF*yMPDF + zMPDF*zMPDF) : sqrt(xSF*xSF + ySF*ySF + zSF*zSF)", "sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) < 5000", "colz");

  TH1F *htemp1 = (TH1F*)gPad->GetPrimitive("htemp");
  //htemp1->GetXaxis()->SetRangeUser(0,16000);
  //htemp1->GetYaxis()->SetRangeUser(0,16000);
  htemp1->GetYaxis()->SetTitle("MultiPDF Fitted Radius, mm");
  htemp1->GetXaxis()->SetTitle("ScintFitter Fitted Radius, mm");
  //  htemp1->GetXaxis()->SetTitle("True Radius, mm");
  htemp1->GetYaxis()->SetTitleOffset(1.5);
  //htemp1->GetXaxis()->SetRangeUser(-800,800);
  htemp1->SetTitle("ScintFitter Reconstructed vs True Radius");

}

