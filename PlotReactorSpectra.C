#include <RAT/DU/DSReader.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/DU/LightPathCalculator.hh>
#include <RAT/DU/GroupVelocity.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/MC.hh>
#include <RAT/DS/EV.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/FitResult.hh>

#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"

#include <string>

void PlotReactorSpectra(){

  gStyle->SetOptStat(0);

  TFile *_file1 = TFile::Open("/data/snoplus3/griddata/Prod_7_0_8_9_Preliminary_Scintillator_Gold/ntuples/scaled_histogram_ScintFit_2p2ReactoribdAll.ntuple.root");

  TH1D* hraw = (TH1D*)_file1->Get("raw")->Clone();
  TH1D* hscaled = (TH1D*)_file1->Get("scaled")->Clone();
  TH1D* hoscillated = (TH1D*)_file1->Get("oscillated")->Clone();

  hraw->SetLineColor(kRed);
  hscaled->SetLineColor(kBlack);
  hoscillated->SetLineColor(kBlue);

  hraw->SetLineStyle(1);
  hscaled->SetLineStyle(1);
  hoscillated->SetLineStyle(1);

  hraw->SetLineWidth(2);
  hscaled->SetLineWidth(2);
  hoscillated->SetLineWidth(2);

  TLegend* t1 = new TLegend( 0.7, 0.7, 0.88, 0.85 );
  t1->AddEntry( hraw, "Raw", "l" );
  t1->AddEntry( hscaled, "Scaled", "l" );
  t1->AddEntry( hoscillated, "Oscillated", "l" );

  hraw->GetXaxis()->SetRangeUser(1,10);
  hraw->GetXaxis()->SetTitle("Prompt Energy, MeV");
  hraw->GetYaxis()->SetTitle("Expected Counts / 0.1 MeV");

  hraw->Draw();
  hscaled->Draw("same");
  hoscillated->Draw("same");
  t1->Draw("same");

  gPad->SetFrameLineWidth(2);
  t1->SetLineWidth(2);
  gPad->SetGrid(1);
  gPad->Update();

}

