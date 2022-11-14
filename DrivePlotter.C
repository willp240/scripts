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


void DrivePlotter(){
  
  //  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/Aug16_MPDF_Dir_MPDFPosOnly_6MeV_FV_diPo_Tree.root");
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree");

  TCanvas* c1 = new TCanvas("c1", "c1", 800,600);
  c1->SetLeftMargin(0.13);

  tree1->Draw("( ((xFit-xTrue)*xdirTrue) + ((yFit-yTrue)*ydirTrue) + ((zFit-zTrue)*zdirTrue))/(sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue))");
  TH1F *htemp1 = (TH1F*)gPad->GetPrimitive("htemp");
  htemp1->GetXaxis()->SetTitle("Drive, mm");
  htemp1->GetYaxis()->SetTitle("Events");
  htemp1->GetYaxis()->SetTitleOffset(1.4);
  htemp1->GetXaxis()->SetRangeUser(-800,800);
  htemp1->SetTitle("Position Fit with 2D PDF");
  //  htemp1->SetTitle("Scattering: Joint Fit");
  htemp1->SetLineColor(kBlue+2);
  htemp1->SetLineWidth(2);
  //  TPaveText *title = (TPaveText*)gPad->GetPrimitive("title");
  // title->SetBorderSize(2);
  //gPad->Update();
}

void PlotCorr(){

  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetPalette(55);

  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Jun7_6MeV_fullvolume_6MeV_noScattering_jointFit_Tree.root");
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree1");

  TCanvas* c1 = new TCanvas("c1", "c1", 1000,600);

  tree1->Draw("(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) ): ( ((xFit-xTrue)*xdirTrue) + ((yFit-yTrue)*ydirTrue) + ((zFit-zTrue)*zdirTrue))/(sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue))","","colz");
  TH1F *htemp1 = (TH1F*)gPad->GetPrimitive("htemp");
  htemp1->GetXaxis()->SetTitle("Drive, mm");
  htemp1->GetYaxis()->SetTitle("cos #alpha");
  htemp1->GetYaxis()->SetTitleOffset(1.3);
  htemp1->GetXaxis()->SetRangeUser(-800,800);
  htemp1->GetYaxis()->SetRangeUser(-1,1);
  //htemp1->SetTitle("6 MeV e- at Center (Random Directions): True Position in PDF, Fit Position Seed");
  htemp1->SetTitle("No Scattering: Joint Fit");
  htemp1->SetLineColor(kGreen);
  htemp1->SetLineWidth(2);
  //  TPaveText *title = (TPaveText*)gPad->GetPrimitive("title");
  // title->SetBorderSize(2);
  //gPad->Update();
}


