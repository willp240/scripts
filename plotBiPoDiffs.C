#include <iostream>
#include <TLine.h>
#include <TStyle.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TAxis.h>


void plotBiPoDiffs( ) {

  gStyle->SetOptStat(0);
  gStyle->SetPalette(51);
  
  TFile* fIn = new TFile("/data/snoplus3/parkerw/ratSimulations/Nov25_BiPoTrees/Analysis20_PPOR_tree.root","READ");

  std::string outname = "Nov25_bipo_nolog";
  
  TTree* tree = (TTree*) fIn->Get("T");
    
  // Now make 1D plots: fitx1-fitx2, fity1-fity2, fitz1-fitz2, fitr1-fitr2, distance pos1 to pos2 for prompt and then delay
  TCanvas* cXPrompt = new TCanvas("cXPrompt", "cXPrompt", 1500,800);
  cXPrompt->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hXPrompt = new TH1F("hXPrompt","hXPrompt", 1000, -2000, 2000);
  hXPrompt->SetTitle("");
  tree->Draw("fitxprompt2-fitxprompt1 >> hXPrompt", "", "");
  hXPrompt->SetLineWidth(2);
  hXPrompt->SetLineColor(kBlue+2);
  hXPrompt->GetXaxis()->SetTitle("Prompt Original Fit X - Updated Fit X, mm");
  hXPrompt->GetYaxis()->SetTitle("Events");
  hXPrompt->Draw();
  cXPrompt->SaveAs( (outname+"_XPrompt.pdf").c_str() );
  std::cout << "x prompt " << hXPrompt->Integral() << " " <<  hXPrompt->Integral(-450,550) << " " << hXPrompt->Integral(450, 550) / hXPrompt->Integral() << std::endl;

  TCanvas* cXDelay = new TCanvas("cXDelay", "cXDelay", 1500,800);
  cXDelay->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hXDelay = new TH1F("hXDelay","hXDelay", 1000, -2000, 2000);
  hXDelay->SetTitle("");
  tree->Draw("fitxdelayed2-fitxdelayed1 >> hXDelay", "", "");
  hXDelay->SetLineWidth(2);
  hXDelay->SetLineColor(kBlue+2);
  hXDelay->GetXaxis()->SetTitle("Delay Original Fit X - Updated Fit X, mm");
  hXDelay->GetYaxis()->SetTitle("Events");
  hXDelay->Draw();
  cXDelay->SaveAs( (outname+"_XDelay.pdf").c_str() );
  std::cout << "x delay " << hXDelay->Integral() << " " <<  hXDelay->Integral(-450,550) << " " << hXDelay->Integral(450, 550) / hXDelay->Integral() << std::endl;


  TCanvas* cYPrompt = new TCanvas("cYPrompt", "cYPrompt", 1500,800);
  cYPrompt->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hYPrompt = new TH1F("hYPrompt","hYPrompt", 1000, -2000, 2000);
  hYPrompt->SetTitle("");
  tree->Draw("fityprompt2-fityprompt1 >> hYPrompt", "", "");
  hYPrompt->SetLineWidth(2);
  hYPrompt->SetLineColor(kBlue+2);
  hYPrompt->GetXaxis()->SetTitle("Prompt Original Fit Y - Updated Fit Y, mm");
  hYPrompt->GetYaxis()->SetTitle("Events");
  hYPrompt->Draw();
  cYPrompt->SaveAs( (outname+"_YPrompt.pdf").c_str() );
  std::cout << "y prompt " << hYPrompt->Integral() << " " <<  hYPrompt->Integral(-450,550) << " " << hYPrompt->Integral(450, 550) / hYPrompt->Integral() << std::endl;

  TCanvas* cYDelay = new TCanvas("cYDelay", "cYDelay", 1500,800);
  cYDelay->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hYDelay = new TH1F("hYDelay","hYDelay", 1000, -2000, 2000);
  hYDelay->SetTitle("");
  tree->Draw("fitydelayed2-fitydelayed1 >> hYDelay", "", "");
  hYDelay->SetLineWidth(2);
  hYDelay->SetLineColor(kBlue+2);
  hYDelay->GetXaxis()->SetTitle("Delay Original Fit Y - Updated Fit Y, mm");
  hYDelay->GetYaxis()->SetTitle("Events");
  hYDelay->Draw();
  cYDelay->SaveAs( (outname+"_YDelay.pdf").c_str() );
  std::cout << "y delay " << hYDelay->Integral() << " " <<  hYDelay->Integral(-450,550) << " " << hYDelay->Integral(450, 550) / hYDelay->Integral() << std::endl;


  TCanvas* cZPrompt = new TCanvas("cZPrompt", "cZPrompt", 1500,800);
  cZPrompt->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hZPrompt = new TH1F("hZPrompt","hZPrompt", 1000, -2000, 2000);
  hZPrompt->SetTitle("");
  tree->Draw("fitzprompt2-fitzprompt1 >> hZPrompt", "", "");
  hZPrompt->SetLineWidth(2);
  hZPrompt->SetLineColor(kBlue+2);
  hZPrompt->GetXaxis()->SetTitle("Prompt Original Fit Z - Updated Fit Z, mm");
  hZPrompt->GetYaxis()->SetTitle("Events");
  hZPrompt->Draw();
  cZPrompt->SaveAs( (outname+"_ZPrompt.pdf").c_str() );
  std::cout << "z prompt " << hZPrompt->Integral() << " " <<  hZPrompt->Integral(-450,550) << " " << hZPrompt->Integral(450, 550) / hZPrompt->Integral() << std::endl;
  
  TCanvas* cZDelay = new TCanvas("cZDelay", "cZDelay", 1500,800);
  cZDelay->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hZDelay = new TH1F("hZDelay","hZDelay", 1000, -2000, 2000);
  hZDelay->SetTitle("");
  tree->Draw("fitzdelayed2-fitzdelayed1 >> hZDelay", "", "");
  hZDelay->SetLineWidth(2);
  hZDelay->SetLineColor(kBlue+2);
  hZDelay->GetXaxis()->SetTitle("Delay Original Fit Z - Updated Fit Z, mm");
  hZDelay->GetYaxis()->SetTitle("Events");
  hZDelay->Draw();
  cZDelay->SaveAs( (outname+"_ZDelay.pdf").c_str() );
  std::cout << "z delay " << hZDelay->Integral() << " " <<  hZDelay->Integral(-450,550) << " " << hZDelay->Integral(450, 550) / hZDelay->Integral() << std::endl;

  TCanvas* cRPrompt = new TCanvas("cRPrompt", "cRPrompt", 1500,800);
  cRPrompt->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hRPrompt = new TH1F("hRPrompt","hRPrompt", 1000, -2000, 2000);
  hRPrompt->SetTitle("");
  tree->Draw("fitrprompt2-fitrprompt1 >> hRPrompt", "", "");
  hRPrompt->SetLineWidth(2);
  hRPrompt->SetLineColor(kBlue+2);
  hRPrompt->GetXaxis()->SetTitle("Prompt Original Fit R - Updated Fit R, mm");
  hRPrompt->GetYaxis()->SetTitle("Events");
  hRPrompt->Draw();
  cRPrompt->SaveAs( (outname+"_RPrompt.pdf").c_str() );
    std::cout << "r prompt " << hRPrompt->Integral() << " " <<  hRPrompt->Integral(-450,550) << " " << hRPrompt->Integral(450, 550) / hRPrompt->Integral() << std::endl;
  
  TCanvas* cRDelay = new TCanvas("cRDelay", "cRDelay", 1500,800);
  cRDelay->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hRDelay = new TH1F("hRDelay","hRDelay", 1000, -2000, 2000);
  hRDelay->SetTitle("");
  tree->Draw("fitrdelayed2-fitrdelayed1 >> hRDelay", "", "");
  hRDelay->SetLineWidth(2);
  hRDelay->SetLineColor(kBlue+2);
  hRDelay->GetXaxis()->SetTitle("Delay Original Fit R - Updated Fit R, mm");
  hRDelay->GetYaxis()->SetTitle("Events");
  hRDelay->Draw();
  cRDelay->SaveAs( (outname+"_RDelay.pdf").c_str() );
  std::cout << "r delay " << hZDelay->Integral() << " " <<  hZDelay->Integral(-450,550) << " " << hZDelay->Integral(450, 550) / hZDelay->Integral() << std::endl;
  
  TCanvas* cDPrompt = new TCanvas("cDPrompt", "cDPrompt", 1500,800);
  cDPrompt->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hDPrompt = new TH1F("hDPrompt","hDPrompt", 100, 0, 2000);
  hDPrompt->SetTitle("");
  tree->Draw("sqrt( (fitxprompt1-fitxprompt2)*(fitxprompt1-fitxprompt2) + (fityprompt1-fityprompt2)*(fityprompt1-fityprompt2) + (fitzprompt2-fitzprompt1)*(fitzprompt2-fitzprompt1) ) >> hDPrompt", "", "");
  hDPrompt->SetLineWidth(2);
  hDPrompt->SetLineColor(kBlue+2);
  hDPrompt->GetXaxis()->SetTitle("Prompt Original - Updated Fit Distance, mm");
  hDPrompt->GetYaxis()->SetTitle("Events");
  hDPrompt->Draw();
  cDPrompt->SaveAs( (outname+"_DPrompt.pdf").c_str() );
  std::cout << "d prompt " << hDPrompt->Integral() << " " <<  hDPrompt->Integral(0,10) << " " << hDPrompt->Integral(0, 10) / hDPrompt->Integral() << std::endl;
  
  TCanvas* cDDelay = new TCanvas("cDDelay", "cDDelay", 1500,800);
  cDDelay->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hDDelay = new TH1F("hDDelay","hDDelay", 100, 0, 2000);
  hDDelay->SetTitle("");
  tree->Draw(" sqrt( (fitxdelayed1-fitxdelayed2)*(fitxdelayed1-fitxdelayed2) + (fitydelayed1-fitydelayed2)*(fitydelayed1-fitydelayed2) + (fitzdelayed2-fitzdelayed1)*(fitzdelayed2-fitzdelayed1) )  >> hDDelay", "", "");
  hDDelay->SetLineWidth(2);
  hDDelay->SetLineColor(kBlue+2);
  hDDelay->GetXaxis()->SetTitle("Delay Original - Updated Fit Distance, mm");
  hDDelay->GetYaxis()->SetTitle("Events");
  hDDelay->Draw();
  cDDelay->SaveAs( (outname+"_DDelay.pdf").c_str() );
  std::cout << "d delay " << hDDelay->Integral() << " " <<  hDDelay->Integral(0, 10) << " " << hDDelay->Integral(0, 10) / hDDelay->Integral() << std::endl;
  
  TCanvas* cD = new TCanvas("cD", "cD", 1500,800);
  cD->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hD1 = new TH1F("hD1","hD1", 40, 0, 1200);
  hD1->SetTitle("");
  tree->Draw("sqrt( (fitxprompt1-fitxdelayed1)*(fitxprompt1-fitxdelayed1) + (fityprompt1-fitydelayed1)*(fityprompt1-fitydelayed1) + (fitzprompt1-fitzdelayed1)*(fitzprompt1-fitzdelayed1) ) >> hD1", "", "");
  hD1->SetLineWidth(2);
  hD1->SetLineColor(kBlue+2);
  hD1->GetXaxis()->SetTitle("Prompt - Delay Fit Distance, mm");
  hD1->GetYaxis()->SetTitle("Events");
  TH1F *hD2 = new TH1F("hD2","hD2", 40, 0, 1200);
  hD2->SetTitle("");
  tree->Draw("sqrt( (fitxprompt2-fitxdelayed2)*(fitxprompt2-fitxdelayed2) + (fityprompt2-fitydelayed2)*(fityprompt2-fitydelayed2) + (fitzprompt2-fitzdelayed2)*(fitzprompt2-fitzdelayed2) ) >> hD2", "", "");
  hD2->SetLineWidth(2);
  hD2->SetLineColor(kRed+2);
  hD2->SetLineStyle(1);
  hD1->Draw("e1");
  hD2->Draw("e1same");
  TLegend* t1 = new TLegend( 0.6, 0.4, 0.8, 0.6 );
  t1->AddEntry(hD2, "Original", "l");
  t1->AddEntry(hD1, "Updated", "l");
  t1->SetLineWidth(2);
  t1->Draw("same");
  cD->SaveAs( (outname+"_D.pdf").c_str() );
 
  TCanvas* cD2 = new TCanvas("cD2", "cD2", 1500,800);
  cD2->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  hD1->Draw();
  hD2->Draw("same");
  t1->Draw("same");
  cD2->SaveAs( (outname+"_D2.pdf").c_str() );

}
