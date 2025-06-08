#include <TH1D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <TVector3.h>
#include <TStyle.h>

void PlotTimeDists() {

  gStyle->SetOptStat(0);
  
  TString fnameUpdated = "/home/parkerw/Scripts/May23_SFTime_updatedhists.root";
  TString fnameOG = "/home/parkerw/Scripts/May23_SFTime_oghists.root";

  TFile *fUpdated = new TFile(fnameUpdated, "OPEN");
  TFile *fOG = new TFile(fnameOG, "OPEN");
  
  TString hname1 = "Total";
  TString hname2 = "Quad_Seed";
  TString hname3 = "MultiPDF";

  TH1D* hTotalUpdated = (TH1D*)fUpdated->Get(hname1);
  hTotalUpdated->SetName("totalUpdated");
  hTotalUpdated->SetTitle("Total ScintFitter Time");
  
  TH1D* hTotalOG = (TH1D*)fOG->Get(hname1);
  hTotalOG->SetName("totalOG");

  hTotalUpdated->SetLineColor(kBlack);
  hTotalOG->SetLineColor(kBlue);
  hTotalUpdated->SetLineWidth(2);
  hTotalOG->SetLineWidth(2);

  hTotalUpdated->GetXaxis()->SetTitle("Time, s");
  hTotalUpdated->GetYaxis()->SetRangeUser(0,2000000);
  hTotalUpdated->GetYaxis()->SetTitle("Events");

  TLegend* t1 = new TLegend( 0.6, 0.7, 0.85, 0.8 );
  t1->SetLineWidth(2);
  t1->AddEntry( hTotalOG, "RAT 8.0.1", "l" );
  t1->AddEntry( hTotalUpdated, "Proposed Updates", "l" );
  
  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  
  hTotalUpdated->Draw();
  hTotalOG->Draw("same");
  t1->Draw();

  c1->SaveAs("totalTimes.pdf");

  TH1D* hQuadUpdated = (TH1D*)fUpdated->Get(hname2);
  hQuadUpdated->SetName("quadUpdated");
  hQuadUpdated->SetTitle("Quad Seed Time");

  TH1D* hQuadOG = (TH1D*)fOG->Get(hname2);
  hQuadOG->SetName("quadOG");

  hQuadUpdated->SetLineColor(kBlack);
  hQuadOG->SetLineColor(kBlue);
  hQuadUpdated->SetLineWidth(2);
  hQuadOG->SetLineWidth(2);

  hQuadUpdated->GetXaxis()->SetTitle("Time, s");
  hQuadUpdated->GetYaxis()->SetRangeUser(0,18000000);
  hQuadUpdated->GetYaxis()->SetTitle("Events");

  TLegend* t2 = new TLegend( 0.6, 0.7, 0.85, 0.85 );
  t2->SetLineWidth(2);
  t2->AddEntry( hTotalOG, "RAT 8.0.1", "l" );
  t2->AddEntry( hTotalUpdated, "Proposed Updates", "l" );

  TCanvas* c2 = new TCanvas("c2", "c2", 1500,800);
  c2->cd();
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  hQuadUpdated->Draw();
  hQuadOG->Draw("same");
  t2->Draw();

  c2->SaveAs("quadTimes.pdf");

  TH1D* hMultiPDFUpdated = (TH1D*)fUpdated->Get(hname3);
  hMultiPDFUpdated->SetName("multipdfUpdated");
  hMultiPDFUpdated->SetTitle("MultiPDF Time");

  TH1D* hMultiPDFOG = (TH1D*)fOG->Get(hname3);
  hMultiPDFOG->SetName("multipdfOG");

  hMultiPDFUpdated->SetLineColor(kBlack);
  hMultiPDFOG->SetLineColor(kBlue);
  hMultiPDFUpdated->SetLineWidth(2);
  hMultiPDFOG->SetLineWidth(2);

  hMultiPDFUpdated->GetXaxis()->SetTitle("Time, s");
  hMultiPDFUpdated->GetYaxis()->SetRangeUser(0,5000000);
  hMultiPDFUpdated->GetYaxis()->SetTitle("Events");

  TLegend* t3 = new TLegend( 0.6, 0.7, 0.85, 0.85 );
  t3->SetLineWidth(2);
  t3->AddEntry( hTotalOG, "RAT 8.0.1", "l" );
  t3->AddEntry( hTotalUpdated, "Proposed Updates", "l" );

  TCanvas* c3 = new TCanvas("c3", "c3", 1500,800);
  c3->cd();
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  hMultiPDFUpdated->Draw();
  hMultiPDFOG->Draw("same");
  t3->Draw();

  c3->SaveAs("multipdfTimes.pdf");
  
}

