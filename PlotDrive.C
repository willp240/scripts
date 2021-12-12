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


void PlotDrive(){

  gStyle->SetOptStat(1);

  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Oct18_dirPosTest_directional_SF_Tree.root");
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree1");

  TFile *file2 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Oct21_dirPos_reallympdf_10pow8_directional_diPo_Tree.root");
  TTree* tree2 = (TTree*)file2->Get("eveTree")->Clone("tree2");
 

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,500);
  c1->Divide(2,1);
  
  c1->cd(1);
  tree1->Draw("xFit-xTrue");
  TH1F *htemp1 = (TH1F*)gPad->GetPrimitive("htemp");
  htemp1->GetXaxis()->SetTitle("Fitted - True X, mm");
  htemp1->GetYaxis()->SetTitle("Events");
  htemp1->GetYaxis()->SetTitleOffset(1.3);
  htemp1->GetXaxis()->SetRangeUser(-1000,1000);
  htemp1->SetTitle("6 MeV: Original ScintFitter");
  TF1 *f1 = new TF1("f1","gaus");
  f1->SetLineColor(kRed);
  htemp1->Fit(f1);
  gPad->Update();
    
  c1->cd(2);
  tree2->Draw("xFit-xTrue");
  TH1F *htemp2 = (TH1F*)gPad->GetPrimitive("htemp");
  htemp2->GetXaxis()->SetTitle("Fitted - True X, mm");
  htemp2->GetYaxis()->SetTitle("Events");
  htemp2->GetYaxis()->SetTitleOffset(1.3);
  htemp2->GetXaxis()->SetRangeUser(-1000,1000);
  htemp2->SetTitle("6 MeV: PosDir Fitter");
  TF1 *f2 = new TF1("f2","gaus");
  f2->SetLineColor(kRed);
  htemp2->Fit(f2);
  gPad->Update();

}

void Plot3Drive(){

  gStyle->SetOptStat(1);

  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Oct22_multiPDFFit_mpdf_10pow8_nondirectional_SF_Tree.root");
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree1");

  TFile *file2 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Oct22_quad_mpdf_10pow8_nondirectional_diPo_Tree.root");
  TTree* tree2 = (TTree*)file2->Get("eveTree")->Clone("tree2");

  TFile *file3 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Oct22_multiPDFFit_mpdf_10pow8_nondirectional_diPo_Tree.root");
  TTree* tree3 = (TTree*)file3->Get("eveTree")->Clone("tree3");

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,300);
  c1->Divide(3,1);

  c1->cd(1);
  tree1->Draw("zFit-zTrue");
  TH1F *htemp1 = (TH1F*)gPad->GetPrimitive("htemp");
  htemp1->GetXaxis()->SetTitle("Fitted - True Z, mm");
  htemp1->GetYaxis()->SetTitle("Events");
  htemp1->GetYaxis()->SetTitleOffset(1.3);
  htemp1->GetXaxis()->SetRangeUser(-1000,1000);
  htemp1->SetTitle("6 MeV: Original ScintFitter");
  TF1 *f1 = new TF1("f1","gaus");
  f1->SetLineColor(kRed);
  htemp1->Fit(f1);
  gPad->Update();

  c1->cd(2);
  tree2->Draw("zFit-zTrue");
  TH1F *htemp2 = (TH1F*)gPad->GetPrimitive("htemp");
  htemp2->GetXaxis()->SetTitle("Fitted - True Z, mm");
  htemp2->GetYaxis()->SetTitle("Events");
  htemp2->GetYaxis()->SetTitleOffset(1.3);
  htemp2->GetXaxis()->SetRangeUser(-1000,1000);
  htemp2->SetTitle("6 MeV: PosDir Fitter with Quad Seed");
  TF1 *f2 = new TF1("f2","gaus");
  f2->SetLineColor(kRed);
  htemp2->Fit(f2);
  gPad->Update();

  c1->cd(3);
  tree3->Draw("zFit-zTrue");
  TH1F *htemp3 = (TH1F*)gPad->GetPrimitive("htemp");
  htemp3->GetXaxis()->SetTitle("Fitted - True Z, mm");
  htemp3->GetYaxis()->SetTitle("Events");
  htemp3->GetYaxis()->SetTitleOffset(1.3);
  htemp3->GetXaxis()->SetRangeUser(-1000,1000);
  htemp3->SetTitle("6 MeV: PosDir Fitter with MPDF Seed");
  TF1 *f3 = new TF1("f3","gaus");
  f3->SetLineColor(kRed);
  htemp3->Fit(f3);
  gPad->Update();

}


void Plot3Theta(){

  gStyle->SetOptStat(0);

  TFile *file2 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Oct22_quad_mpdf_10pow8_nondirectional_diPo_Tree.root");
  TTree* tree2 = (TTree*)file2->Get("eveTree")->Clone("tree2");

  TFile *file3 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Oct22_multiPDFFit_mpdf_10pow8_nondirectional_diPo_Tree.root");
  TTree* tree3 = (TTree*)file3->Get("eveTree")->Clone("tree3");

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,500);
  c1->Divide(2,1);

  c1->cd(1);
  tree2->Draw("cos(tFit-tTrue)");
  TH1F *htemp2 = (TH1F*)gPad->GetPrimitive("htemp");
  htemp2->GetXaxis()->SetTitle("cos(#theta_{Fitted}-#theta_{True})");
  htemp2->GetYaxis()->SetTitle("Events");
  htemp2->GetYaxis()->SetTitleOffset(1.3);
  htemp2->GetXaxis()->SetRangeUser(0.5,1);
  htemp2->SetTitle("6 MeV: PosDir Fitter with Quad Seed");
  gPad->Update();

  c1->cd(2);
  tree3->Draw("cos(tFit-tTrue)");
  TH1F *htemp3 = (TH1F*)gPad->GetPrimitive("htemp");
  htemp3->GetXaxis()->SetTitle("cos(#theta_{Fitted}-#theta_{True})");
  htemp3->GetYaxis()->SetTitle("Events");
  htemp3->GetYaxis()->SetTitleOffset(1.3);
  htemp3->GetXaxis()->SetRangeUser(0.5,1);
  htemp3->SetTitle("6 MeV: PosDir Fitter with MPDF Seed");
  gPad->Update();

}
