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

void Compare2PDFs(){

  TFile *_file1 = TFile::Open("Dec10FVCut_1PDFBand0.root");
  TFile *_file2 = TFile::Open("Dec10NoFVCut_1PDFBand0.root");

  TH1D* h1 = (TH1D*)_file1->Get("PDF")->Clone();
  TH1D* h2 = (TH1D*)_file2->Get("PDF")->Clone();

  h1->SetLineColor(kRed);
  h2->SetLineColor(kBlue);

  h1->Scale(1/h1->Integral());
  h2->Scale(1/h2->Integral());

  TLegend* t1 = new TLegend( 0.5, 0.6, 0.9, 0.9 );
  t1->AddEntry( h1, "FV Cut", "l" );
  t1->AddEntry( h2, "No FV Cut", "l" );

  h2->GetXaxis()->SetRangeUser(-5,100);
  h2->GetXaxis()->SetTitle("Hit time residuals [ns]");
  h2->GetYaxis()->SetTitle("Normalised Counts");
  h2->GetYaxis()->SetTitleOffset(1.2);

  h2->Draw();
  h1->Draw("same");
  t1->Draw("same");

}

void Compare3PDFs(){

  gStyle->SetOptStat(0);

  TFile *_fileAll = TFile::Open("ET1D_PMTZAll_Rlt1.5.root");
  TFile *_fileAbove = TFile::Open("ET1D_PMTZAbove0_Rlt1.5.root");
  TFile *_fileBelow = TFile::Open("ET1D_PMTZBelow0_Rlt1.5.root");

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

  TFile *_fileBelow = TFile::Open("/data/snoplus/parkerw/pdfs/recoord_Nov2020/Recoord_8PDFBand4.root");
  TFile *_fileMidBelow = TFile::Open("/data/snoplus/parkerw/pdfs/recoord_Nov2020/Recoord_8PDFBand5.root");
  TFile *_fileMidAbove = TFile::Open("/data/snoplus/parkerw/pdfs/recoord_Nov2020/Recoord_8PDFBand6.root");
  TFile *_fileAbove = TFile::Open("/data/snoplus/parkerw/pdfs/recoord_Nov2020/Recoord_8PDFBand7.root");

  TH1D* hBelow = (TH1D*)_fileBelow->Get("PDF")->Clone();
  TH1D* hMidBelow = (TH1D*)_fileMidBelow->Get("PDF")->Clone();
  TH1D* hMidAbove = (TH1D*)_fileMidAbove->Get("PDF")->Clone();
  TH1D* hAbove = (TH1D*)_fileAbove->Get("PDF")->Clone();

  hBelow->SetLineColor(kRed);
  hMidBelow->SetLineColor(kBlack);
  hMidAbove->SetLineColor(kBlue);
  hAbove->SetLineColor(kGreen);

  hBelow->Scale(1/hBelow->Integral());
  hMidBelow->Scale(1/hMidBelow->Integral());
  hMidAbove->Scale(1/hMidAbove->Integral());
  hAbove->Scale(1/hAbove->Integral());

  TLegend* t1 = new TLegend( 0.5, 0.6, 0.9, 0.9 );
  t1->AddEntry( hBelow, "PMT Z < -1.5m", "l" );
  t1->AddEntry( hMidBelow, "-1.5m < PMT Z < 0m", "l" );
  t1->AddEntry( hMidAbove, "0m < PMT Z < 1.5m", "l" );
  t1->AddEntry( hAbove, "PMT Z > 1.5m", "l" );

  hMidAbove->GetXaxis()->SetRangeUser(-5,100);
  hMidAbove->GetXaxis()->SetTitle("Hit time residuals [ns]");
  hMidAbove->GetYaxis()->SetTitle("Normalised Counts");
  hMidAbove->GetYaxis()->SetTitleOffset(1.2);

  hMidAbove->Draw();
  hAbove->Draw("same");
  hMidBelow->Draw("same");
  hBelow->Draw("same");
  t1->Draw("same");

}

