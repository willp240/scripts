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
  
  std::string axs[4] = {"xFit:xTrue", "yFit:yTrue", "zFit:zTrue", "sqrt(xFit*xFit+yFit*yFit+zFit*zFit):sqrt(xTrue*xTrue+yTrue*yTrue+zTrue*zTrue)"};
  std::string title = "Reconstructed";

  std::string coord[4] = {"X", "Y", "Z", "R"};

  TFile *fFile = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/May19_Beta14_3MeV_Tree.root");
  TTree* fTree = (TTree*)fFile->Get("eveTree")->Clone("fTree");

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->SetRightMargin(0.13);
  c1->Divide(2,2);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);  

  //std::string cut = "sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) < 6000 && nearAV > 0.28";//"zFit<7000 && zFit>0 && xFit<7000 && xFit>-7000 && yFit<7000 && yFit>-7000";
  //std::string cut = "neckHit<2 && scaledLLH  > 14.2 && nearAV > 0.28";

  std::string cut ="beta14 > -0.05 && scaledLLH  > 13.5 && zFit < 40000";// "itr > 0.18 && scaledLLH  > 14.2 && nearAV > 0.28";

  for(int i=0; i<4; i++){
    c1->cd(i+1);
    gPad->SetFrameLineWidth(2);
    gPad->SetGrid(1);
    //gPad->SetRightMargin(0.14);
    fTree->Draw(axs[i].c_str(),cut.c_str(),"colz");
    TH2F *htemp = (TH2F*)gPad->GetPrimitive("htemp");
    std::string xtitle = "True " + coord[i] + ", mm";
    std::string ytitle = "Fitted " + coord[i] + ", mm";
    htemp->GetXaxis()->SetTitle(xtitle.c_str());
    htemp->GetYaxis()->SetTitle(ytitle.c_str());
    htemp->SetTitle("");
    htemp->GetYaxis()->SetTitleOffset(1.3);
    gPad->Update();
    
  }

}

