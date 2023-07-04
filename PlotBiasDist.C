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


void PlotBiasDist(){

  gStyle->SetPalette(51);
  gStyle->SetLineWidth(2);
  
  std::string axs = "sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue) ) : sqrt(xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) ";
  std::string title = "Reconstructed";

  TFile *fFile = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Apr9_2.5MeV_AllR_NearAV_Tree.root");
  TTree* fTree = (TTree*)fFile->Get("eveTree")->Clone("fTree");

  TCanvas* c1 = new TCanvas("c1", "c1", 1000,600);
  c1->SetRightMargin(0.13);
  //c1->Divide(2,2);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);  

  fTree->Draw(axs.c_str(),"nearAV > 0.28 && neckHit < 2 && scaledLLH > 14.2","colz");
  TH2F *htemp = (TH2F*)gPad->GetPrimitive("htemp");
  std::string xtitle = "True Radius, mm";
  std::string ytitle = "Fitted - True Distance, mm";
  htemp->GetXaxis()->SetTitle(xtitle.c_str());
  htemp->GetYaxis()->SetTitle(ytitle.c_str());
  htemp->SetTitle("");
  htemp->GetYaxis()->SetTitleOffset(1.3);
  gPad->Update();
  
}

