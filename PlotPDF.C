
#include <TFile.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TMath.h>

#include <string>

void* PlotPDF( )
{

  gStyle->SetOptStat(0);

  TFile* f = new TFile("labppo_0p5_scintillator_6MeV_TruePos_centred.root", "READ");

  TH2D* hPDF = (TH2D*)f->Get("PDF")->Clone("pdf");

  hPDF->GetXaxis()->SetRangeUser(-50,200);
  hPDF->SetTitle("6 MeV, 0.5 g/l PPO, Events at Centre, True Position ");
  hPDF->GetYaxis()->SetTitle("cos(#theta_{#gamma})");
  hPDF->GetZaxis()->SetTitle( "Events" );
  hPDF->GetXaxis()->SetTitleOffset(1.8);
  hPDF->GetYaxis()->SetTitleOffset(1.8);
  hPDF->GetZaxis()->SetTitleOffset(1.4);
  hPDF->Draw("lego");


}
