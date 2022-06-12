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


void PlotIndivBiasHistos() {

  std::string coord = "x";

  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Mar31_1.1ppo_1to10MeV_0to5500_mean_E_noAutoview.root");

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->Divide(5,4);
  c1->Print("./Mar31_1.1ppo_1to10MeV_0to5500_mean_E_noAutoview_x.pdf[");
  for(int i=0; i<20; i++){

    c1->cd(i+1);

    TString hname = Form("x_%d",i);
    std::cout << hname << std::endl;
    TH1D* histo = file1->Get(hname);
    gPad->SetLogy();
    histo->Draw();
  }
  c1->Print("./Mar31_1.1ppo_1to10MeV_0to5500_mean_E_noAutoview_x.pdf");
  c1->Print("./Mar31_1.1ppo_1to10MeV_0to5500_mean_E_noAutoview_x.pdf]");
}
