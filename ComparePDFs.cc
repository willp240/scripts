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


void Compare3PDFs(){

  gStyle->SetOptStat(0);

  TFile *_fileAll = TFile::Open("ET1D_PMTZAll_PDF_10000.root");
  TFile *_fileAbove = TFile::Open("ET1D_PMTZAbove0_PDF.root");
  TFile *_fileBelow = TFile::Open("ET1D_PMTZBelow0_PDF.root");

  TH1D* hAll = (TH1D*)_fileAll->Get("PDF")->Clone();
  TH1D* hAbove = (TH1D*)_fileAbove->Get("PDF")->Clone();
  TH1D* hBelow = (TH1D*)_fileBelow->Get("PDF")->Clone();

  hAll->SetLineColor(kRed);
  hAbove->SetLineColor(kBlack);
  hBelow->SetLineColor(kBlue);

  hAll->Scale(1/hAll->Integral());
  hAbove->Scale(1/hAbove->Integral());
  hBelow->Scale(1/hBelow->Integral());

  TLegend* t1 = new TLegend( 0.5, 0.6, 0.9, 0.9 );
  t1->AddEntry( hAll, "All PMTs", "l" );
  t1->AddEntry( hAbove, "PMT Z > 0", "l" );
  t1->AddEntry( hBelow, "PMT Z < 0", "l" );

  hAbove->GetXaxis()->SetRangeUser(-5,100);
  hAbove->GetXaxis()->SetTitle("Hit time residuals [ns]");
  hAbove->GetYaxis()->SetTitle("Normalised Counts");
  hAbove->GetYaxis()->SetTitleOffset(1.2);

  hAbove->Draw();
  hAll->Draw("same");
  hBelow->Draw("same");
  t1->Draw("same");

}

void Compare4PDFs(){

  gStyle->SetOptStat(0);

  TFile *_fileBelow = TFile::Open("ET1D_PMTZBelow-1.5_PDF.root");
  TFile *_fileMidBelow = TFile::Open("ET1D_PMTZ-1.5to0_PDF.root");
  TFile *_fileMidAbove = TFile::Open("ET1D_PMTZ0to1.5_PDF.root");
  TFile *_fileAbove = TFile::Open("ET1D_PMTZAbove1.5_PDF.root");

  TH1D* hBelow = (TH1D*)_fileBelow->Get("PDF")->Clone();
  TH1D* hMidBelow = (TH1D*)_fileMidBelow->Get("PDF")->Clone();
  TH1D* hMidAbove = (TH1D*)_fileMidAbove->Get("PDF")->Clone();
  TH1D* hAbove = (TH1D*)_fileAbove->Get("PDF")->Clone();

  hBelow->SetLineColor(kRed);
  hMidBelow->SetLineColor(kBlack);
  hMidAbove->SetLineColor(kBlue);
  hAbove->SetLineColor(kGreen);

  //  hBelow->Scale(1/hBelow->Integral());
  //hMidBelow->Scale(1/hMidBelow->Integral());
  //hMidAbove->Scale(1/hMidAbove->Integral());
  //hAbove->Scale(1/hAbove->Integral());

  TLegend* t1 = new TLegend( 0.5, 0.6, 0.9, 0.9 );
  t1->AddEntry( hBelow, "PMT Z < -1.5m", "l" );
  t1->AddEntry( hMidBelow, "-1.5m < PMT Z < 0m", "l" );
  t1->AddEntry( hMidAbove, "0m < PMT Z < 1.5m", "l" );
  t1->AddEntry( hAbove, "PMT Z > 1.5m", "l" );

  hMidAbove->GetXaxis()->SetRangeUser(-5,100);
  hMidAbove->GetXaxis()->SetTitle("Hit time residuals [ns]");
  hMidAbove->GetYaxis()->SetTitle("Normalised Counts");
  hMidAbove->GetYaxis()->SetTitleOffset(1.2);

  hAbove->Draw();
  hMidAbove->Draw("same");
  hMidBelow->Draw("same");
  hBelow->Draw("same");
  t1->Draw("same");

}
