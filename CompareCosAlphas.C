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
#include <TLegend.h>
#include <string>


void CompareCosAlphas(){

  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Jun3_6MeV_fullvolume_6MeV_wScattering_dirOnly_Tree.root");
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree1");
  //TFile *file2 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Feb16_justdir_trueposPDF_truePos_Tree.root");
  //TTree* tree2 = (TTree*)file2->Get("eveTree")->Clone("tree2");
  TFile *file3 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Jun3_6MeV_fullvolume_6MeV_noScattering_dirOnly_Tree.root");
  TTree* tree2 = (TTree*)file3->Get("eveTree")->Clone("tree2");
  //TFile *file4 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Jun14_6MeV_centred_6MeV_TrueSeed_TruePDF_NoScatter_Tree.root");
  //TTree* tree4 = (TTree*)file4->Get("eveTree")->Clone("tree4");
  /*  TFile *file5 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Jun14_6MeV_fv_grid10_Tree.root");
  TTree* tree5 = (TTree*)file5->Get("eveTree")->Clone("tree5");
  TFile *file6 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Jun14_6MeV_fv_simAnn_Tree.root");
  TTree* tree6 = (TTree*)file6->Get("eveTree")->Clone("tree6");
  TFile *file7 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Jun14_6MeV_fv_powelll_Tree.root");
  TTree* tree7 = (TTree*)file7->Get("eveTree")->Clone("tree7");
  TFile *file8 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Jun14_6MeV_fv_minuit_Tree.root");
  TTree* tree8 = (TTree*)file8->Get("eveTree")->Clone("tree8");*/
  
  TH1D *h1 = new TH1D("h1", "6 MeV e- throughout AV, Direction Only Fit, 2g/l; cos (#alpha); Events", 40, -1, 1);
  TH1D *h2 = new TH1D("h2", "6 MeV e- throughout AV, Direction Only Fit, 2g/l; cos (#alpha); Events", 40, -1, 1);
  //TH1D *h3 = new TH1D("h3", "6 MeV e- throughout AV, Direction Only Fit, 2g/l; cos (#alpha); Events", 40, -1, 1);
  //TH1D *h4 = new TH1D("h4", "6 MeV e- throughout AV, Direction Only Fit, 2g/l; cos (#alpha); Events", 40, -1, 1);
  /*  TH1D *h5 = new TH1D("h5", "6 MeV e- throughout AV, Direction Only Fit, 2g/l; cos (#alpha); Events", 40, -1, 1);
  TH1D *h6 = new TH1D("h6", "6 MeV e- throughout AV, Direction Only Fit, 2g/l; cos (#alpha); Events", 40, -1, 1);  
  TH1D *h7 = new TH1D("h7", "6 MeV e- throughout AV, Direction Only Fit, 2g/l; cos (#alpha); Events", 40, -1, 1);
  TH1D *h8 = new TH1D("h8", "6 MeV e- throughout AV, Direction Only Fit, 2g/l; cos (#alpha); Events", 40, -1, 1);*/

  TCanvas* c1 = new TCanvas("c1", "c1", 1000,600);

  tree1->Draw("(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) ) >>h1");
  h1->GetYaxis()->SetTitleOffset(1.3);
  h1->Scale(1/h1->Integral());
  h1->GetYaxis()->SetRangeUser(0,0.25);
  h1->SetLineColor(kRed+1);
  h1->SetLineWidth(2);

  tree2->Draw("(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) ) >> h2", "", "same");
  h2->GetYaxis()->SetTitleOffset(1.3);
  h2->Scale(1/h2->Integral());
  h2->SetLineColor(kBlack);
  h2->SetLineWidth(2);
  h2->SetLineStyle(2);
  /*
  tree3->Draw("(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) ) >> h3", "", "same");
  h3->GetYaxis()->SetTitleOffset(1.3);
  h3->Scale(1/h3->Integral());
  h3->SetLineColor(kRed+1);
  h3->SetLineWidth(2);
  h3->SetLineStyle(2);

  tree4->Draw("(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) ) >> h4", "" , "same");
  h4->GetYaxis()->SetTitleOffset(1.3);
  h4->Scale(1/h4->Integral());
  h4->SetLineColor(kBlue+1);
  h4->SetLineWidth(2);
  h4->SetLineStyle(2);
  /*      
  tree5->Draw("(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) ) >> h5", "", "same");
  h5->GetYaxis()->SetTitleOffset(1.3);
  h5->Scale(1/h5->Integral());
  h5->SetLineColor(kCyan+1);
  h5->SetLineWidth(2);

  tree6->Draw("(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) ) >> h6", "" , "same");
  h6->GetYaxis()->SetTitleOffset(1.3);
  h6->Scale(1/h6->Integral());
  h6->SetLineColor(kMagenta+1);
  h6->SetLineWidth(2);

  tree7->Draw("(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) ) >> h7", "", "same");
  h7->GetYaxis()->SetTitleOffset(1.3);
  h7->Scale(1/h7->Integral());
  h7->SetLineColor(kBlack);
  h7->SetLineWidth(2);

  tree8->Draw("(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) ) >> h8", "" , "same");
  h8->GetYaxis()->SetTitleOffset(1.3);
  h8->Scale(1/h8->Integral());
  h8->SetLineColor(kGray+1);
  h8->SetLineWidth(2);
  */
  TLegend *leg = new TLegend(0.2,0.5,0.55,0.79);
  leg->AddEntry(h1, "Fit Position", "l");
  leg->AddEntry(h2, "Fit Position, No Scattering", "l");
  //leg->AddEntry(h1, "Fit Position, No Scattering", "l");
  //leg->AddEntry(h2, "True Position, No Scattering", "l");
  //leg->AddEntry(h5, "Grid 10", "l");
  //leg->AddEntry(h6, "Simulated Annealing", "l");
  // leg->AddEntry(h7, "Powell", "l");
  //leg->AddEntry(h8, "Minuit", "l");
  leg->SetLineWidth(2);

  h1->Draw();
  h2->Draw("same");
  //  h3->Draw("same");
  //h4->Draw("same");
  //h5->Draw("same");
  //h6->Draw("same");
  //h7->Draw("same");
  //h8->Draw("same");
  leg->Draw("same");
}


