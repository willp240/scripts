#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"
#include <TMath.h>
#include <TF2.h>

#include <string>
using namespace std;

double* CompPlots( )
{

  TFile *_file1 = TFile::Open("fit5000.root");
  TFile *_file2 = TFile::Open("fit5900.root");

  TH1D* hdist1 = (TH1D*)_file1->Get("fDistance")->Clone("fdist1");
  TH1D* hdist2 = (TH1D*)_file2->Get("fDistance")->Clone("fdist2");

  hdist1->SetLineColor(kRed);
  hdist2->SetLineColor(kBlue);

  TLegend* t1 = new TLegend( 0.5, 0.6, 0.9, 0.9 );
  t1->AddEntry( hdist1, "Proposed Vertex (5000,0,0)", "l" );
  t1->AddEntry( hdist2, "Proposed Vertex (000,0,0)", "l" );

  hdist1->Draw();
  hdist2->Draw("same");
  t1->Draw();

  TH1D* htime1 = (TH1D*)_file1->Get("fHitTimeResidualsMC")->Clone("ftime1");
  TH1D* htime2 = (TH1D*)_file2->Get("fHitTimeResidualsMC")->Clone("ftime2");

  htime1->SetLineColor(kRed);
  htime2->SetLineColor(kBlue);
  htime1->Draw();
  htime2->Draw("same");
  t1->Draw();

}
