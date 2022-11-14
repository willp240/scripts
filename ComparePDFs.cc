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

  TFile *_file1 = TFile::Open("MultiPDF_Plots_Oct17_Round3.root");
  TFile *_file2 = TFile::Open("MultiPDF_Plots_Nov2_Round1.root");

  TH1D* h1 = (TH1D*)_file1->Get("PDF_0")->Clone();
  TH1D* h2 = (TH1D*)_file2->Get("PDF_0")->Clone();

  h1->SetLineColor(kRed);
  h2->SetLineColor(kBlue);

  h1->SetLineStyle(1);
  h2->SetLineStyle(1);

  h1->SetLineWidth(2);
  h2->SetLineWidth(2);

  h1->Scale(1/h1->Integral());
  h2->Scale(1/h2->Integral());

  TLegend* t1 = new TLegend( 0.5, 0.6, 0.9, 0.9 );
  t1->AddEntry( h1, "Original", "l" );
  t1->AddEntry( h2, "Round 1", "l" );

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

  TFile *_file1 = TFile::Open("MultiPDF_Plots_Oct17_Round3.root");
  TFile *_file2 = TFile::Open("MultiPDF_Plots_Nov2_Round1.root");
  TFile *_file3 = TFile::Open("MultiPDF_Plots_Nov2_Round2.root");

  TH1D* h1 = (TH1D*)_file1->Get("PDF_0")->Clone();
  TH1D* h2 = (TH1D*)_file2->Get("PDF_0")->Clone();
  TH1D* h3 = (TH1D*)_file3->Get("PDF_0")->Clone();

  h1->SetLineColor(kRed);
  h2->SetLineColor(kBlue);
  h3->SetLineColor(kBlack);

  h1->SetLineWidth(2);
  h2->SetLineWidth(2);
  h3->SetLineWidth(2);

  h1->SetLineStyle(2);
  h2->SetLineStyle(2);
  h3->SetLineStyle(2);

  h1->Scale(1/h1->Integral());
  h2->Scale(1/h2->Integral());
  h3->Scale(1/h3->Integral());

  TLegend* t1 = new TLegend( 0.5, 0.6, 0.9, 0.9 );
  t1->AddEntry( h1, "Original", "l" );
  t1->AddEntry( h2, "Round 1", "l" );
  t1->AddEntry( h3, "Round 2", "l" );

  h3->GetXaxis()->SetRangeUser(-5,100);
  h3->GetXaxis()->SetTitle("Hit time residuals [ns]");
  h3->GetYaxis()->SetTitle("Normalised Counts");
  h1->GetYaxis()->SetTitleOffset(1.2);

  h3->Draw();
  h2->Draw("same");
  h1->Draw("same");
  t1->Draw("same");

}

