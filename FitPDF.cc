#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"
#include <TMath.h>

#include <string>
using namespace std;

TH1D* FitPDF( )
{

  TFile *_fileAll = TFile::Open("timeresid.root");
  
  TH1D* h = (TH1D*) _fileAll->Get("h1");
    h->GetXaxis()->SetRangeUser(-5,30);

  TF1 *f = new TF1("f", "([0]) * (exp(-x/[1]) - exp(-x/[2])) / ([1]-[2])");

  f->SetLineColor(kRed);
  f->SetParameter(0,0.820);
  //  f->SetParLimits(0,0.0,1.0);
  f->SetParameter(1,1.01);
  //f->SetParLimits(1,-2.0,2.0);
  f->SetParameter(2,11.16);
  //f->SetParLimits(2,-2.0,2.0);
  //f->SetParameter(3,-0.187316);
  // f->SetParLimits(3,0,2.0);
  //  f->Draw();
  h->Fit("f", "ML");
  //  f->Draw();
//h->Fit("landau");
  h->GetXaxis()->SetRangeUser(-200,500);
  f->GetXaxis()->SetRangeUser(-200,500);
  h->Draw("");
  //h->GetYaxis()->SetRangeUser(-.2,.2);
  
}


