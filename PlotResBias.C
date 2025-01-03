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


void PlotNearAVRad(){

  gStyle->SetPalette(51);
  gStyle->SetLineWidth(2);
  gStyle->SetOptStat(0);

  TFile *fFile = TFile::Open("/home/parkerw/Scripts/Dec30_4to6m_Tree.root");
  TTree* fTree = (TTree*)fFile->Get("eveTree")->Clone("fTree");

  std::string cut1 = "nearAV>0 && xSeed > -999999";

  TH1F *htemp1 = new TH1F("htemp1","htemp1", 25, 5000, 6000);
  fTree->Draw("sqrt(xTrue*xTrue+yTrue*yTrue+zTrue*zTrue) >> htemp1", "", "");
  // TH1F *htemp2 = new TH1F("htemp2","htemp2", 25, 5000, 6000);
  fTree->Draw("sqrt(xTrue*xTrue+yTrue*yTrue+zTrue*zTrue) >> htemp2", cut1.c_str(), "");

  htemp1->SetTitle("");
  htemp1->GetXaxis()->SetTitle("MC Radius, mm");
  htemp1->GetYaxis()->SetTitle("Events");
  
  htemp1->SetLineWidth(2);
  htemp1->SetLineColor(kBlack);
  htemp2->SetLineWidth(2);
  htemp2->SetLineColor(kBlue);

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  htemp1->Draw();
  htemp2->Draw("same");

  TLegend* t1 = new TLegend( 0.15, 0.65, 0.35, 0.85 );
  t1->AddEntry( htemp1, "All Events", "l");
  t1->AddEntry( htemp2, "NearAV Fitter Applied", "l" );
  t1->Draw();

  c1->SaveAs("radialdist.pdf");

  /*
  std::string plotprefix = "e_test1m_";

  std::string axs[4] = {"xFit:xTrue", "yFit:yTrue", "zFit-184.4:zTrue-184.4", "sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)):sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4))"};
  std::string title = "Reconstructed";

  std::string coord[4] = {"X", "Y", "Z", "R"};

  std::string cut1 = "sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)) < 6000 && (dcflagged & 8) == 8 ";
  std::string cut2 = "";//"&& scaledLLH > 12.0 && neckHit/nhits < 0.01 && nearAV > 0.1 && itr > 0.18 && highOwl/nhits < 0.005";
    //"(neckHit >= 2 || nearAV < 0.1 || itr < 0.16 || neckHit/nhits > 0.03 || scaledLLH < 12.2) && sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4)) < 6000 && sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)) < 6000";//"sqrt(xFit*xFit+yFit*xyFit+(zFit-184.4)*(zFit-184.4)) < 20000 && neckHit/nhits < 0.04 && scaledLLH > 12.2";// && itr > 0.2 && nearAV > 0.1 && scaledLLH > 12.4 ";//"beta14 > -0.05 && scaledLLH  > 12.4 && zFit < 40000";// "itr > 0.18 && scaledLLH  > 14.2 && nearAV > 0.28";
  //std::string cut = "neckHit > 2 && nearAV > 0.1 && itr > 0.16 && neckHit/nhits > 0.03 && scaledLLH > 12.2"

  TFile *fFile = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/Dec21_e_owlTree.root");
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

  fTree->Draw("sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)):sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4))",(cut1+"&& sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) > 6184.4").c_str(),"colz");
  TH2F *hNeckBefore = (TH2F*)gPad->GetPrimitive("htemp");
  float numNeckBefore = hNeckBefore->Integral();

  fTree->Draw("sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)):sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4))",(cut1+cut2+"&& sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) > 6184.4 ").c_str(),"colz");
  TH2F *hNeckAfter = (TH2F*)gPad->GetPrimitive("htemp");
  float numNeckAfter = hNeckAfter->Integral();

  fTree->Draw("sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)):sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4))",(cut1+"&& sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) > 6184.4 && sqrt(xFit*xFit + yFit*yFit + (zFit-184.4)*(zFit-184.4)) < 6000").c_str(),"colz");
  TH2F *hNeckMisRecoBefore = (TH2F*)gPad->GetPrimitive("htemp");
  float numNeckMisRecoBefore = hNeckMisRecoBefore->Integral();

  fTree->Draw("sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)):sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4))",(cut1+cut2+"&& sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) > 6184.4 && sqrt(xFit*xFit + yFit*yFit + (zFit-184.4)*(zFit-184.4)) < 6000").c_str(),"colz");
  TH2F *hNeckMisRecoAfter = (TH2F*)gPad->GetPrimitive("htemp");
  float numNeckMisRecoAfter = hNeckMisRecoAfter->Integral();

  fTree->Draw("sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)):sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4))",(cut1+"&& sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) > 6184.4 && sqrt(xFit*xFit + yFit*yFit + (zFit-184.4)*(zFit-184.4)) < 5700").c_str(),"colz");
  TH2F *hNeckMisRecoFVBefore = (TH2F*)gPad->GetPrimitive("htemp");
  float numNeckMisRecoFVBefore = hNeckMisRecoFVBefore->Integral();

  fTree->Draw("sqrt(xFit*xFit+yFit*yFit+(zFit-184.4)*(zFit-184.4)):sqrt(xTrue*xTrue+yTrue*yTrue+(zTrue-184.4)*(zTrue-184.4))",(cut1+cut2+"&& sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) > 6184.4 && sqrt(xFit*xFit + yFit*yFit + (zFit-184.4)*(zFit-184.4)) < 5700").c_str(),"colz");
  TH2F *hNeckMisRecoFVAfter = (TH2F*)gPad->GetPrimitive("htemp");
  float numNeckMisRecoFVAfter = hNeckMisRecoFVAfter->Integral();
  
  std::cout << "Num Events in AV Before Cut " << numAVBefore << std::endl;
  std::cout << "Num Events in AV After Cut " << numAVAfter << std::endl;
  std::cout << "% lost = " << 100*(numAVBefore-numAVAfter)/numAVBefore << std::endl << std::endl;

  std::cout << "Num Events in FV Before Cut " << numFVBefore << std::endl;
  std::cout << "Num Events in FV After Cut " << numFVAfter << std::endl;
  std::cout << "% lost = " << 100*(numFVBefore-numFVAfter)/numFVBefore << std::endl << std::endl;
  
  std::cout << "Num Events in Neck Before Cut " << numNeckBefore << std::endl;
  std::cout << "Num Events in Neck After Cut " << numNeckAfter << std::endl;
  std::cout << "% lost = " << 100*(numNeckBefore-numNeckAfter)/numNeckBefore << std::endl << std::endl;

  std::cout << "Num Events in Neck MisRecon into AV Before Cut " << numNeckMisRecoBefore << std::endl;
  std::cout << "Num Events in Neck MisRecon into AV After Cut " << numNeckMisRecoAfter << std::endl;
  std::cout << "% lost = " << 100*(numNeckMisRecoBefore-numNeckMisRecoAfter)/numNeckMisRecoBefore << std::endl << std::endl;

  std::cout << "Num Events in Neck MisRecon into FV Before Cut " << numNeckMisRecoFVBefore << std::endl;
  std::cout << "Num Events in Neck MisRecon into FV After Cut " << numNeckMisRecoFVAfter << std::endl;
  std::cout << "% lost = " << 100*(numNeckMisRecoFVBefore-numNeckMisRecoFVAfter)/numNeckMisRecoFVBefore << std::endl << std::endl;


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
    htemp->GetYaxis()->SetTitleOffset(1.3);
    gPad->Update();
    
  }
  c1->SaveAs((plotprefix+"Res.pdf").c_str());

  TCanvas* c2 = new TCanvas("c2", "c2", 1500,800);
  c2->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  TH2F *htemp2 = new TH2F("htemp2","htemp2", 140, 0, 14000, 100, -1000, 0.7);
  fTree->Draw("nearAV:sqrt(xTrue*xTrue + yTrue*yTrue + (zTrue-184.4)*(zTrue-184.4)) >> htemp2", (cut1+cut2).c_str(), "colz");
  //fTree->Draw("scaledLLH:sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue) )", (cut1+cut2).c_str(), "colz");
  //TH2F *htemp2 = (TH2F*)gPad->GetPrimitive("htemp");
  std::string xtitle = "True Radius, mm";
  //std::string xtitle = "Fit - True Position, mm";
  std::string ytitle = "NearAV";
  htemp2->GetXaxis()->SetTitle(xtitle.c_str());
  htemp2->GetYaxis()->SetTitle(ytitle.c_str());
  htemp2->SetTitle("");
  htemp2->GetYaxis()->SetTitleOffset(1.3);
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
  htemp->GetYaxis()->SetTitleOffset(1.3);
  std::cout << htemp->Integral() << std::endl;
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
  htemp3->GetYaxis()->SetTitleOffset(1.3);
  gPad->Update();
  c4->SetLogz();
  c4->SaveAs((plotprefix+"FitDist.pdf").c_str());
  */

}

