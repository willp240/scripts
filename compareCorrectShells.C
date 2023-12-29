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


void compareCorrectShells(){
  
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  //TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Dec24_coord_1mshells_fulllrecoord_Tree.root");
  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Dec23_coord_oldrat_Tree.root");
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree");

  TFile *file2 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Dec24_coord_1mshells_fulllrecoord_Tree.root");
  TTree* tree2 = (TTree*)file2->Get("eveTree")->Clone("tree2");

  TH1F *htemp = new TH1F("htemp", "Drive", 100, -500, 500);
  TH1F *htemp2 = new TH1F("htemp2", "htemp2", 100, -500, 500);

  TCanvas* c1 = new TCanvas("c1", "c1", 800,600);
  //c1->SetLeftMargin(0.13);
  c1->SetGrid();

  //tree1->Draw("  ( (xFit-xTrue)*xFit + (yFit-yTrue)*yFit + (zFit-zTrue)*zFit ) / sqrt(xFit*xFit + yFit*yFit + zFit*zFit) >> htemp", "seedShell != trueShell && sqrt(xFit*xFit + yFit*yFit + zFit*zFit) < 5000", "");
  //tree1->Draw("sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue) ) >> htemp", "seedShell != trueShell", "");
  tree1->Draw("( ((xFit-xTrue)*xdirTrue) + ((yFit-yTrue)*ydirTrue) + ((zFit-zTrue)*zdirTrue))/(sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue)) >> htemp", "sqrt(xFit*xFit + yFit*yFit + zFit*zFit) < 5000 && sqrt(xFit*xFit + yFit*yFit + zFit*zFit) > 4000", "");
  htemp->GetXaxis()->SetTitle("Drive, mm");
  htemp->GetYaxis()->SetTitle("Events");
  htemp->GetXaxis()->SetRangeUser(-500,500);
  htemp->SetLineColor(kBlue+2);
  htemp->SetLineWidth(2);
  htemp->GetYaxis()->SetTitleOffset(1.2);
  htemp->Scale(1/htemp->Integral());
  htemp->GetYaxis()->SetRangeUser(0,0.06);

  //tree2->Draw("sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue) ) >> htemp2","seedShell == trueShell","");
  tree2->Draw("( ((xFit-xTrue)*xdirTrue) + ((yFit-yTrue)*ydirTrue) + ((zFit-zTrue)*zdirTrue))/(sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue)) >> htemp2", "sqrt(xFit*xFit + yFit*yFit + zFit*zFit) < 5000 && sqrt(xFit*xFit + yFit*yFit + zFit*zFit) > 4000", "");
  htemp2->GetXaxis()->SetTitle("Drive, mm");
  htemp2->GetYaxis()->SetTitle("Events");
  htemp2->GetXaxis()->SetRangeUser(-500,500);
  htemp2->SetLineColor(kBlack);
  htemp2->SetLineWidth(2);
  htemp2->Scale(1/htemp2->Integral());
  htemp->Draw();
  htemp2->Draw("same");

  std::cout << htemp->GetMean() << " " << htemp2->GetMean() << std::endl;
  
  TLegend* t1 = new TLegend( 0.15, 0.6, 0.45, 0.85 );
  t1->AddEntry( htemp, "Current RAT", "l" );
  t1->AddEntry( htemp2, "Radial SEV+MPDF", "l" );
  t1->SetLineWidth(2);
  t1->Draw("same");

  c1->SaveAs("Dec24_shellcompr4to5m_drive.pdf");
}
