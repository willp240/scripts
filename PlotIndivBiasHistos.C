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

  std::string coord[4] = {"x", "y", "z", "r"};
  
  std::string fname = "/home/parkerw/Software/rat-tools_master/FitPerformance/Jul21_recoordMPDF_2p2gl_perf_1to10MeVpoint_E_gaus_noautoview";

  TFile *file1 = TFile::Open( (fname+".root").c_str() );

  for(int i=0; i<4; i++){
    TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
    c1->Divide(5,4);    
    c1->Print( (fname+"_"+coord[i]+".pdf[").c_str() );
    for(int j=0; j<20; j++){
      c1->cd(j+1);    
      TString hname = Form("%s_%d", coord[i].c_str(), j);
      TH1D* histo = file1->Get(hname);
      gPad->SetLogy();
      histo->Draw();
    }
  c1->Print( (fname+"_"+coord[i]+".pdf").c_str() );
  c1->Print( (fname+"_"+coord[i]+".pdf]").c_str() );
  }
}