void Compare4PDFs(){

  gStyle->SetOptStat(0);

  TFile *_file0 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitCoordination/MultiPDFFull/MultiPDF_Plots_Oct17_Round3.root");
  TFile *_file1 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitCoordination/MultiPDFFull/MultiPDF_Plots_Nov2_Round1.root");
  TFile *_file2 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitCoordination/MultiPDFFull/MultiPDF_Plots_Nov2_Round2.root");
  TFile *_file3 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitCoordination/MultiPDFFull/MultiPDF_Plots_Nov4_Round3.root");

  TH1D* h0 = (TH1D*)_file0->Get("PDF_0")->Clone("h0");
  TH1D* h1 = (TH1D*)_file1->Get("PDF_0")->Clone("h1");
  TH1D* h2 = (TH1D*)_file2->Get("PDF_0")->Clone("h2");
  TH1D* h3 = (TH1D*)_file3->Get("PDF_0")->Clone("h3");

  h0->SetLineColor(kRed);
  h1->SetLineColor(kBlue);
  h2->SetLineColor(kBlack);
  h3->SetLineColor(kGreen);

  h0->SetLineStyle(2);
  h1->SetLineStyle(2);
  h2->SetLineStyle(2);
  h3->SetLineStyle(2);

  h0->SetLineWidth(2);
  h1->SetLineWidth(2);
  h2->SetLineWidth(2);
  h3->SetLineWidth(2);

  h0->Scale(1/h0->Integral());
  h1->Scale(1/h1->Integral());
  h2->Scale(1/h2->Integral());
  h3->Scale(1/h3->Integral());

  TLegend* t1 = new TLegend( 0.5, 0.6, 0.9, 0.9 );
  t1->AddEntry( h0, "Original", "l" );
  t1->AddEntry( h1, "Round 1", "l" );
  t1->AddEntry( h2, "Round 2", "l" );
  t1->AddEntry( h3, "Round 3", "l" );

  h0->GetXaxis()->SetRangeUser(-5,100);
  h0->GetXaxis()->SetTitle("Hit time residuals [ns]");
  h0->GetYaxis()->SetTitle("Normalised Counts");
  h0->GetYaxis()->SetTitleOffset(1.3);

  h0->Draw();
  h2->Draw("same");
  h3->Draw("same");
  h1->Draw("same");
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

void Compare6PDFs(){

  TFile *_file1 = TFile::Open("May10_RHist.root");
  int col[6] = {1,4,7,3,5,6}; 
  for(int i=0; i<6; i++){

    TString hname = Form("PDF_%d",i);
    TH1D* h1 = (TH1D*)_file1->Get(hname)->Clone();
 
    h1->SetLineColor(col[i]);

    h1->Scale(1/h1->Integral());

    //    h1->GetXaxis()->SetRangeUser(-5,100);
    h1->GetXaxis()->SetTitle("Hit time residuals [ns]");
    h1->GetYaxis()->SetTitle("Normalised Counts");
    h1->GetYaxis()->SetTitleOffset(1.2);

    h1->Draw("same");
  }
}

void Compare2RPDFs(){

  TFile *_file1 = TFile::Open("May10_RHist0.5.root");
  int col[6] = {2,4};
  TLegend* t1 = new TLegend( 0.5, 0.6, 0.9, 0.9 );
  const char* hnom[2] = {"R<0.5m","R>0.5m"};
  //  hnom[0] = "R<3m";
  //hnom[1] = "R>3m";
  for(int i=1; i>-1; i--){

    TString hname = Form("PDF_%d",i);
    TH1D* h1 = (TH1D*)_file1->Get(hname)->Clone();

    h1->SetLineColor(col[i]);

    h1->Scale(1/h1->Integral());

    //    h1->GetXaxis()->SetRangeUser(-5,100);
    h1->GetXaxis()->SetTitle("Hit time residuals [ns]");
    h1->GetYaxis()->SetTitle("Normalised Counts");
    h1->GetYaxis()->SetTitleOffset(1.2);

    t1->AddEntry( h1, hnom[i], "l" );

    h1->Draw("same");
  }
  t1->Draw();
}

void Compare4PMTPDFs(){

  TFile *_file1 = TFile::Open("May10_PMTHist4.root");
  int col[8] = {1,4,7,3,5,6,2,28};

  for(int i=0; i<4; i++){

    TString hname = Form("PDF_%d",i);
    TH1D* h1 = (TH1D*)_file1->Get(hname)->Clone();

    h1->SetLineColor(col[i]);

    h1->Scale(1/h1->Integral());

    //    h1->GetXaxis()->SetRangeUser(-5,100);
    h1->GetXaxis()->SetTitle("Hit time residuals [ns]");
    h1->GetYaxis()->SetTitle("Normalised Counts");
    h1->GetYaxis()->SetTitleOffset(1.2);

    h1->Draw("same");
  }
}

void Compare8PMTPDFs(){

  TFile *_file1 = TFile::Open("May10_PMTHist8.root");
  int col[8] = {1,4,7,3,5,6,2,28};
  TLegend* t1 = new TLegend( 0.5, 0.6, 0.9, 0.9 );
 
  for(int i=0; i<8; i++){

    TString hname = Form("PDF_%d",i);
    TString hnom = Form("Octant %d",i);
    TH1D* h1 = (TH1D*)_file1->Get(hname)->Clone();

    h1->SetLineColor(col[i]);

    h1->Scale(1/h1->Integral());

    //    h1->GetXaxis()->SetRangeUser(-5,100);
    h1->GetXaxis()->SetTitle("Hit time residuals [ns]");
    h1->GetYaxis()->SetTitle("Normalised Counts");
    h1->GetYaxis()->SetTitleOffset(1.2);
    t1->AddEntry( h1, hnom, "l" );
    h1->Draw("same");
  }
  t1->Draw();
}

void Compare2DistPDFs(){

  TFile *_file1 = TFile::Open("May24_2PDF_DistHist.root");

  TH1D* h1 = (TH1D*)_file1->Get("PDF_0")->Clone();
  TH1D* h2 = (TH1D*)_file1->Get("PDF_1")->Clone();

  h1->SetLineColor(kRed);
  h2->SetLineColor(kBlue);

  h1->Scale(1/h1->Integral());
  h2->Scale(1/h2->Integral());

  TLegend* t1 = new TLegend( 0.5, 0.6, 0.9, 0.9 );
  t1->AddEntry( h1, "Distance < 8m", "l" );
  t1->AddEntry( h2, "Distance > 8m", "l" );

  h2->GetXaxis()->SetRangeUser(-5,100);
  h2->GetXaxis()->SetTitle("Hit time residuals [ns]");
  h2->GetYaxis()->SetTitle("Normalised Counts");
  h2->GetYaxis()->SetTitleOffset(1.2);

  h2->Draw();
  h1->Draw("same");
  t1->Draw("same");

}
