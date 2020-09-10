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
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"

#include <string>


void Compare3Fitters(){

  TFile *_fileScint = TFile::Open("scint100Z.root");
  TFile *_fileAll = TFile::Open("all100Z.root");
  TFile *_file2Z = TFile::Open("2zsplit100Z.root");

  TGraphErrors* hScint = (TGraphErrors*)_fileScint->Get("z_bias")->Clone();
  TGraphErrors* hAll = (TGraphErrors*)_fileAll->Get("z_bias")->Clone();
  TGraphErrors* h2Z = (TGraphErrors*)_file2Z->Get("z_bias")->Clone();

  hScint->SetLineColor(kRed);
  hAll->SetLineColor(kBlack);
  h2Z->SetLineColor(kBlue);

  TLegend* t1 = new TLegend( 0.6, 0.7, 0.9, 0.9 );
  t1->AddEntry( hScint, "ScintFitter", "l" );
  t1->AddEntry( hAll, "1 PDF", "l" );
  t1->AddEntry( h2Z, "2 PDFs", "l" );

  hScint->GetYaxis()->SetRangeUser(-600,600);
  //hAbove->GetXaxis()->SetTitle("Hit time residuals [ns]");
  //hAbove->GetYaxis()->SetTitle("Normalised Counts");

  hScint->Draw();
  hAll->Draw("same");
  h2Z->Draw("same");
  t1->Draw("same");

}

void Compare4Fitters(){

  //  TCanvas c1("c1", "c1", 1024, 768);

  std::string title = "Electrons Simulated in Whole Scintillator";
  //  std::string title = "Electrons with R > 1.5m";
  //std::string title = "Electrons with R < 1.5m";

  //  std::string name = "Rgt1.5_RecoordBiases1000.png";
  //  std::string name = "Rlt1.5_RecoordBiases1000.png";
  std::string name = "AllR_RecoordBiases1000.png";

  TFile *_fileScint = TFile::Open("scintFitter.root");
  TFile *_fileAll = TFile::Open("partialTestFitter_1Z.root");
  TFile *_file2Z = TFile::Open("partialTestFitter_2Z.root");
  TFile *_file4Z = TFile::Open("partialTestFitter_4Z.root");

  TGraphErrors* hScint = (TGraphErrors*)_fileScint->Get("z_bias")->Clone();
  TGraphErrors* hAll = (TGraphErrors*)_fileAll->Get("z_bias")->Clone();
  TGraphErrors* h2Z = (TGraphErrors*)_file2Z->Get("z_bias")->Clone();
  TGraphErrors* h4Z = (TGraphErrors*)_file4Z->Get("z_bias")->Clone();

  hScint->SetLineColor(kRed);
  hAll->SetLineColor(kBlack);
  h2Z->SetLineColor(kBlue);
  h4Z->SetLineColor(kGreen);
  
  TLegend* t1 = new TLegend( 0.6, 0.7, 0.9, 0.9 );
  t1->AddEntry( hScint, "ScintFitter", "l" );
  t1->AddEntry( hAll, "1 PDF", "l" );
  t1->AddEntry( h2Z, "2 PDFs", "l" );
  t1->AddEntry( h4Z, "4 PDFs", "l" );

  hScint->GetYaxis()->SetRangeUser(-600,600);
  //hAbove->GetXaxis()->SetTitle("Hit time residuals [ns]");
  //hAbove->GetYaxis()->SetTitle("Normalised Counts");
  hScint->SetTitle(title.c_str());

  hScint->Draw("AP");
  hAll->Draw("same P");
  h2Z->Draw("same P");
  h4Z->Draw("same P");
  t1->Draw("same");

  TLine *line = new TLine(hScint->GetXaxis()->GetXmax(),0,hScint->GetXaxis()->GetXmin(),0);
  line->Draw("same");

  //  c1.SaveAs(name.c_str());
}

void Compare4Resolutions(){

  //  TCanvas c1("c1", "c1", 1024, 768);

  std::string title = "Electrons Simulated in Whole Scintillator";
  //std::string title = "Electrons with R > 1.5m";
  //std::string title = "Electrons with R < 1.5m";

  //std::string name = "Rgt1.5_RecoordRes1000.png";
  //  std::string name = "Rlt1.5_RecoordRes1000.png";
  std::string name = "AllR_RecoordRes1000.png";

  TFile *_fileScint = TFile::Open("scintFitter.root");
  TFile *_fileAll = TFile::Open("partialTestFitter_1Z.root");
  TFile *_file2Z = TFile::Open("partialTestFitter_2Z.root");
  TFile *_file4Z = TFile::Open("partialTestFitter_4Z.root");

  TGraphErrors* hScint = (TGraphErrors*)_fileScint->Get("z_resolution")->Clone();
  TGraphErrors* hAll = (TGraphErrors*)_fileAll->Get("z_resolution")->Clone();
  TGraphErrors* h2Z = (TGraphErrors*)_file2Z->Get("z_resolution")->Clone();
  TGraphErrors* h4Z = (TGraphErrors*)_file4Z->Get("z_resolution")->Clone();

  hScint->SetLineColor(kRed);
  hAll->SetLineColor(kBlack);
  h2Z->SetLineColor(kBlue);
  h4Z->SetLineColor(kGreen);

  TLegend* t1 = new TLegend( 0.6, 0.7, 0.9, 0.9 );
  t1->AddEntry( hScint, "ScintFitter", "l" );
  t1->AddEntry( hAll, "1 PDF", "l" );
  t1->AddEntry( h2Z, "2 PDFs", "l" );
  t1->AddEntry( h4Z, "4 PDFs", "l" );

  hScint->GetYaxis()->SetRangeUser(0,150);
  //hAbove->GetXaxis()->SetTitle("Hit time residuals [ns]");
  //hAbove->GetYaxis()->SetTitle("Normalised Counts");
  hScint->SetTitle(title.c_str());

  hScint->Draw("AP");
  hAll->Draw("same P");
  h2Z->Draw("same P");
  h4Z->Draw("same P");
  t1->Draw("same AP");

  TLine *line = new TLine(hScint->GetXaxis()->GetXmax(),0,hScint->GetXaxis()->GetXmin(),0);
  line->Draw("same");

   //c1.SaveAs(name.c_str());
}
