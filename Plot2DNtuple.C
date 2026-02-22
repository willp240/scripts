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

void Plot2DNtuple(){

  gStyle->SetPalette(51);
  gStyle->SetLineWidth(2);
  gStyle->SetOptStat(0);

  std::string plotprefix = "alpha_ntupavonly_";

  std::string axs[4] = {"posx:mcPosx", "posy:mcPosy", "posz-184.4:mcPosz-184.4", "sqrt(posx*posx+posy*posy+(posz-184.4)*(posz-184.4)):sqrt(mcPosx*mcPosx+mcPosy*mcPosy+(mcPosz-184.4)*(mcPosz-184.4))"};
  std::string title = "Reconstructed";

  std::string coord[4] = {"X", "Y", "Z", "R"};

  std::string cut1 = "sqrt(posx*posx+posy*posy+(posz-184.4)*(posz-184.4)) < 6000 && (dcFlagged & 256) == 256 && fitValid == 1 && evIndex == 0 ";
  //  std::string cut2 = "";
  std::string cut2 = "&& sqrt(mcPosx*mcPosx+mcPosy*mcPosy+(mcPosz-184.4)*(mcPosz-184.4)) < 5700";
  //  std::string cut2 = "&& necknhits / nhitsCleaned < 0.01 && sqrt( (seedxPosError-seedxNegError)*(seedxPosError-seedxNegError) + (seedyPosError-seedyNegError)*(seedyPosError-seedyNegError) + (seedzPosError-seedzNegError)*(seedzPosError-seedzNegError)) < 250 &&  sqrt( (posxPosError-posxNegError)*(posxPosError-posxNegError) + (posyPosError-posyNegError)*(posyPosError-posyNegError) + (poszPosError-poszNegError)*(poszPosError-poszNegError)) < 1200 && sqrt( (posx-seedx)*(posx-seedx)+(posy-seedy)*(posy-seedy)+(posz-seedz)*(posz-seedz) ) < 1000 && posFOM/posFOM2 > 12.0 && itr > 0.2";

  TFile *fFile = TFile::Open("/home/parkerw/Software/rat-tools2/FitCoordination/HighNeck/Feb19_e_combined.root");
  TTree* fTree = (TTree*)fFile->Get("output")->Clone("output");
  /*
  fTree->Draw("sqrt(posx*posx+posy*posy+(posz-184.4)*(posz-184.4)):sqrt(mcPosx*mcPosx+mcPosy*mcPosy+(mcPosz-184.4)*(mcPosz-184.4))",(cut1+"&& sqrt(mcPosx*mcPosx + mcPosy*mcPosy + mcPosz*mcPosz) < 6184.4").c_str(),"colz");
  TH2F *hAVBefore = (TH2F*)gPad->GetPrimitive("htemp");
  float numAVBefore = hAVBefore->Integral();

  fTree->Draw("sqrt(posx*posx+posy*posy+(posz-184.4)*(posz-184.4)):sqrt(mcPosx*mcPosx+mcPosy*mcPosy+(mcPosz-184.4)*(mcPosz-184.4))",(cut1+cut2+"&& sqrt(mcPosx*mcPosx + mcPosy*mcPosy + mcPosz*mcPosz) < 6184.4").c_str(),"colz");
  TH2F *hAVAfter = (TH2F*)gPad->GetPrimitive("htemp");
  float numAVAfter = hAVAfter->Integral();

  fTree->Draw("sqrt(posx*posx+posy*posy+(posz-184.4)*(posz-184.4)):sqrt(mcPosx*mcPosx+mcPosy*mcPosy+(mcPosz-184.4)*(mcPosz-184.4))",(cut1+"&& sqrt(mcPosx*mcPosx + mcPosy*mcPosy + (mcPosz-184.4)*(mcPosz-184.4)) < 5700").c_str(),"colz");
  TH2F *hFVBefore = (TH2F*)gPad->GetPrimitive("htemp");
  float numFVBefore = hFVBefore->Integral();
  
  fTree->Draw("sqrt(posx*posx+posy*posy+(posz-184.4)*(posz-184.4)):sqrt(mcPosx*mcPosx+mcPosy*mcPosy+(mcPosz-184.4)*(mcPosz-184.4))",(cut1+cut2+"&& sqrt(mcPosx*mcPosx + mcPosy*mcPosy + (mcPosz-184.4)*(mcPosz-184.4)) < 5700").c_str(),"colz");
  TH2F *hFVAfter = (TH2F*)gPad->GetPrimitive("htemp");
  float numFVAfter = hFVAfter->Integral();

  fTree->Draw("sqrt(posx*posx+posy*posy+(posz-184.4)*(posz-184.4)):sqrt(mcPosx*mcPosx+mcPosy*mcPosy+(mcPosz-184.4)*(mcPosz-184.4))",(cut1+"&& sqrt(mcPosx*mcPosx + mcPosy*mcPosy + mcPosz*mcPosz) > 7000").c_str(),"colz");
  TH2F *hNeckBefore = (TH2F*)gPad->GetPrimitive("htemp");
  float numNeckBefore = hNeckBefore->Integral();

  fTree->Draw("sqrt(posx*posx+posy*posy+(posz-184.4)*(posz-184.4)):sqrt(mcPosx*mcPosx+mcPosy*mcPosy+(mcPosz-184.4)*(mcPosz-184.4))",(cut1+cut2+"&& sqrt(mcPosx*mcPosx + mcPosy*mcPosy + mcPosz*mcPosz) > 7000").c_str(),"colz");
  TH2F *hNeckAfter = (TH2F*)gPad->GetPrimitive("htemp");
  float numNeckAfter = hNeckAfter->Integral();

  fTree->Draw("sqrt(posx*posx+posy*posy+(posz-184.4)*(posz-184.4)):sqrt(mcPosx*mcPosx+mcPosy*mcPosy+(mcPosz-184.4)*(mcPosz-184.4))",(cut1+"&& sqrt(mcPosx*mcPosx + mcPosy*mcPosy + mcPosz*mcPosz) > 7000 && sqrt(posx*posx + posy*posy + (posz-184.4)*(posz-184.4)) < 6000").c_str(),"colz");
  TH2F *hNeckMisRecoBefore = (TH2F*)gPad->GetPrimitive("htemp");
  float numNeckMisRecoBefore = hNeckMisRecoBefore->Integral();

  fTree->Draw("sqrt(posx*posx+posy*posy+(posz-184.4)*(posz-184.4)):sqrt(mcPosx*mcPosx+mcPosy*mcPosy+(mcPosz-184.4)*(mcPosz-184.4))",(cut1+cut2+"&& sqrt(mcPosx*mcPosx + mcPosy*mcPosy + mcPosz*mcPosz) > 7000 && sqrt(posx*posx + posy*posy + (posz-184.4)*(posz-184.4)) < 6000").c_str(),"colz");
  TH2F *hNeckMisRecoAfter = (TH2F*)gPad->GetPrimitive("htemp");
  float numNeckMisRecoAfter = hNeckMisRecoAfter->Integral();

  fTree->Draw("sqrt(posx*posx+posy*posy+(posz-184.4)*(posz-184.4)):sqrt(mcPosx*mcPosx+mcPosy*mcPosy+(mcPosz-184.4)*(mcPosz-184.4))",(cut1+"&& sqrt(mcPosx*mcPosx + mcPosy*mcPosy + mcPosz*mcPosz) > 7000 && sqrt(posx*posx + posy*posy + (posz-184.4)*(posz-184.4)) < 5700").c_str(),"colz");
  TH2F *hNeckMisRecoFVBefore = (TH2F*)gPad->GetPrimitive("htemp");
  float numNeckMisRecoFVBefore = hNeckMisRecoFVBefore->Integral();
 
  fTree->Draw("sqrt(posx*posx+posy*posy+(posz-184.4)*(posz-184.4)):sqrt(mcPosx*mcPosx+mcPosy*mcPosy+(mcPosz-184.4)*(mcPosz-184.4))",(cut1+cut2+"&& sqrt(mcPosx*mcPosx + mcPosy*mcPosy + mcPosz*mcPosz) > 7000 && sqrt(posx*posx + posy*posy + (posz-184.4)*(posz-184.4)) < 5700").c_str(),"colz");
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
  */
 
  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->SetRightMargin(0.13);
  c1->Divide(2,2);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);  

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
    htemp->GetYaxis()->SetTitleOffset(1.0);
    htemp->GetXaxis()->SetTitleSize(0.05);
    htemp->GetYaxis()->SetTitleSize(0.05);
    gPad->Update();
    
  }
  c1->SaveAs((plotprefix+"Res.pdf").c_str());

  TCanvas* c3 = new TCanvas("c3", "c3", 1500,800);
  c3->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  fTree->Draw("sqrt(posx*posx+posy*posy+(posz-184.4)*(posz-184.4)):sqrt(mcPosx*mcPosx+mcPosy*mcPosy+(mcPosz-184.4)*(mcPosz-184.4))",(cut1+cut2).c_str(),"colz");
  TH2F *htemp3 = (TH2F*)gPad->GetPrimitive("htemp");
  std::string xtitle = "True Radius, mm";
  std::string ytitle = "Fitted Radius, mm";
  htemp3->GetXaxis()->SetTitle(xtitle.c_str());
  htemp3->GetYaxis()->SetTitle(ytitle.c_str());
  htemp3->SetTitle("");
  htemp3->GetYaxis()->SetTitleOffset(0.9);
  htemp3->GetXaxis()->SetTitleSize(0.05);
  htemp3->GetYaxis()->SetTitleSize(0.05);
  c3->SetLogz();
  gPad->Update();
  c3->SaveAs((plotprefix+"ResR.pdf").c_str());

  TCanvas* c4 = new TCanvas("c4", "c4", 1500,800);
  c4->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp4 = new TH2F("htemp4","htemp4", 100, 0, 14000, 100, 0, 0.4);
  fTree->Draw("necknhits / nhitsCleaned:sqrt( mcPosx*mcPosx + mcPosy*mcPosy + mcPosz*mcPosz ) >> htemp4",(cut1+cut2).c_str(),"colz");
  ytitle = "Neck Hits / Nhits";
  xtitle = "True Radius, mm";
  htemp4->GetXaxis()->SetTitle(xtitle.c_str());
  htemp4->GetYaxis()->SetTitle(ytitle.c_str());
  htemp4->SetTitle("");
  htemp4->GetYaxis()->SetTitleOffset(0.9);
  htemp4->GetXaxis()->SetTitleSize(0.05);
  htemp4->GetYaxis()->SetTitleSize(0.05);
  gPad->Update();
  c4->SetLogz();
  c4->SaveAs((plotprefix+"NeckHitR.pdf").c_str());
  
  TCanvas* c5 = new TCanvas("c5", "c5", 1500,800);
  c5->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp5 = new TH2F("htemp5","htemp5", 100, 0, 14000, 100, 0, 0.4);
  fTree->Draw("necknhits / nhitsCleaned:sqrt( (posx-mcPosx)*(posx-mcPosx) + (posy-mcPosy)*(posy-mcPosy) + (posz-mcPosz)*(posz-mcPosz) ) >> htemp5",(cut1+cut2).c_str(),"colz");
  ytitle = "Neck Hits / Nhits";
  xtitle = "Fitted-True Distance, mm";
  htemp5->GetXaxis()->SetTitle(xtitle.c_str());
  htemp5->GetYaxis()->SetTitle(ytitle.c_str());
  htemp5->SetTitle("");
  htemp5->GetYaxis()->SetTitleOffset(0.9);
  htemp5->GetXaxis()->SetTitleSize(0.05);
  htemp5->GetYaxis()->SetTitleSize(0.05);
  gPad->Update();
  c5->SetLogz();
  c5->SaveAs((plotprefix+"NeckHitD.pdf").c_str());

  TCanvas* c6 = new TCanvas("c6", "c6", 1500,800);
  c6->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp6 = new TH2F("htemp6","htemp6", 100, 0, 14000, 100, 0, 5000);
  fTree->Draw("sqrt( (seedxPosError-seedxNegError)*(seedxPosError-seedxNegError) + (seedyPosError-seedyNegError)*(seedyPosError-seedyNegError) + (seedzPosError-seedzNegError)*(seedzPosError-seedzNegError)):sqrt( mcPosx*mcPosx + mcPosy*mcPosy + mcPosz*mcPosz ) >> htemp6",(cut1+cut2).c_str(),"colz");
  ytitle = "Quad Uncertainty, mm";
  xtitle = "True Radius, mm";
  htemp6->GetXaxis()->SetTitle(xtitle.c_str());
  htemp6->GetYaxis()->SetTitle(ytitle.c_str());
  htemp6->SetTitle("");
  htemp6->GetYaxis()->SetTitleOffset(0.9);
  htemp6->GetXaxis()->SetTitleSize(0.05);
  htemp6->GetYaxis()->SetTitleSize(0.05);
  gPad->Update();
  c6->SetLogz();
  c6->SaveAs((plotprefix+"QuadUncertR.pdf").c_str());

  TCanvas* c7 = new TCanvas("c7", "c7", 1500,800);
  c7->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp7 = new TH2F("htemp7","htemp7", 100, 0, 14000, 100, 0, 5000);
  fTree->Draw("sqrt( (seedxPosError-seedxNegError)*(seedxPosError-seedxNegError) + (seedyPosError-seedyNegError)*(seedyPosError-seedyNegError) + (seedzPosError-seedzNegError)*(seedzPosError-seedzNegError)):sqrt( (posx-mcPosx)*(posx-mcPosx) + (posy-mcPosy)*(posy-mcPosy) + (posz-mcPosz)*(posz-mcPosz) ) >> htemp7",(cut1+cut2).c_str(),"colz");
  ytitle = "Quad Uncertainty, mm";
  xtitle = "Fitted-True Distance, mm";
  htemp7->GetXaxis()->SetTitle(xtitle.c_str());
  htemp7->GetYaxis()->SetTitle(ytitle.c_str());
  htemp7->SetTitle("");
  htemp7->GetYaxis()->SetTitleOffset(0.9);
  htemp7->GetXaxis()->SetTitleSize(0.05);
  htemp7->GetYaxis()->SetTitleSize(0.05);
  gPad->Update();
  c7->SetLogz();
  c7->SaveAs((plotprefix+"QuadUncertD.pdf").c_str());

  TCanvas* c8 = new TCanvas("c8", "c8", 1500,800);
  c8->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp8 = new TH2F("htemp8","htemp8", 100, 0, 14000, 100, 0, 5000);
  fTree->Draw("sqrt( (posxPosError-posxNegError)*(posxPosError-posxNegError) + (posyPosError-posyNegError)*(posyPosError-posyNegError) + (poszPosError-poszNegError)*(poszPosError-poszNegError)) : sqrt( mcPosx*mcPosx + mcPosy*mcPosy + mcPosz*mcPosz ) >> htemp8",(cut1+cut2).c_str(),"colz");
  ytitle = "MultiPDF Uncertainty, mm";
  xtitle = "True Radius, mm";
  htemp8->GetXaxis()->SetTitle(xtitle.c_str());
  htemp8->GetYaxis()->SetTitle(ytitle.c_str());
  htemp8->SetTitle("");
  htemp8->GetYaxis()->SetTitleOffset(0.9);
  htemp8->GetXaxis()->SetTitleSize(0.05);
  htemp8->GetYaxis()->SetTitleSize(0.05);
  gPad->Update();
  c8->SetLogz();
  c8->SaveAs((plotprefix+"MPDFUncertR.pdf").c_str());

  TCanvas* c9 = new TCanvas("c9", "c9", 1500,800);
  c9->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp9 = new TH2F("htemp9","htemp9", 100, 0, 14000, 100, 0, 5000);
  fTree->Draw("sqrt( (posxPosError-posxNegError)*(posxPosError-posxNegError) + (posyPosError-posyNegError)*(posyPosError-posyNegError) + (poszPosError-poszNegError)*(poszPosError-poszNegError)) : sqrt( (posx-mcPosx)*(posx-mcPosx) + (posy-mcPosy)*(posy-mcPosy) + (posz-mcPosz)*(posz-mcPosz) ) >> htemp9",(cut1+cut2).c_str(),"colz");
  ytitle = "MultiPDF Uncertainty, mm";
  xtitle = "Fitted-True Distance, mm";
  htemp9->GetXaxis()->SetTitle(xtitle.c_str());
  htemp9->GetYaxis()->SetTitle(ytitle.c_str());
  htemp9->SetTitle("");
  htemp9->GetYaxis()->SetTitleOffset(0.9);
  htemp9->GetXaxis()->SetTitleSize(0.05);
  htemp9->GetYaxis()->SetTitleSize(0.05);
  gPad->Update();
  c9->SetLogz();
  c9->SaveAs((plotprefix+"MPDFUncertD.pdf").c_str());

  TCanvas* c10 = new TCanvas("c10", "c10", 1500,800);
  c10->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp10 = new TH2F("htemp10","htemp10", 100, 0, 14000, 100, 0, 5000);
  fTree->Draw("sqrt( (posx-seedx)*(posx-seedx)+(posy-seedy)*(posy-seedy)+(posz-seedz)*(posz-seedz) ) : sqrt( mcPosx*mcPosx + mcPosy*mcPosy + mcPosz*mcPosz ) >> htemp10", (cut1+cut2).c_str(), "colz");
  ytitle = "Quad - MultiPDF Distance, mm";
  xtitle = "True Radius, mm";
  htemp10->GetXaxis()->SetTitle(xtitle.c_str());
  htemp10->GetYaxis()->SetTitle(ytitle.c_str());
  htemp10->SetTitle("");
  htemp10->GetYaxis()->SetTitleOffset(0.9);
  htemp10->GetXaxis()->SetTitleSize(0.05);
  htemp10->GetYaxis()->SetTitleSize(0.05);
  gPad->Update();
  c10->SetLogz();
  c10->SaveAs((plotprefix+"QuadMPDFDistR.pdf").c_str());

  TCanvas* c11 = new TCanvas("c11", "c11", 1500,800);
  c11->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp11 = new TH2F("htemp11","htemp11", 100, 0, 14000, 100, 0, 5000);
  fTree->Draw("sqrt( (posx-seedx)*(posx-seedx)+(posy-seedy)*(posy-seedy)+(posz-seedz)*(posz-seedz) ) : sqrt( (posx-mcPosx)*(posx-mcPosx) + (posy-mcPosy)*(posy-mcPosy) + (posz-mcPosz)*(posz-mcPosz) ) >> htemp11",(cut1+cut2).c_str(),"colz");
  ytitle = "Quad - MultiPDF, mm";
  xtitle = "Fitted-True Distance, mm";
  htemp11->GetXaxis()->SetTitle(xtitle.c_str());
  htemp11->GetYaxis()->SetTitle(ytitle.c_str());
  htemp11->SetTitle("");
  htemp11->GetYaxis()->SetTitleOffset(0.9);
  htemp11->GetXaxis()->SetTitleSize(0.05);
  htemp11->GetYaxis()->SetTitleSize(0.05);
  gPad->Update();
  c11->SetLogz();
  c11->SaveAs((plotprefix+"QuadMPDFDistD.pdf").c_str());

  TCanvas* c12 = new TCanvas("c12", "c12", 1500,800);
  c12->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp12 = new TH2F("htemp12","htemp12", 100, 0, 14000, 100, 5, 14);
  fTree->Draw("posFOM/posFOM2 : sqrt( mcPosx*mcPosx + mcPosy*mcPosy + mcPosz*mcPosz ) >> htemp12", (cut1+cut2).c_str(), "colz");
  ytitle = "Position FOM / Nhits";
  xtitle = "True Radius, mm";
  htemp12->GetXaxis()->SetTitle(xtitle.c_str());
  htemp12->GetYaxis()->SetTitle(ytitle.c_str());
  htemp12->SetTitle("");
  htemp12->GetYaxis()->SetTitleOffset(0.9);
  htemp12->GetXaxis()->SetTitleSize(0.05);
  htemp12->GetYaxis()->SetTitleSize(0.05);
  gPad->Update();
  c12->SetLogz();
  c12->SaveAs((plotprefix+"PosFOMR.pdf").c_str());

  TCanvas* c13 = new TCanvas("c13", "c13", 1500,800);
  c13->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp13 = new TH2F("htemp13","htemp13", 100, 0, 14000, 100, 5, 14);
  fTree->Draw("posFOM/posFOM2 : sqrt( (posx-mcPosx)*(posx-mcPosx) + (posy-mcPosy)*(posy-mcPosy) + (posz-mcPosz)*(posz-mcPosz) ) >> htemp13",(cut1+cut2).c_str(),"colz");
  ytitle = "Position FOM / Nhits";
  xtitle = "Fitted-True Distance, mm";
  htemp13->GetXaxis()->SetTitle(xtitle.c_str());
  htemp13->GetYaxis()->SetTitle(ytitle.c_str());
  htemp13->SetTitle("");
  htemp13->GetYaxis()->SetTitleOffset(0.9);
  htemp13->GetXaxis()->SetTitleSize(0.05);
  htemp13->GetYaxis()->SetTitleSize(0.05);
  gPad->Update();
  c13->SetLogz();
  c13->SaveAs((plotprefix+"PosFOMD.pdf").c_str());

  TCanvas* c14 = new TCanvas("c14", "c14", 1500,800);
  c14->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp14 = new TH2F("htemp14","htemp14", 100, 0, 14000, 100, 0, 1);
  fTree->Draw("itr : sqrt( mcPosx*mcPosx + mcPosy*mcPosy + mcPosz*mcPosz ) >> htemp14", (cut1+cut2).c_str(), "colz");
  ytitle = "ITR";
  xtitle = "True Radius, mm";
  htemp14->GetXaxis()->SetTitle(xtitle.c_str());
  htemp14->GetYaxis()->SetTitle(ytitle.c_str());
  htemp14->SetTitle("");
  htemp14->GetYaxis()->SetTitleOffset(0.9);
  htemp14->GetXaxis()->SetTitleSize(0.05);
  htemp14->GetYaxis()->SetTitleSize(0.05);
  gPad->Update();
  c14->SetLogz();
  c14->SaveAs((plotprefix+"itrR.pdf").c_str());

  TCanvas* c15 = new TCanvas("c15", "c15", 1500,800);
  c15->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp15 = new TH2F("htemp15","htemp15", 100, 0, 14000, 100, 0, 1);
  fTree->Draw("itr : sqrt( (posx-mcPosx)*(posx-mcPosx) + (posy-mcPosy)*(posy-mcPosy) + (posz-mcPosz)*(posz-mcPosz) ) >> htemp15",(cut1+cut2).c_str(),"colz");
  ytitle = "ITR";
  xtitle = "Fitted-True Distance, mm";
  htemp15->GetXaxis()->SetTitle(xtitle.c_str());
  htemp15->GetYaxis()->SetTitle(ytitle.c_str());
  htemp15->SetTitle("");
  htemp15->GetYaxis()->SetTitleOffset(0.9);
  htemp15->GetXaxis()->SetTitleSize(0.05);
  htemp15->GetYaxis()->SetTitleSize(0.05);
  gPad->Update();
  c15->SetLogz();
  c15->SaveAs((plotprefix+"itrD.pdf").c_str());  

  TCanvas* c16 = new TCanvas("c16", "c16", 1500,800);
  c16->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *htemp16 = new TH2F("htemp16", "htemp16", 100, 0, 14000, 100, 0, 5000);
  fTree->Draw("nhitsCleaned : sqrt( mcPosx*mcPosx + mcPosy*mcPosy + mcPosz*mcPosz ) >> htemp16", (cut1).c_str(), "colz");
  ytitle = "Nhits";
  xtitle = "True Radius, mm";
  htemp16->GetXaxis()->SetTitle(xtitle.c_str());
  htemp16->GetYaxis()->SetTitle(ytitle.c_str());
  htemp16->SetTitle("");
  htemp16->GetYaxis()->SetTitleOffset(0.9);
  htemp16->GetXaxis()->SetTitleSize(0.05);
  htemp16->GetYaxis()->SetTitleSize(0.05);
  htemp16->SetLineColor(kBlue);
  htemp16->SetLineWidth(2);
  gPad->Update();
  c16->SaveAs((plotprefix+"nhitsR.pdf").c_str());

  TCanvas* c17 = new TCanvas("c17", "c17", 1500,800);
  c17->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH1F *htemp17 = new TH1F("htemp17", "htemp17", 100, 0, 2);
  fTree->Draw("energy >> htemp17", ("!("+cut1+cut2+") && sqrt( mcPosx*mcPosx + mcPosy*mcPosy + mcPosz*mcPosz ) > 7000").c_str(), "colz");
  ytitle = "Incorrectly Tagged High Neck Events";
  xtitle = "Reconstructed Energy, MeV";
  htemp17->GetXaxis()->SetTitle(xtitle.c_str());
  htemp17->GetYaxis()->SetTitle(ytitle.c_str());
  htemp17->SetTitle("");
  htemp17->GetYaxis()->SetTitleOffset(0.9);
  htemp17->GetXaxis()->SetTitleSize(0.05);
  htemp17->GetYaxis()->SetTitleSize(0.05);
  htemp17->SetLineColor(kBlue);
  htemp17->SetLineWidth(2);
  gPad->Update();
  c17->SaveAs((plotprefix+"cutEnergy.pdf").c_str());

  TCanvas* c18 = new TCanvas("c18", "c18", 1500,800);
  c18->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH1F *htemp18 = new TH1F("htemp18", "htemp18", 100, 0, 1000);
  fTree->Draw("nhitsCleaned >> htemp18", ("!("+cut1+cut2+") && sqrt( mcPosx*mcPosx + mcPosy*mcPosy + mcPosz*mcPosz ) < 6000").c_str(), "colz");
  ytitle = "Incorrectly Tagged High Neck Events";
  xtitle = "Nhits";
  htemp18->GetXaxis()->SetTitle(xtitle.c_str());
  htemp18->GetYaxis()->SetTitle(ytitle.c_str());
  htemp18->SetTitle("");
  htemp18->GetYaxis()->SetTitleOffset(0.9);
  htemp18->GetXaxis()->SetTitleSize(0.05);
  htemp18->GetYaxis()->SetTitleSize(0.05);
  htemp18->SetLineColor(kBlue);
  htemp18->SetLineWidth(2);
  gPad->Update();
  c18->SaveAs((plotprefix+"cutNhits.pdf").c_str());

  TCanvas* c19 = new TCanvas("c19", "c19", 1500, 800);
  c19->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH1F *htemp19 = new TH1F("htemp19", "htemp19", 100, 0, 20000);
  fTree->Draw("sqrt( (posx-mcPosx)*(posx-mcPosx) + (posy-mcPosy)*(posy-mcPosy) + (posz-mcPosz)*(posz-mcPosz) ) >> htemp19", ("!("+cut1+cut2+") && sqrt( mcPosx*mcPosx + mcPosy*mcPosy + mcPosz*mcPosz ) < 6000").c_str(), "colz");
  ytitle = "Incorrectly Tagged High Neck Events";
  xtitle = "Fit - Truth Distance, mm";
  htemp19->GetXaxis()->SetTitle(xtitle.c_str());
  htemp19->GetYaxis()->SetTitle(ytitle.c_str());
  htemp19->SetTitle("");
  htemp19->GetYaxis()->SetTitleOffset(0.9);
  htemp19->GetXaxis()->SetTitleSize(0.05);
  htemp19->GetYaxis()->SetTitleSize(0.05);
  htemp19->SetLineColor(kBlue);
  htemp19->SetLineWidth(2);
  gPad->Update();
  c19->SaveAs((plotprefix+"cutFitDist.pdf").c_str());

  TCanvas* c20 = new TCanvas("c20", "c20", 1500, 800);
  c20->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH1F *htemp20 = new TH1F("htemp20", "htemp20", 100, 0, 2000);
  fTree->Draw("sqrt( (posx-mcPosx)*(posx-mcPosx) + (posy-mcPosy)*(posy-mcPosy) + (posz-mcPosz)*(posz-mcPosz) ) >> htemp20", (cut1.c_str()), "colz");
  ytitle = "Events";
  xtitle = "Fit - Truth Distance, mm";
  htemp20->GetXaxis()->SetTitle(xtitle.c_str());
  htemp20->GetYaxis()->SetTitle(ytitle.c_str());
  htemp20->SetTitle("");
  htemp20->GetYaxis()->SetTitleOffset(0.9);
  htemp20->GetXaxis()->SetTitleSize(0.05);
  htemp20->GetYaxis()->SetTitleSize(0.05);
  htemp20->SetLineColor(kBlue);
  htemp20->SetLineWidth(2);
  gPad->Update();
  c20->SaveAs((plotprefix+"allFitDist.pdf").c_str());
  
}

