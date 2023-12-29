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


void PlotBandComp(){
  
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Nov15_5to6m_SEVMPDF_Tree.root");
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree");

  TFile *file2 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Nov17_5to5p5m_perf5to6_SEVMPDF_Tree.root");
  TTree* tree2 = (TTree*)file2->Get("eveTree")->Clone("tree2");

  TFile *file3 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Sep25_testdag_prevtag_6msim_Tree.root");
  TTree* tree3 = (TTree*)file3->Get("eveTree")->Clone("tree3");

  ////////////////////////////////////////////////////////////////////////////////

  TH1F *hz = new TH1F("hz", "Z Bias", 100, -3000, 3000);
  TH1F *hz2 = new TH1F("hz2", "Z Bias", 100, -3000, 3000);
  TH1F *hz3 = new TH1F("hz3", "Z Bias", 100, -3000, 3000);

  TCanvas* cz = new TCanvas("cz", "cz", 800,600);
  //c1->SetLeftMargin(0.13);
  cz->SetGrid();

  tree1->Draw(" ( (xFit-xTrue)*xFit + (yFit-yTrue)*yFit + (zFit-zTrue)*zFit ) / sqrt(xFit*xFit + yFit*yFit + zFit*zFit)  >> hz", "sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) < 6000 && sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) > 5500");
  hz->GetXaxis()->SetTitle("Fit - Truth Z, mm");
  hz->GetYaxis()->SetTitle("Normalised Events");
  hz->Scale(1/hz->Integral());
  hz->GetXaxis()->SetRangeUser(-3000,3000);
  hz->GetYaxis()->SetRangeUser(0,0.28);
  hz->SetLineColor(kBlack);
  hz->SetLineWidth(2);
  hz->GetYaxis()->SetTitleOffset(1.4);
  TF1  *fz1 = new TF1("fz1","gaus");
  fz1->SetLineColor(kBlack);
  hz->Fit("fz1","0");

  tree2->Draw(" ( (xFit-xTrue)*xFit + (yFit-yTrue)*yFit + (zFit-zTrue)*zFit ) / sqrt(xFit*xFit + yFit*yFit + zFit*zFit) >> hz2","sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) < 6000 && sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) > 5500","same");
  hz2->GetXaxis()->SetTitle("Fit - Truth Z, mm");
  hz2->GetYaxis()->SetTitle("Events");
  hz2->Scale(1/hz2->Integral());
  // hz2->GetXaxis()->SetRangeUser(-500,500);
  hz2->SetLineColor(kRed+2);
  hz2->SetLineWidth(2);
  TF1  *fz2 = new TF1("fz2","gaus");
  fz2->SetLineColor(kRed+2);
  hz2->Fit("fz2","0");

  tree3->Draw(" ( (xFit-xTrue)*xFit + (yFit-yTrue)*yFit + (zFit-zTrue)*zFit ) / sqrt(xFit*xFit + yFit*yFit + zFit*zFit)  >> hz3", "sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) < 6000 && sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) > 5500", "same");
  hz3->GetXaxis()->SetTitle("Fit - Truth Z, mm");
  hz3->GetYaxis()->SetTitle("Normalised Events");
  hz3->Scale(1/hz3->Integral());
  hz3->GetXaxis()->SetRangeUser(-3000,3000);
  hz3->GetYaxis()->SetRangeUser(0,0.28);
  hz3->SetLineColor(kMagenta+2);
  hz3->SetLineWidth(2);
  TF1  *fz3 = new TF1("fz3","gaus");
  fz3->SetLineColor(kMagenta+2);
  hz3->Fit("fz3","0");

  hz3->Draw("");
  hz2->Draw("same");
  hz->Draw("same");

  TLegend* tz = new TLegend( 0.14, 0.6, 0.39, 0.8 );
  tz->AddEntry( hz, Form("PDF 5-6 m, #mu %.2f mm", fz1->GetParameter(1)), "l" );
  tz->AddEntry( hz2, Form("PDF 5-5.5 m, #mu %.2f mm", fz2->GetParameter(1)), "l" );
  tz->AddEntry( hz3, Form("PDF 0-4 m, #mu %.2f mm", fz3->GetParameter(1)), "l" );
  tz->SetLineWidth(2);
  tz->Draw("same");

  cz->SaveAs("Nov15_5p5to6m_rbias.pdf");


}
