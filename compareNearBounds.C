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


void compareNearBounds(){
  
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  //TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Dec24_coord_1mshells_fulllrecoord_Tree.root");
  TFile *file1 = TFile::Open("/data/snoplus3/parkerw/rat_perf/Nov23_sevRadialScale/Dec24_coord_1mshells_fulllrecoord_fitshell_Tree.root");
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree");

  TFile *file2 = TFile::Open("/data/snoplus3/parkerw/rat_perf/Nov23_sevRadialScale/Dec24_coord_1mshells_fulllrecoord_fitshell_Tree.root");
  TTree* tree2 = (TTree*)file2->Get("eveTree")->Clone("tree2");

  TH1F *htemp = new TH1F("htemp", "Fit - Truth Distance", 100, -500, 500);
  TH1F *htemp2 = new TH1F("htemp2", "htemp2", 100, -500, 500);

  TCanvas* c1 = new TCanvas("c1", "c1", 800,600);
  //c1->SetLeftMargin(0.13);
  c1->SetGrid();

  float xTrue, yTrue, zTrue;
  tree1->SetBranchAddress("xTrue", &xTrue);
  tree1->SetBranchAddress("yTrue", &yTrue);
  tree1->SetBranchAddress("zTrue", &zTrue);

  for(int i = 0; i<10; i++){

    tree1->GetEntry(i);
    std::cout << "x: " << xTrue << ", y: " << yTrue << ", z: " << zTrue << ", r: " << sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) << ", dist: " << int( sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) ) % 1000 << std::endl;
  }

  //tree1->Draw("  ( (xFit-xTrue)*xFit + (yFit-yTrue)*yFit + (zFit-zTrue)*zFit ) / sqrt(xFit*xFit + yFit*yFit + zFit*zFit) >> htemp", "seedShell != trueShell && sqrt(xFit*xFit + yFit*yFit + zFit*zFit) < 5000", "");
  //tree1->Draw("sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue) ) >> htemp", "seedShell != trueShell", "");
  //  tree1->Draw("( ( (xFit-xTrue)*xFit + (yFit-yTrue)*yFit + (zFit-zTrue)*zFit ) / sqrt(xFit*xFit + yFit*yFit + zFit*zFit) ) >> htemp", "( int(sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue)) % 1000 > 50 && int(sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue)) % 1000 < 950 ) ", "");
  
  tree1->Draw("sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue) ) >> htemp", "( int(sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue)) % 1000 < 50 || int(sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue)) % 1000 > 950 )", "");
  htemp->GetXaxis()->SetTitle("Fit - Truth Distance, mm");
  htemp->GetYaxis()->SetTitle("Events");
  htemp->GetXaxis()->SetRangeUser(0,500);
  htemp->SetLineColor(kBlue+2);
  htemp->SetLineWidth(2);
  htemp->GetYaxis()->SetTitleOffset(1.3);
  htemp->Scale(1/htemp->Integral());
  htemp->GetYaxis()->SetRangeUser(0,0.07);

  //tree2->Draw("sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue) ) >> htemp2","seedShell == trueShell","");
  //  tree2->Draw("( ( (xFit-xTrue)*xFit + (yFit-yTrue)*yFit + (zFit-zTrue)*zFit ) / sqrt(xFit*xFit + yFit*yFit + zFit*zFit)) >> htemp2", "( int(sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue)) % 1000 < 50 || int(sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue)) > 950 )", "");

  tree2->Draw("sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue) ) >> htemp2 ", "( int(sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue)) % 1000 > 50 && int(sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue)) % 1000 < 950 ) ", "");
  htemp2->GetXaxis()->SetTitle("Fit - Truth Distance, mm");
  htemp2->GetYaxis()->SetTitle("Events");
  htemp2->GetXaxis()->SetRangeUser(0,500);
  htemp2->SetLineColor(kBlack);
  htemp2->SetLineWidth(2);
  htemp2->Scale(1/htemp2->Integral());
  htemp->Draw();
  htemp2->Draw("same");

  std::cout << htemp->GetMean() << " " << htemp->Integral() << std::endl;
  std::cout << htemp2->GetMean() << " " << htemp2->Integral() << std::endl;
  
  //TLegend* t1 = new TLegend( 0.15, 0.67, 0.45, 0.87 );
  TLegend* t1 = new TLegend( 0.55, 0.67, 0.85, 0.87 );
  t1->AddEntry( htemp, "< 50 mm from Boundary", "l" );
  t1->AddEntry( htemp2, "> 50 mm from Boundary", "l" );
  t1->SetLineWidth(2);
  t1->Draw("same");

  c1->SaveAs("Dec24_compNearBoundsFitDist.pdf");
}
