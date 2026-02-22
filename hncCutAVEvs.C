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

void hncCutAVEvs(){

  gStyle->SetPalette(51);
  gStyle->SetLineWidth(2);
  gStyle->SetOptStat(0);

  std::string plotprefix = "Feb19_e_avcut_";

  std::string axs[4] = {"posx:mcPosx", "posy:mcPosy", "posz-184.4:mcPosz-184.4", "sqrt(posx*posx+posy*posy+(posz-184.4)*(posz-184.4)):sqrt(mcPosx*mcPosx+mcPosy*mcPosy+(mcPosz-184.4)*(mcPosz-184.4))"};
  std::string title = "Reconstructed";

  std::string coord[4] = {"X", "Y", "Z", "R"};

  std::string cut1 = "(sqrt(posx*posx+posy*posy+(posz-184.4)*(posz-184.4)) < 6000 && (dcFlagged & 256) == 256 && fitValid == 1 && evIndex == 0 )";

  std::string cut2 = "(necknhits / nhitsCleaned < 0.004 && sqrt( (seedxPosError-seedxNegError)*(seedxPosError-seedxNegError) + (seedyPosError-seedyNegError)*(seedyPosError-seedyNegError) + (seedzPosError-seedzNegError)*(seedzPosError-seedzNegError)) < 600 &&  sqrt( (posxPosError-posxNegError)*(posxPosError-posxNegError) + (posyPosError-posyNegError)*(posyPosError-posyNegError) + (poszPosError-poszNegError)*(poszPosError-poszNegError)) < 800 && sqrt( (posx-seedx)*(posx-seedx)+(posy-seedy)*(posy-seedy)+(posz-seedz)*(posz-seedz) ) < 1000 && posFOM/posFOM2 > 12.0 && itr > 0.2)";

  std::string cut3 = " (sqrt( mcPosx*mcPosx + mcPosy*mcPosy + mcPosz*mcPosz ) < 6000 )";
  
  TFile *fFile = TFile::Open("/home/parkerw/Software/rat-tools2/FitCoordination/HighNeck/Feb19_e_combined.root");
  TTree* fTree = (TTree*)fFile->Get("output")->Clone("output");

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH1F *htemp1 = new TH1F("htemp1", "htemp1", 100, 0, 2);
  fTree->Draw("energy >> htemp1", ( cut1 + " && !( " + cut2 + ") && " + cut3 ).c_str(), "colz");
  std::cout << ( cut1 + " && !( " + cut2 + ") && " + cut3 ).c_str() << std::endl;
  std::string ytitle = "Incorrectly Tagged High Neck Events";
  std::string xtitle = "Reconstructed Energy, MeV";
  htemp1->GetXaxis()->SetTitle(xtitle.c_str());
  htemp1->GetYaxis()->SetTitle(ytitle.c_str());
  htemp1->SetTitle("");
  htemp1->GetYaxis()->SetTitleOffset(0.9);
  htemp1->GetXaxis()->SetTitleSize(0.05);
  htemp1->GetYaxis()->SetTitleSize(0.05);
  htemp1->SetLineColor(kBlue);
  htemp1->SetLineWidth(2);
  gPad->Update();
  c1->SaveAs((plotprefix+"cutEnergy.pdf").c_str());

  TCanvas* c2 = new TCanvas("c2", "c2", 1500,800);
  c2->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH1F *htemp2 = new TH1F("htemp2", "htemp2", 100, 0, 1000);
  fTree->Draw("nhitsCleaned >> htemp2",  ( cut1 + " && !( " + cut2 + ") && " + cut3 ).c_str(), "colz");
  ytitle = "Incorrectly Tagged High Neck Events";
  xtitle = "Nhits";
  htemp2->GetXaxis()->SetTitle(xtitle.c_str());
  htemp2->GetYaxis()->SetTitle(ytitle.c_str());
  htemp2->SetTitle("");
  htemp2->GetYaxis()->SetTitleOffset(0.9);
  htemp2->GetXaxis()->SetTitleSize(0.05);
  htemp2->GetYaxis()->SetTitleSize(0.05);
  htemp2->SetLineColor(kBlue);
  htemp2->SetLineWidth(2);
  gPad->Update();
  c2->SaveAs((plotprefix+"cutNhits.pdf").c_str());

  TCanvas* c3 = new TCanvas("c3", "c3", 1500, 800);
  c3->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH1F *htemp3 = new TH1F("htemp3", "htemp3", 100, 0, 2000);
  fTree->Draw("sqrt( (posx-mcPosx)*(posx-mcPosx) + (posy-mcPosy)*(posy-mcPosy) + (posz-mcPosz)*(posz-mcPosz) ) >> htemp3", ( cut1 + " && !( " + cut2 + ") && " + cut3 ).c_str(), "colz");
  ytitle = "Incorrectly Tagged High Neck Events";
  xtitle = "Fit - Truth Distance, mm";
  htemp3->GetXaxis()->SetTitle(xtitle.c_str());
  htemp3->GetYaxis()->SetTitle(ytitle.c_str());
  htemp3->SetTitle("");
  htemp3->GetYaxis()->SetTitleOffset(0.9);
  htemp3->GetXaxis()->SetTitleSize(0.05);
  htemp3->GetYaxis()->SetTitleSize(0.05);
  htemp3->SetLineColor(kBlue);
  htemp3->SetLineWidth(2);
  gPad->Update();
  c3->SaveAs((plotprefix+"cutFitDist.pdf").c_str());

  TCanvas* c4 = new TCanvas("c4", "c4", 1500, 800);
  c4->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH1F *htemp4 = new TH1F("htemp4", "htemp4", 100, 0, 2000);
  fTree->Draw("sqrt( (posx-mcPosx)*(posx-mcPosx) + (posy-mcPosy)*(posy-mcPosy) + (posz-mcPosz)*(posz-mcPosz) ) >> htemp4", (cut1 + " && " + cut3 ).c_str(), "colz");
  ytitle = "Events";
  xtitle = "Fit - Truth Distance, mm";
  htemp4->GetXaxis()->SetTitle(xtitle.c_str());
  htemp4->GetYaxis()->SetTitle(ytitle.c_str());
  htemp4->SetTitle("");
  htemp4->GetYaxis()->SetTitleOffset(0.9);
  htemp4->GetXaxis()->SetTitleSize(0.05);
  htemp4->GetYaxis()->SetTitleSize(0.05);
  htemp4->SetLineColor(kBlue);
  htemp4->SetLineWidth(2);
  gPad->Update();
  c4->SaveAs((plotprefix+"allFitDist.pdf").c_str());
  
}

