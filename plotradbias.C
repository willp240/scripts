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


void plotradbias(){
  
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Nov7widerange_0to500mm_155_Tree.root");
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree");

  TFile *file2 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Nov7widerange_0to500mm_185_Tree.root");
  TTree* tree2 = (TTree*)file2->Get("eveTree")->Clone("tree2");

  TH1F *htemp = new TH1F("htemp", "Time Bias", 100, -10, 10);
  TH1F *htemp2 = new TH1F("htemp2", "htemp2", 100, -10, 10);

  TCanvas* c1 = new TCanvas("c1", "c1", 800,600);
  //c1->SetLeftMargin(0.13);
  c1->SetGrid();

  tree1->Draw(" tiFit-tiTrue  >> htemp");
  htemp->GetXaxis()->SetTitle("Time Bias, ns");
  htemp->GetYaxis()->SetTitle("Events");
  htemp->GetXaxis()->SetRangeUser(-500,500);
  htemp->SetLineColor(kBlue+2);
  htemp->SetLineWidth(2);
  htemp->GetYaxis()->SetTitleOffset(1.2);

  tree2->Draw(" tiFit-tiTrue >> htemp2","","same");
  htemp2->GetXaxis()->SetTitle("Time Bias, ns");
  htemp2->GetYaxis()->SetTitle("Events");
  htemp2->GetXaxis()->SetRangeUser(-500,500);
  htemp2->SetLineColor(kBlack);
  htemp2->SetLineWidth(2);
  htemp->Draw();
  htemp2->Draw("same");

  std::cout << htemp->GetMean() << " " << htemp2->GetMean() << std::endl;
  
  TLegend* t1 = new TLegend( 0.6, 0.6, 0.8, 0.8 );
  t1->AddEntry( htemp, "155 mm/ns", "l" );
  t1->AddEntry( htemp2, "185 mm/ns", "l" );
  t1->SetLineWidth(2);
  t1->Draw("same");

  c1->SaveAs("Nov7widerange_0to500mm_185_155_tbias.pdf");
}
