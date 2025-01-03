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


void plot5m(){

  gStyle->SetPalette(51);
  gStyle->SetLineWidth(2);
  gStyle->SetOptStat(0);

  TFile *fFile = TFile::Open("/home/parkerw/Software/rat-tools_fresh/FitCoordination/NearAVAngular/Coordinate_DebuggingPlots_Done.root");

  TGraph* g = (TGraph*)fFile->Get("Graph");
  g->SetTitle("");
  g->GetYaxis()->SetRangeUser(0,0.5);
  g->SetMarkerStyle(2);
  g->SetMarkerSize(2);
  g->SetLineWidth(2);
  
  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  
  g->Draw("AP");
  c1->SaveAs("mmts_nhit.pdf");  
}

