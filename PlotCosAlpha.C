#include <TH1D.h>
#include <TH2F.h>
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


void PlotCosAlpha(){

  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Jun7_6MeV_fullvolume_6MeV_wScattering_jointFit_Tree.root");
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree1");

  TCanvas* c1 = new TCanvas("c1", "c1", 1000,600);

  tree1->Draw("(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) )");
  TH1F *htemp1 = (TH1F*)gPad->GetPrimitive("htemp");
  htemp1->GetXaxis()->SetTitle("cos (#alpha)");
  htemp1->GetYaxis()->SetTitle("Events");
  htemp1->GetYaxis()->SetTitleOffset(1.3);
  htemp1->GetXaxis()->SetRangeUser(-1,1);
  //htemp1->SetTitle("6 MeV e- at Center (Random Directions) 2g/l: True Position in PDF, Fit Position Seed");
  htemp1->SetTitle("Scattering: Joint Fit");
  htemp1->SetLineColor(kGreen);
  htemp1->SetLineWidth(2);
  //  TPaveText *title = (TPaveText*)gPad->GetPrimitive("title");
  // title->SetBorderSize(2);
  //gPad->Update();
}

void RCosAlpha(){

  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/May13_scaled1p0_centred_fit_Tree.root");
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree1");

  TCanvas* c1 = new TCanvas("c1", "c1", 1000,600);

  tree1->Draw("sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) : (xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) )", "", "colz");
  TH2F *htemp1 = (TH2F*)gPad->GetPrimitive("htemp");
  htemp1->GetXaxis()->SetTitle("cos (#alpha)");
  htemp1->GetYaxis()->SetTitle("True Radius, mm");
  htemp1->GetYaxis()->SetTitleOffset(1.3);
  htemp1->GetXaxis()->SetRangeUser(-1,1);
  htemp1->GetXaxis()->SetRangeUser(0,6000);
  //htemp1->SetTitle("6 MeV e- at Center (Random Directions) 2g/l: True Position in PDF, Fit Position Seed");
  htemp1->SetTitle("6 MeV e- at Random Positions (Random Directions) Scaled 2g/l: Direction Only Fit (Fit Position in PDF)");
  htemp1->SetLineColor(kGreen);
  htemp1->SetLineWidth(2);
  //  TPaveText *title = (TPaveText*)gPad->GetPrimitive("title");
  // title->SetBorderSize(2);
  //gPad->Update();
}