void CompareDrives(){

  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/Jul21_recoordMPDF_2p2gl_perf_1to10MeV_Tree.root");
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree");
  TFile *file2 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/Aug24_MPDFRecoord_4m_perf1to10MeV_Tree.root");
  TTree* tree2 = (TTree*)file2->Get("eveTree")->Clone("tree");
  TFile *file3 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/Aug10_MPDFRecoord_3m_perf1to10MeV_Tree.root");
  TTree* tree3 = (TTree*)file3->Get("eveTree")->Clone("tree");
  TFile *file4 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/Aug15_MPDFRecoordcentre_perf_1to10MeV_Tree.root");
  TTree* tree4 = (TTree*)file4->Get("eveTree")->Clone("tree");
  //  TFile *file5 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/Aug19_2p2_dirpos_3m_1nsbins_m100to300_MPDF_Tree.root");
  //TTree* tree5 = (TTree*)file5->Get("eveTree")->Clone("tree");

  TH1D *htemp1 = new TH1D("htemp1", "Time Window -5 - 10 ns; Drive, mm; Normalised Events", 80, -800, 800);
  TH1D *htemp2 = new TH1D("htemp2", "Time Window -5 - 15 ns; Drive, mm; Normalised Events", 80, -800, 800);
  TH1D *htemp3 = new TH1D("htemp3", "Time Window -50 - 100 ns; Drive, mm; Normalised Events", 80, -800, 800);
  TH1D *htemp4 = new TH1D("htemp4", "Time Window -100 - 300 ns; Drive, mm; Normalised Events", 80, -800, 800);
  //TH1D *htemp5 = new TH1D("htemp5", "Time Window -5 - 15 ns; Drive, mm; Normalised Events", 40, -1000, 1000);

  TCanvas* c1 = new TCanvas("c1", "c1", 800,600);
  c1->SetLeftMargin(0.13);

  tree1->Draw("( ((xFit-xTrue)*xdirTrue) + ((yFit-yTrue)*ydirTrue) + ((zFit-zTrue)*zdirTrue))/(sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue)) >> htemp1");
  htemp1->GetYaxis()->SetTitleOffset(1.2);
  htemp1->SetLineColor(kRed+2);
  htemp1->SetLineWidth(2);
  htemp1->Scale(1/htemp1->Integral());
  htemp1->GetYaxis()->SetRangeUser(0,0.12);
  //htemp1->GetXaxis()->SetRangeUser(-600,600);

  tree2->Draw("( ((xFit-xTrue)*xdirTrue) + ((yFit-yTrue)*ydirTrue) + ((zFit-zTrue)*zdirTrue))/(sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue)) >> htemp2");
  htemp2->GetYaxis()->SetTitleOffset(1.4);
  htemp2->SetLineColor(kBlue+2);
  htemp2->SetLineWidth(2);
  htemp2->Scale(1/htemp2->Integral());


  tree3->Draw("( ((xFit-xTrue)*xdirTrue) + ((yFit-yTrue)*ydirTrue) + ((zFit-zTrue)*zdirTrue))/(sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue)) >> htemp3");
  htemp3->GetYaxis()->SetTitleOffset(1.4);
  htemp3->SetLineColor(kGreen+2);
  htemp3->SetLineWidth(2);
  htemp3->Scale(1/htemp3->Integral());


  tree4->Draw("( ((xFit-xTrue)*xdirTrue) + ((yFit-yTrue)*ydirTrue) + ((zFit-zTrue)*zdirTrue))/(sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue)) >> htemp4");
  htemp4->GetYaxis()->SetTitleOffset(1.4);
  htemp4->SetLineColor(kYellow+2);
  htemp4->SetLineWidth(2);
  htemp4->Scale(1/htemp4->Integral());


  // tree5->Draw("( ((xFit-xTrue)*xdirTrue) + ((yFit-yTrue)*ydirTrue) + ((zFit-zTrue)*zdirTrue))/(sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue)) >> htemp5");
  //htemp5->GetYaxis()->SetTitleOffset(1.4);
  //htemp5->SetLineColor(kBlack);
  //htemp5->SetLineWidth(2);
  //htemp5->Scale(1/htemp5->Integral());

  
  TLegend *leg = new TLegend(0.13,0.7,0.52,0.9);
  leg->AddEntry(htemp1, "R < 5.5 m:  Mean 102.9 mm, RMS 76.35 mm", "l");
  leg->AddEntry(htemp2, "R < 4.0 m:  Mean 103.5 mm, RMS 70.75 mm", "l");
  leg->AddEntry(htemp3, "R < 3.0 m:  Mean 103.4 mm, RMS 71.27 mm", "l");
  leg->AddEntry(htemp4, "R == 0.0 m: Mean 104.5 mm, RMS 70.87 mm", "l");
  //leg->AddEntry(htemp5, "Pos Only:   Time Window -100 - 300 ns", "l");
  leg->SetLineWidth(2);

  htemp1->Draw();
  htemp2->Draw("same");
  htemp3->Draw("same");
  htemp4->Draw("same");
  //htemp5->Draw("same");
  leg->Draw("same");

}
