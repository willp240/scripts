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


void CompareBias() {

  gStyle->SetPalette(51);

  std::string axsy = "Bias";
  std::string axsx = "True";

  std::string coord[3] = {"x", "y", "z"};

  TFile *_pdffile = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Apr8_2.5MeV_AllR_Tree.root");
  TTree* pdfTree = (TTree*)_pdffile->Get("eveTree")->Clone("pdftree");

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->Divide(3,3);

  std::string cut = "";//"zFit<7000 && zFit>0 && xFit<7000 && xFit>-7000 && yFit<7000 && yFit>-7000";

  for(int i=0; i<3; i++){

    for(int j=0; j<3; j++){

      c1->cd(j+1 + 3*i);

      gPad->SetRightMargin(0.14);
      std::string histname = coord[j] + axsy + ":" + coord[i] + axsx;
      std::cout << histname << std::endl;
      pdfTree->Draw(histname.c_str(),cut.c_str(),"colz");

      TH2F *htemp = (TH2F*)gPad->GetPrimitive("htemp");
      std::string xtitle = "True " + coord[i] + ", mm";
      std::string ytitle = "Fitted - True " + coord[j] + ", mm";
      htemp->GetXaxis()->SetTitle(xtitle.c_str());
      htemp->GetYaxis()->SetTitle(ytitle.c_str());
      htemp->GetXaxis()->SetRangeUser(-6000,6000);
      htemp->GetYaxis()->SetRangeUser(-800,800);
      htemp->GetYaxis()->SetTitleOffset(1.3);
      gPad->Update();
    }
    
  }
}
