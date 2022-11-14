
#include <TFile.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TMath.h>

#include <string>

void* PlotPDF( )
{

  bool colz = false;

  gStyle->SetOptStat(0);
  gStyle->SetPalette(55);

  TFile* f = new TFile("labppo_2p2_scintillator_6MeV_TruePos_fullvolume.root", "READ");

  TH2D* hPDF = (TH2D*)f->Get("PDF")->Clone("pdf");

  hPDF->GetXaxis()->SetRangeUser(-20,100);
  hPDF->SetTitle("6 MeV, 2.2 g/l, R < 3m, 0.1ns Bin Width");
  hPDF->GetYaxis()->SetTitle("cos(#theta_{#gamma})");
  hPDF->GetZaxis()->SetTitle( "Events" );
  if(!colz){
    hPDF->GetXaxis()->SetRangeUser(-50,200);
    hPDF->GetXaxis()->SetTitleOffset(1.8);
    hPDF->GetYaxis()->SetTitleOffset(1.8);
    hPDF->GetZaxis()->SetTitleOffset(1.3);
    hPDF->Draw("lego");
  }
  else
    hPDF->Draw("colz");

}