void CompareTwoCosAlphas(std::string filename1, std::string label1, std::string filename2, std::string label2){

  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TFile *file1 = TFile::Open(filename1.c_str());
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree1");
  TFile *file2 = TFile::Open(filename2.c_str());
  TTree* tree2 = (TTree*)file2->Get("eveTree")->Clone("tree2");

  TCanvas* c1 = new TCanvas("c1", "c1", 1000,600);

  tree1->Draw("(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) ) >>h1");
  TH1F *htemp1 = (TH1F*)gPad->GetPrimitive("h1");
  htemp1->GetXaxis()->SetTitle("cos (#alpha)");
  htemp1->GetYaxis()->SetTitle("Events");
  htemp1->GetYaxis()->SetTitleOffset(1.3);
  htemp1->GetXaxis()->SetRangeUser(-1,1);
  htemp1->GetYaxis()->SetRangeUser(0,0.05);
  htemp1->Scale(1/htemp1->Integral());
  htemp1->SetTitle("B8 #nu_{#mu}");
  htemp1->SetLineColor(kRed);
  htemp1->SetLineWidth(2);
  htemp1->GetYaxis()->SetRangeUser(0,0.05);
  
  tree2->Draw("(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue) ) >> h2", "", "same");
  TH1F *htemp2 = (TH1F*)gPad->GetPrimitive("h2");
  htemp2->GetXaxis()->SetTitle("cos (#alpha)");
  htemp2->GetYaxis()->SetTitle("Events");
  htemp2->GetYaxis()->SetTitleOffset(1.3);
  htemp2->GetXaxis()->SetRangeUser(-1,1);
  htemp2->Scale(1/htemp2->Integral());
  htemp2->SetLineColor(kBlue);
  htemp2->SetLineWidth(2);
  htemp2->GetYaxis()->SetRangeUser(0,0.05);
  
  TLegend *leg = new TLegend(0.2,0.6,0.75,0.79);
  leg->AddEntry(htemp1, label1.c_str(), "l");
  leg->AddEntry(htemp2, label2.c_str(), "l");
  leg->SetLineWidth(2);    

  htemp1->Draw();
  htemp2->Draw("same");
  leg->Draw("same");
}



void CompareTwoSolarangles(std::string filename1, std::string label1, std::string filename2, std::string label2){

  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TFile *file1 = TFile::Open(filename1.c_str());
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree1");
  TFile *file2 = TFile::Open(filename2.c_str());
  TTree* tree2 = (TTree*)file2->Get("eveTree")->Clone("tree2");

  TCanvas* c1 = new TCanvas("c1", "c1", 1000,600);

  tree1->Draw("(xdirFit*-xSol + ydirFit*-ySol + zdirFit*-zSol)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xSol*xSol + ySol*ySol + zSol*zSol) ) >>h1");
  TH1F *htemp1 = (TH1F*)gPad->GetPrimitive("h1");
  htemp1->GetXaxis()->SetTitle("cos (#theta_{Sun})");
  htemp1->GetYaxis()->SetTitle("Events");
  htemp1->GetYaxis()->SetTitleOffset(1.3);
  htemp1->GetXaxis()->SetRangeUser(-1,1);
  htemp1->Scale(1/htemp1->Integral());
  htemp1->SetTitle("B8 #nu_{#mu}");
  htemp1->SetLineColor(kRed);
  htemp1->SetLineWidth(2);
  htemp1->GetYaxis()->SetRangeUser(0,0.05);
  
  tree2->Draw("(xdirFit*-xSol + ydirFit*-ySol + zdirFit*-zSol)/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)*sqrt(xSol*xSol + ySol*ySol + zSol*zSol) ) >> h2", "", "same");
  TH1F *htemp2 = (TH1F*)gPad->GetPrimitive("h2");
  htemp2->GetXaxis()->SetTitle("cos (#theta_{Sun})");
  htemp2->GetYaxis()->SetTitle("Events");
  htemp2->GetYaxis()->SetTitleOffset(1.3);
  htemp2->GetXaxis()->SetRangeUser(-1,1);
  htemp2->Scale(1/htemp2->Integral());
  htemp2->SetLineColor(kBlue);
  htemp2->SetLineWidth(2);
  htemp2->GetYaxis()->SetRangeUser(0,0.05);
  
  TLegend *leg = new TLegend(0.2,0.6,0.75,0.79);
  leg->AddEntry(htemp1, label1.c_str(), "l");
  leg->AddEntry(htemp2, label2.c_str(), "l");
  leg->SetLineWidth(2);

  htemp1->Draw();
  htemp2->Draw("same");
  leg->Draw("same");
}
