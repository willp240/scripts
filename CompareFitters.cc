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
