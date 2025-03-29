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

void Plot2D(){

  gStyle->SetPalette(51);
  gStyle->SetLineWidth(2);
  gStyle->SetOptStat(0);

  std::string plotprefix = "alpha_tmin_";

  std::string axs[4] = {"xFit:xTrue", "yFit:yTrue", "zFit-184.4:zTrue-184.4", "sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)):sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4))"};
  std::string title = "Reconstructed";

  std::string coord[4] = {"X", "Y", "Z", "R"};

  std::string cut1 = "sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)) < 6000 && neckcut == 1";
  std::string cut2 = "&& neckHit / nhits < 0.01 && sqrt( (xSeedPosErr-xSeedNegErr)*(xSeedPosErr-xSeedNegErr) + (ySeedPosErr-ySeedNegErr)*(ySeedPosErr-ySeedNegErr) + (zSeedPosErr-zSeedNegErr)*(zSeedPosErr-zSeedNegErr)) < 250 &&  sqrt( (xFitPosErr-xFitNegErr)*(xFitPosErr-xFitNegErr) + (yFitPosErr-yFitNegErr)*(yFitPosErr-yFitNegErr) + (zFitPosErr-zFitNegErr)*(zFitPosErr-zFitNegErr)) < 1200 && sqrt( (xFit-xSeed)*(xFit-xSeed)+(yFit-ySeed)*(yFit-ySeed)+(zFit-zSeed)*(zFit-zSeed) ) < 1000 && scaledLLH > 12.0 && itr > 0.2";
  //std::string cut2 = "&&  neckHit / nhits < 0.01 && sqrt( (xSeedPosErr-xSeedNegErr)*(xSeedPosErr-xSeedNegErr) + (ySeedPosErr-ySeedNegErr)*(ySeedPosErr-ySeedNegErr) + (zSeedPosErr-zSeedNegErr)*(zSeedPosErr-zSeedNegErr)) < 250 && sqrt( (xFitPosErr-xFitNegErr)*(xFitPosErr-xFitNegErr) + (yFitPosErr-yFitNegErr)*(yFitPosErr-yFitNegErr) + (zFitPosErr-zFitNegErr)*(zFitPosErr-zFitNegErr)) < 900 && sqrt( (xFit-xSeed)*(xFit-xSeed)+(yFit-ySeed)*(yFit-ySeed)+(zFit-zSeed)*(zFit-zSeed) ) < 1000 ";//"&& scaledLLH > 12.0 && neckHit/nhits < 0.01 && itr > 0.2 && highOwl / nhits < 0.01";
  // std::string cut2 = "&& scaledLLH > 12.0 && neckHit/nhits < 0.01 && itr > 0.2 && highOwl / nhits < 0.01";

  // sqrt( (xSeedPosErr-xSeedNegErr)*(xSeedPosErr-xSeedNegErr) + (ySeedPosErr-ySeedNegErr)*(ySeedPosErr-ySeedNegErr) + (zSeedPosErr-zSeedNegErr)*(zSeedPosErr-zSeedNegErr))
  // sqrt( (xFit-xSeed)*(xFit-xSeed)+(yFit-ySeed)*(yFit-ySeed)+(zFit-zSeed)*(zFit-zSeed) )
  // sqrt( (xFitPosErr-xFitNegErr)*(xFitPosErr-xFitNegErr) + (yFitPosErr-yFitNegErr)*(yFitPosErr-yFitNegErr) + (zFitPosErr-zFitNegErr)*(zFitPosErr-zFitNegErr))
  
  // TFile *fFile = TFile::Open("/home/parkerw/Scripts/Jan8_NearAV_pmtzratio.root");
  //  TFile *fFile = TFile::Open("/home/parkerw/Scripts/Feb5_alpha_pmtzratio.root");
  //  TFile *fFile = TFile::Open("/home/parkerw/Scripts/Jan8_NearAV_ETree.root");
  //  TFile *fFile = TFile::Open("/home/parkerw/Scripts/Feb9_e_Tree.root");
  TFile *fFile = TFile::Open("/home/parkerw/Scripts/Mar26_quad_alpha_tspread_Tree.root");
  //  TFile *fFile = TFile::Open("/home/parkerw/Scripts/Jan8_NearAV_timepeakstree.root");
  //TFile *fFile = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/Dec21_alpha_owlTree.root");
  TTree* fTree = (TTree*)fFile->Get("eveTree")->Clone("fTree");

  fTree->Draw("sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)):sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4))",(cut1+"&& sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) < 6184.4").c_str(),"colz");
  TH2F *hAVBefore = (TH2F*)gPad->GetPrimitive("htemp");
  float numAVBefore = hAVBefore->Integral();

  fTree->Draw("sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)):sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4))",(cut1+cut2+"&& sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) < 6184.4").c_str(),"colz");
  TH2F *hAVAfter = (TH2F*)gPad->GetPrimitive("htemp");
  float numAVAfter = hAVAfter->Integral();

  fTree->Draw("sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)):sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4))",(cut1+"&& sqrt(xTrue*xTrue + yTrue*yTrue + (zTrue-184.4)*(zTrue-184.4)) < 5700").c_str(),"colz");
  TH2F *hFVBefore = (TH2F*)gPad->GetPrimitive("htemp");
  float numFVBefore = hFVBefore->Integral();

  fTree->Draw("sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)):sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4))",(cut1+cut2+"&& sqrt(xTrue*xTrue + yTrue*yTrue + (zTrue-184.4)*(zTrue-184.4)) < 5700").c_str(),"colz");
  TH2F *hFVAfter = (TH2F*)gPad->GetPrimitive("htemp");
  float numFVAfter = hFVAfter->Integral();

  fTree->Draw("sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)):sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4))",(cut1+"&& sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) > 7000").c_str(),"colz");
  TH2F *hNeckBefore = (TH2F*)gPad->GetPrimitive("htemp");
  float numNeckBefore = hNeckBefore->Integral();

  fTree->Draw("sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)):sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4))",(cut1+cut2+"&& sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) > 7000").c_str(),"colz");
  TH2F *hNeckAfter = (TH2F*)gPad->GetPrimitive("htemp");
  float numNeckAfter = hNeckAfter->Integral();

  fTree->Draw("sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)):sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4))",(cut1+"&& sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) > 7000 && sqrt(xFit*xFit + yFit*yFit + (zFit-184.4)*(zFit-184.4)) < 6000").c_str(),"colz");
  TH2F *hNeckMisRecoBefore = (TH2F*)gPad->GetPrimitive("htemp");
  float numNeckMisRecoBefore = hNeckMisRecoBefore->Integral();

  fTree->Draw("sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)):sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4))",(cut1+cut2+"&& sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) > 7000 && sqrt(xFit*xFit + yFit*yFit + (zFit-184.4)*(zFit-184.4)) < 6000").c_str(),"colz");
  TH2F *hNeckMisRecoAfter = (TH2F*)gPad->GetPrimitive("htemp");
  float numNeckMisRecoAfter = hNeckMisRecoAfter->Integral();

  fTree->Draw("sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)):sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4))",(cut1+"&& sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) > 7000 && sqrt(xFit*xFit + yFit*yFit + (zFit-184.4)*(zFit-184.4)) < 5700").c_str(),"colz");
  TH2F *hNeckMisRecoFVBefore = (TH2F*)gPad->GetPrimitive("htemp");
  float numNeckMisRecoFVBefore = hNeckMisRecoFVBefore->Integral();

  fTree->Draw("sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)):sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4))",(cut1+cut2+"&& sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) > 7000 && sqrt(xFit*xFit + yFit*yFit + (zFit-184.4)*(zFit-184.4)) < 5700").c_str(),"colz");
  TH2F *hNeckMisRecoFVAfter = (TH2F*)gPad->GetPrimitive("htemp");
  float numNeckMisRecoFVAfter = hNeckMisRecoFVAfter->Integral();
 
  std::cout << "Num Events in AV Before Cut " << numAVBefore << std::endl;
  std::cout << "Num Events in AV After Cut " << numAVAfter << std::endl;
  std::cout << "% lost = " << 100*(numAVBefore-numAVAfter)/numAVBefore << "; % kept = " << 100 - (100*(numAVBefore-numAVAfter)/numAVBefore) << std::endl << std::endl;

  std::cout << "Num Events in FV Before Cut " << numFVBefore << std::endl;
  std::cout << "Num Events in FV After Cut " << numFVAfter << std::endl;
  std::cout << "% lost = " << 100*(numFVBefore-numFVAfter)/numFVBefore << "; % kept = " << 100 - (100*(numFVBefore-numFVAfter)/numFVBefore) << std::endl << std::endl;
 
  std::cout << "Num Events in Neck Before Cut " << numNeckBefore << std::endl;
  std::cout << "Num Events in Neck After Cut " << numNeckAfter << std::endl;
  std::cout << "% lost = " << 100*(numNeckBefore-numNeckAfter)/numNeckBefore << "; % kept = " << 100 - (100*(numNeckBefore-numNeckAfter)/numNeckBefore) << std::endl << std::endl;
  
  std::cout << "Num Events in Neck MisRecon into AV Before Cut " << numNeckMisRecoBefore << std::endl;
  std::cout << "Num Events in Neck MisRecon into AV After Cut " << numNeckMisRecoAfter << std::endl;
  std::cout << "% lost = " << 100*(numNeckMisRecoBefore-numNeckMisRecoAfter)/numNeckMisRecoBefore << "; % kept = " << 100 - (100*(numNeckMisRecoBefore-numNeckMisRecoAfter)/numNeckMisRecoBefore) << std::endl << std::endl;

  std::cout << "Num Events in Neck MisRecon into FV Before Cut " << numNeckMisRecoFVBefore << std::endl;
  std::cout << "Num Events in Neck MisRecon into FV After Cut " << numNeckMisRecoFVAfter << std::endl;
  std::cout << "% lost = " << 100*(numNeckMisRecoFVBefore-numNeckMisRecoFVAfter)/numNeckMisRecoFVBefore << "; % kept = " << 100 - (100*(numNeckMisRecoFVBefore-numNeckMisRecoFVAfter)/numNeckMisRecoFVBefore) << std::endl << std::endl;
  

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->SetRightMargin(0.13);
  c1->Divide(2,2);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);  

  //std::string cut = "sqrt(xTrue*xTrue + yTrue*yTrue + (zTrue-184.4)*(zTrue-184.4)) < 6000 && nearAV > 0.28";//"(zFit-184.4)<7000 && zFit>0 && xFit<7000 && xFit>-7000 && yFit<7000 && yFit>-7000";
  //std::string cut = "neckHit<2 && scaledLLH  > 14.2 && nearAV > 0.28";

  //  std::string cut = "scaledLLH  > 12.4";//"beta14 > -0.05 && scaledLLH  > 13.5 && zFit < 40000";// "itr > 0.18 && scaledLLH  > 14.2 && nearAV > 0.28";

  for(int i=0; i<4; i++){
    c1->cd(i+1);
    gPad->SetFrameLineWidth(2);
    gPad->SetGrid(1);

    fTree->Draw(axs[i].c_str(),(cut1+cut2).c_str(),"colz");
    TH2F *htemp = (TH2F*)gPad->GetPrimitive("htemp");
    std::string xtitle = "True " + coord[i] + ", mm";
    std::string ytitle = "Fitted " + coord[i] + ", mm";
    htemp->GetXaxis()->SetTitle(xtitle.c_str());
    htemp->GetYaxis()->SetTitle(ytitle.c_str());
    htemp->SetTitle("");
    //htemp->GetYaxis()->SetTitleOffset(1.0);
    htemp->GetXaxis()->SetTitleSize(0.05);
    htemp->GetYaxis()->SetTitleSize(0.05);
    gPad->Update();
    
  }
  c1->SaveAs((plotprefix+"Res.pdf").c_str());

  TCanvas* c2 = new TCanvas("c2", "c2", 1500,800);
  c2->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  TH2F *htemp2 = new TH2F("htemp2","htemp2", 140, 0, 14000, 100, 0, 300);
  fTree->Draw("tMin:sqrt(xTrue*xTrue + yTrue*yTrue + (zTrue-184.4)*(zTrue-184.4)) >> htemp2", (cut1+cut2).c_str(), "colz");
  //fTree->Draw("scaledLLH:sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue) ) >> htemp2", (cut1+cut2).c_str(), "colz");
  //TH2F *htemp2 = (TH2F*)gPad->GetPrimitive("htemp");
  std::cout << htemp2->Integral() << " " << htemp2->GetXaxis()->GetNbins() << " " << htemp2->GetYaxis()->GetNbins() << std::endl;
  std::string xtitle = "True Radius, mm";
  //std::string xtitle = "Fit - True Position, mm";
  std::string ytitle = "Earliest Hit Time, ns";
  htemp2->GetXaxis()->SetTitle(xtitle.c_str());
  htemp2->GetYaxis()->SetTitle(ytitle.c_str());
  htemp2->SetTitle("");
  htemp2->GetYaxis()->SetTitleOffset(0.8);
  htemp2->GetXaxis()->SetTitleOffset(0.8);
  htemp2->GetXaxis()->SetTitleSize(0.06);
  htemp2->GetYaxis()->SetTitleSize(0.06);
  c2->SetLogz();
  c2->SaveAs((plotprefix+"vsTrueR.pdf").c_str());


  TCanvas* c3 = new TCanvas("c3", "c3", 1500,800);
  c3->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  fTree->Draw("sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)):sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4))",(cut1+cut2).c_str(),"colz");
  TH2F *htemp = (TH2F*)gPad->GetPrimitive("htemp");
  xtitle = "True Radius, mm";
  ytitle = "Fitted Radius, mm";
  htemp->GetXaxis()->SetTitle(xtitle.c_str());
  htemp->GetYaxis()->SetTitle(ytitle.c_str());
  htemp->SetTitle("");
  //htemp->GetYaxis()->SetTitleOffset(1.0);
  htemp->GetXaxis()->SetTitleSize(0.05);
  htemp->GetYaxis()->SetTitleSize(0.05);
  c3->SetLogz();
  gPad->Update();
  c3->SaveAs((plotprefix+"ResR.pdf").c_str());

  TCanvas* c4 = new TCanvas("c4", "c4", 1500,800);
  c4->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp3 = new TH2F("htemp3","htemp3", 100, 0, 14000, 100, 8, 15);
  fTree->Draw("scaledLLH:sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue) ) >> htemp3",(cut1+cut2).c_str(),"colz");
  //  TH2F *htemp3 = (TH2F*)gPad->GetPrimitive("htemp");
  //xtitle = "True Radius, mm";
  ytitle = "Scaled LLH";
  xtitle = "Fitted-True Distance, mm";
  htemp3->GetXaxis()->SetTitle(xtitle.c_str());
  htemp3->GetYaxis()->SetTitle(ytitle.c_str());
  htemp3->SetTitle("");
  //htemp3->GetYaxis()->SetTitleOffset(1.0);
  htemp3->GetXaxis()->SetTitleSize(0.05);
  htemp3->GetYaxis()->SetTitleSize(0.05);
  gPad->Update();
  c4->SetLogz();
  c4->SaveAs((plotprefix+"FitDist.pdf").c_str());

  TCanvas* c5 = new TCanvas("c5", "c5", 1500,800);
  c5->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp5 = new TH2F("htemp5","htemp5", 140, 8, 14, 100, 0, 10000);
  fTree->Draw("sqrt( (xFitPosErr-xFitNegErr)*(xFitPosErr-xFitNegErr) + (yFitPosErr-yFitNegErr)*(yFitPosErr-yFitNegErr) + (zFitPosErr-zFitNegErr)*(zFitPosErr-zFitNegErr)  ):scaledLLH >> htemp5", (cut1+cut2).c_str(), "colz");
  std::string ytitle5 = "MPDF Fit Uncertainty";
  std::string xtitle5 = "MPDF FOM / NHits";
  htemp5->GetXaxis()->SetTitle(xtitle5.c_str());
  htemp5->GetYaxis()->SetTitle(ytitle5.c_str());
  htemp5->SetTitle("");
  //htemp2->GetYaxis()->SetTitleOffset(1.0);
  htemp5->GetXaxis()->SetTitleSize(0.05);
  htemp5->GetYaxis()->SetTitleSize(0.05);
  c5->SetLogz();
  c5->SaveAs((plotprefix+"vsScaledLLH.pdf").c_str());
  
}