void CompareCosAlpha(){

  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/Aug18_2p2Direction2D_perf_3m_Tree.root");
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree1");
  TFile *file2 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/Aug24_2p2_DirOnly_3m_solidang_m5to15_Tree.root");
  TTree* tree2 = (TTree*)file2->Get("eveTree")->Clone("tree2");
  /*  TFile *file3 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/Aug21_2p2_DirOnly_3m_m5to30_Tree.root");
  TTree* tree3 = (TTree*)file3->Get("eveTree")->Clone("tree3");
  TFile *file4 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/Aug21_2p2_DirOnly_3m_m15to15_Tree.root");
  TTree* tree4 = (TTree*)file4->Get("eveTree")->Clone("tree4");
  TFile *file5 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/Aug21_2p2_DirOnly_3m_m30to15_Tree.root");
  TTree* tree5 = (TTree*)file5->Get("eveTree")->Clone("tree5");
  TFile *file6 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/Aug21_2p2_DirOnly_3m_m50to100_Tree.root");
  TTree* tree6 = (TTree*)file6->Get("eveTree")->Clone("tree6");
  TFile *file7 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/Aug21_2p2_DirOnly_3m_m100to300_Tree.root");
  TTree* tree7 = (TTree*)file7->Get("eveTree")->Clone("tree7");*/

  TCanvas* c1 = new TCanvas("c1", "c1", 1000,600);

  TH1D *h1 = new TH1D("h1", "Time Window -5 - 10 ns; cos (#alpha); Events", 40, -1, 1);
  TH1D *h2 = new TH1D("h2", "Time Window -5 - 15 ns; cos (#alpha); Events", 40, -1, 1);
  /*TH1D *h3 = new TH1D("h3", "Time Window -50 - 100 ns; cos (#alpha); Events", 40, -1, 1);
  TH1D *h4 = new TH1D("h4", "Time Window -100 - 300 ns; cos (#alpha); Events", 40, -1, 1);
  TH1D *h5 = new TH1D("h5", "Time Window -30 - 15 ns; cos (#alpha); Events", 40, -1, 1);
  TH1D *h6 = new TH1D("h6", "Time Window -50 - 100 ns; cos (#alpha); Events", 40, -1, 1);
  TH1D *h7 = new TH1D("h7", "Time Window -100 - 300 ns; cos (#alpha); Events", 40, -1, 1);*/

  tree1->Draw("(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) ) >> h1");
  h1->GetXaxis()->SetTitle("cos (#alpha)");
  h1->GetYaxis()->SetTitle("Normalised Events");
  h1->GetYaxis()->SetTitleOffset(1.1);
  h1->GetXaxis()->SetRangeUser(-1,1);
  h1->SetLineColor(kRed+2);
  h1->SetLineWidth(2);
  h1->Scale(1/h1->Integral());

  tree2->Draw("(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) ) >> h2");
  h2->GetXaxis()->SetTitle("cos (#alpha)");
  h2->GetYaxis()->SetTitle("Normalised Events");
  h2->GetYaxis()->SetTitleOffset(1.3);
  h2->GetXaxis()->SetRangeUser(-1,1);
  h2->SetLineColor(kBlue+2);
  h2->SetLineWidth(2);
  h2->SetLineStyle(2);
  h2->Scale(1/h2->Integral());
  /*
  tree3->Draw("(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) ) >> h3");
  h3->GetXaxis()->SetTitle("cos (#alpha)");
  h3->GetYaxis()->SetTitle("Normalised Events");
  h3->GetYaxis()->SetTitleOffset(1.3);
  h3->GetXaxis()->SetRangeUser(-1,1);
  h3->SetLineColor(kGreen+2);
  h3->SetLineWidth(2);
  //  h3->SetLineStyle(7);
  h3->Scale(1/h3->Integral());

  tree4->Draw("(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) ) >> h4");
  h4->GetXaxis()->SetTitle("cos (#alpha)");
  h4->GetYaxis()->SetTitle("Normalised Events");
  h4->GetYaxis()->SetTitleOffset(1.3);
  h4->GetXaxis()->SetRangeUser(-1,1);
  h4->SetLineColor(kYellow+2);
  //h4->SetLineStyle(2);
  h4->SetLineWidth(2);
  h4->Scale(1/h4->Integral());

  tree5->Draw("(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) ) >> h5");
  h5->GetXaxis()->SetTitle("cos (#alpha)");
  h5->GetYaxis()->SetTitle("Normalised Events");
  h5->GetYaxis()->SetTitleOffset(1.3);
  h5->GetXaxis()->SetRangeUser(-1,1);
  h5->GetYaxis()->SetRangeUser(0,0.25);
  h5->SetLineColor(kBlack);
  h5->SetLineWidth(2);
  h5->Scale(1/h5->Integral());
  
  tree6->Draw("(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) ) >> h6");
  h6->GetXaxis()->SetTitle("cos (#alpha)");
  h6->GetYaxis()->SetTitle("Normalised Events");
  h6->GetYaxis()->SetTitleOffset(1.3);
  h6->GetXaxis()->SetRangeUser(-1,1);
  h6->SetLineColor(kPink+2);
  h6->SetLineWidth(2);
  h6->Scale(1/h6->Integral());

  tree7->Draw("(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) ) >> h7");
  h7->GetXaxis()->SetTitle("cos (#alpha)");
  h7->GetYaxis()->SetTitle("Normalised Events");
  h7->GetYaxis()->SetTitleOffset(1.3);
  h7->GetXaxis()->SetRangeUser(-1,1);
  h7->SetLineColor(kCyan+2);
  h7->SetLineWidth(2);
  h7->Scale(1/h7->Integral());
  */
  TLegend *leg = new TLegend(0.2,0.5,0.55,0.79);
  leg->AddEntry(h1, "No Solid Angle Correction  (39.0% > 0.8; 31.1% > 0.9)", "l");
  leg->AddEntry(h2, "Solid Angle Correction     (39.0% > 0.8; 31.1% > 0.9)", "l");
  /*leg->AddEntry(h3, "Time Window -5 - 30 ns         (36.4% > 0.8; 28.8% > 0.9)", "l");
  leg->AddEntry(h4, "Time Window -15 - 15 ns       (39.1% > 0.8; 31.2% > 0.9)", "l");
  leg->AddEntry(h5, "Time Window -30 - 15 ns       (39.1% > 0.8; 31.2% > 0.9)", "l");
  leg->AddEntry(h6, "Time Window -50 - 100 ns     (33.3% > 0.8; 26.2% > 0.9)", "l");
  leg->AddEntry(h7, "Time Window -100 - 300 ns   (32.8% > 0.8; 25.9% > 0.9)", "l");*/
  leg->SetLineWidth(2);

  h1->Draw();
  h2->Draw("same");
  /*h3->Draw("same");
  h4->Draw("same");
  h5->Draw("same");
  h6->Draw("same");
  h7->Draw("same");*/
  leg->Draw("same");
}
