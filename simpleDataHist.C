#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"
#include <TMath.h>

#include <string>
using namespace std;

void* simpleDataHist( )
{
  gStyle->SetOptStat(0);
  
  TH1D* hPDF = new TH1D("PDF","PDF",4,0,4);
  TH1D* hData = new TH1D("Data","Data",4,0,4);

  hPDF->SetBinContent(1,20);
  hPDF->SetBinContent(2,35);
  hPDF->SetBinContent(3,30);
  hPDF->SetBinContent(4,15);

  hPDF->SetBinError(1,0.1);
  hPDF->SetBinError(2,0.1);
  hPDF->SetBinError(3,0.1);
  hPDF->SetBinError(4,0.1);

  hData->SetBinContent(1,25);
  hData->SetBinContent(2,30);
  hData->SetBinContent(3,40);
  hData->SetBinContent(4,10);

  hPDF->SetLineColor(kRed);
  hData->GetXaxis()->SetTitle("Energy, MeV");
  hData->GetYaxis()->SetTitle("Events");
  
  TLegend* t1 = new TLegend( 0.5, 0.6, 0.88, 0.88);
  t1->AddEntry(hData,"Data","l");
  t1->AddEntry(hPDF,"PDF","l");

  hData->Draw();
  hPDF->Draw("LE1 same");
  t1->Draw();
}
