#include <iostream>
#include <TLine.h>
#include <TStyle.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TAxis.h>


void dataTree2FitDiffs( ) {

  gStyle->SetOptStat(0);
  gStyle->SetPalette(51);
  
  TFile* fIn = new TFile("/data/snoplus3/parkerw/ratSimulations/Oct14_bMR_data_trees/Analysis20_bMR_.tree.root","READ");

  std::string outname = "Oct14_bMR";
  
  TTree* tree = (TTree*) fIn->Get("T");
    
  // Now make 1D plots: fitx1-fitx2, fity1-fity2, fitz1-fitz2, fitr1-fitr2, distance pos1 to pos2
  TCanvas* cX = new TCanvas("cX", "cX", 1500,800);
  cX->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hX = new TH1F("hX","hX", 1000, -6000, 6000);
  hX->SetTitle("");
  tree->Draw("fitx1-fitx2 >> hX", "", "");
  hX->SetLineWidth(2);
  hX->SetLineColor(kBlue+2);
  hX->GetXaxis()->SetTitle("Original Fit X - Updated Fit X, mm");
  hX->GetYaxis()->SetTitle("Events");
  hX->Draw();
  cX->SaveAs( (outname+"_X.pdf").c_str() );
  
  TCanvas* cY = new TCanvas("cY", "cY", 1500,800);
  cY->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hY = new TH1F("hY","hY", 1000, -6000, 6000);
  hY->SetTitle("");
  tree->Draw("fity1-fity2 >> hY", "", "");
  hY->SetLineWidth(2);
  hY->SetLineColor(kBlue+2);
  hY->GetXaxis()->SetTitle("Original Fit Y - Updated Fit Y, mm");
  hY->GetYaxis()->SetTitle("Events");
  hY->Draw();
  cY->SaveAs( (outname+"_Y.pdf").c_str() );
  
  TCanvas* cZ = new TCanvas("cZ", "cZ", 1500,800);
  cZ->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hZ = new TH1F("hZ","hZ", 1000, -6000, 6000);
  hZ->SetTitle("");
  tree->Draw("fitz1-fitz2 >> hZ", "", "");
  hZ->SetLineWidth(2);
  hZ->SetLineColor(kBlue+2);
  hZ->GetXaxis()->SetTitle("Original Fit Z - Updated Fit Z, mm");
  hZ->GetYaxis()->SetTitle("Events");
  hZ->Draw();
  cZ->SaveAs( (outname+"_Z.pdf").c_str() );
  
  TCanvas* cR = new TCanvas("cR", "cR", 1500,800);
  cR->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hR = new TH1F("hR","hR", 1000, -6000, 6000);
  hR->SetTitle("");
  tree->Draw("fitr1-fitr2 >> hR", "", "");
  hR->SetLineWidth(2);
  hR->SetLineColor(kBlue+2);
  hR->GetXaxis()->SetTitle("Original Fit R - Updated Fit R, mm");
  hR->GetYaxis()->SetTitle("Events");
  hR->Draw();
  cR->SaveAs( (outname+"_R.pdf").c_str() );
    
  TCanvas* cD = new TCanvas("cD", "cD", 1500,800);
  cD->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hD = new TH1F("hD","hD", 1000, 0, 12000);
  hD->SetTitle("");
  tree->Draw("sqrt( (fitx1-fitx2)*(fitx1-fitx2) + (fity1-fity2)*(fity1-fity2) + (fitz1-fitz2)*(fitz1-fitz2) ) >> hD", "", "");
  hD->SetLineWidth(2);
  hD->SetLineColor(kBlue+2);
  hD->GetXaxis()->SetTitle("Original - Updated Fit Distance, mm");
  hD->GetYaxis()->SetTitle("Events");
  hD->Draw();
  cD->SaveAs( (outname+"_D.pdf").c_str() );

  TCanvas* c2R = new TCanvas("c2R", "c2R", 1500,800);
  c2R->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hR1 = new TH1F("hR1","hR1", 1000, 0, 6000);
  hR1->SetTitle("");
  tree->Draw("fitr1 >> hR1", "", "");
  hR1->SetLineWidth(2);
  hR1->SetLineColor(kBlue+2);
  hR1->GetXaxis()->SetTitle("Fitted Radius, mm");
  hR1->GetYaxis()->SetTitle("Events");
  hR1->Draw();
  TH1F *hR2 = new TH1F("hR2","hR2", 1000, 0, 6000);
  hR2->SetTitle("");
  tree->Draw("fitr2 >> hR2", "", "same");
  hR2->SetLineWidth(2);
  hR2->SetLineStyle(2);
  hR2->SetLineColor(kRed+2);
  hR2->GetXaxis()->SetTitle("Fitted Radius, mm");
  hR2->GetYaxis()->SetTitle("Events");
  hR2->Draw("same");
  TLegend* t1 = new TLegend( 0.6, 0.4, 0.8, 0.6 );
  t1->AddEntry(hR1, "Original Fit", "l");
  t1->AddEntry(hR2, "Updated Fit", "l");
  t1->SetLineWidth(2);
  t1->Draw("same");
  c2R->SaveAs( (outname+"_2R.pdf").c_str() );


  // 2D plots
  
  TCanvas* cDR = new TCanvas("cDR", "cDR", 1500,800);
  cDR->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogz();
  TH2F *hDR = new TH2F("hDR","hDR", 100, 0, 6000, 100, 0, 12000);
  hDR->SetTitle("");
  tree->Draw("sqrt( (fitx1-fitx2)*(fitx1-fitx2) + (fity1-fity2)*(fity1-fity2) + (fitz1-fitz2)*(fitz1-fitz1)): fitr1 >> hDR", "", "");
  hDR->SetLineWidth(2);
  hDR->GetYaxis()->SetTitle("Original - Updated Fit Distance, mm");
  hDR->GetXaxis()->SetTitle("Original Fitted Radius, mm");
  hDR->Draw("colz");
  cDR->SaveAs( (outname+"_DR.pdf").c_str() );
  
  TCanvas* cXX = new TCanvas("cXX", "cXX", 1500,800);
  cXX->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogz();
  TH2F *hXX = new TH2F("hXX","hXX", 100, -6000, 6000, 100, -6000, 6000);
  hXX->SetTitle("");
  tree->Draw("fitx2:fitx1 >> hXX", "", "");
  hXX->SetLineWidth(2);
  hXX->GetXaxis()->SetTitle("Original Fit X, mm");
  hXX->GetYaxis()->SetTitle("Updated Fit X, mm");
  hXX->Draw("colz");
  cXX->SaveAs( (outname+"_XX.pdf").c_str() );
  
  TCanvas* cYY = new TCanvas("cYY", "cYY", 1500,800);
  cXX->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogz();
  TH2F *hYY = new TH2F("hYY","hYY", 100, -6000, 6000, 100, -6000, 6000);
  hYY->SetTitle("");
  tree->Draw("fity2:fity1 >> hYY", "", "");
  hYY->SetLineWidth(2);
  hYY->GetXaxis()->SetTitle("Original Fit Y, mm");
  hYY->GetYaxis()->SetTitle("Updated Fit Y, mm");
  hYY->Draw("colz");
  cYY->SaveAs( (outname+"_YY.pdf").c_str() );
  
  TCanvas* cZZ = new TCanvas("cZZ", "cZZ", 1500, 800);
  cZZ->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogz();
  TH2F *hZZ = new TH2F("hZZ","hZZ", 100, -6000, 6000, 100, -6000, 6000);
  hZZ->SetTitle("");
  tree->Draw("fitz2:fitz1 >> hZZ", "", "");
  hZZ->SetLineWidth(2);
  hZZ->GetXaxis()->SetTitle("Original Fit Z, mm");
  hZZ->GetYaxis()->SetTitle("Updated Fit Z, mm");
  hZZ->Draw("colz");
  cZZ->SaveAs( (outname+"_ZZ.pdf").c_str() );
  
  TCanvas* cRR = new TCanvas("cRR", "cRR", 1500, 800);
  cRR->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogz();
  TH2F *hRR = new TH2F("hRR","hRR", 100, 0, 6000, 100, 0, 6000);
  hRR->SetTitle("");
  tree->Draw("fitr2:fitr1 >> hRR", "", "");
  hRR->SetLineWidth(2);
  hRR->GetXaxis()->SetTitle("Original Fit R, mm");
  hRR->GetYaxis()->SetTitle("Updated Fit R, mm");
  hRR->Draw("colz");
  cRR->SaveAs( (outname+"_RR.pdf").c_str() );

  TCanvas* cNhit = new TCanvas("cNhit", "cNhit", 1500, 800);
  cNhit->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogz();
  TH2F *hNhit = new TH2F("hNhit","hNhit", 100, 0, 12000, 100, 0, 5000);
  hNhit->SetTitle("");
  tree->Draw("nhits:sqrt( (fitx1-fitx2)*(fitx1-fitx2) + (fity1-fity2)*(fity1-fity2) + (fitz1-fitz2)*(fitz1-fitz2) ) >> hNhit", "", "");
  hNhit->SetLineWidth(2);
  hNhit->GetXaxis()->SetTitle("Original - Updated Fit Distance, mm");
  hNhit->GetYaxis()->SetTitle("Nhits");
  hNhit->Draw("colz");
  cNhit->SaveAs( (outname+"_Nhit.pdf").c_str() );
  
}