void Compare8RefactPDFs(){

  gStyle->SetOptStat(0);

  TFile *_fileBelow2 = TFile::Open("Recoord_4PDFBand3.root");
  TFile *_fileMidBelow2 = TFile::Open("Recoord_4PDFBand2.root");
  TFile *_fileMidAbove2 = TFile::Open("Recoord_4PDFBand1.root");
  TFile *_fileAbove2 = TFile::Open("Recoord_4PDFBand0.root");

  TFile *_fileBelow = TFile::Open("FixConc4PDFBand3.root");
  TFile *_fileMidBelow = TFile::Open("FixConc4PDFBand2.root");
  TFile *_fileMidAbove = TFile::Open("FixConc4PDFBand1.root");
  TFile *_fileAbove = TFile::Open("FixConc4PDFBand0.root");

  TH1D* hBelow = (TH1D*)_fileBelow->Get("PDF")->Clone();
  TH1D* hMidBelow = (TH1D*)_fileMidBelow->Get("PDF")->Clone();
  TH1D* hMidAbove = (TH1D*)_fileMidAbove->Get("PDF")->Clone();
  TH1D* hAbove = (TH1D*)_fileAbove->Get("PDF")->Clone();

  TH1D* hBelow2 = (TH1D*)_fileBelow2->Get("PDF")->Clone();
  TH1D* hMidBelow2 = (TH1D*)_fileMidBelow2->Get("PDF")->Clone();
  TH1D* hMidAbove2 = (TH1D*)_fileMidAbove2->Get("PDF")->Clone();
  TH1D* hAbove2 = (TH1D*)_fileAbove2->Get("PDF")->Clone();

  hBelow->SetLineColor(kRed);
  hMidBelow->SetLineColor(kBlack);
  hMidAbove->SetLineColor(kBlue);
  hAbove->SetLineColor(kGreen);

  hBelow2->SetLineColor(kRed);
  hMidBelow2->SetLineColor(kBlack);
  hMidAbove2->SetLineColor(kBlue);
  hAbove2->SetLineColor(kGreen);

  hBelow2->SetLineStyle(2);
  hMidBelow2->SetLineStyle(2);
  hMidAbove2->SetLineStyle(2);
  hAbove2->SetLineStyle(2);

  hBelow->Scale(1/hBelow->Integral());
  hMidBelow->Scale(1/hMidBelow->Integral());
  hMidAbove->Scale(1/hMidAbove->Integral());
  hAbove->Scale(1/hAbove->Integral());

  hBelow2->Scale(1/hBelow2->Integral());
  hMidBelow2->Scale(1/hMidBelow2->Integral());
  hMidAbove2->Scale(1/hMidAbove2->Integral());
  hAbove2->Scale(1/hAbove2->Integral());

  TLegend* t1 = new TLegend( 0.5, 0.6, 0.9, 0.9 );
  t1->AddEntry( hBelow, "PMT Z < -1.5m", "l" );
  t1->AddEntry( hMidBelow, "-1.5m < PMT Z < 0m", "l" );
  t1->AddEntry( hMidAbove, "0m < PMT Z < 1.5m", "l" );
  t1->AddEntry( hAbove, "PMT Z > 1.5m", "l" );

  hMidAbove->GetXaxis()->SetRangeUser(-5,100);
  hMidAbove->GetXaxis()->SetTitle("Hit time residuals [ns]");
  hMidAbove->GetYaxis()->SetTitle("Normalised Counts");
  hMidAbove->GetYaxis()->SetTitleOffset(1.2);

  hMidAbove->Draw();
  hAbove->Draw("same");
  hMidBelow->Draw("same");
  hBelow->Draw("same");

  hMidAbove2->Draw("same");
  hAbove2->Draw("same");
  hMidBelow2->Draw("same");
  hBelow2->Draw("same");

  t1->Draw("same");

}
