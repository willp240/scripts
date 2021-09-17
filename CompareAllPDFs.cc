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
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"

#include <string>

void CompareAllPDFs(int numPDFs){

  TFile *_file1 = TFile::Open("MultiPDF_Plots.root");
  int col[8] = {1,4,7,3,5,6,2,28};
  TLegend* t1 = new TLegend( 0.5, 0.6, 0.9, 0.9 );
 
  for(int i=0; i<numPDFs; i++){

    TString hname = Form("PDF_%d",i);
    TString hnom = Form("Range %d",i);
    TH1D* h1 = (TH1D*)_file1->Get(hname)->Clone();

    h1->SetLineColor(col[i]);

    if(h1->Integral()>0)
      h1->Scale(1/h1->Integral());

    //    h1->GetXaxis()->SetRangeUser(-5,100);
    h1->GetXaxis()->SetTitle("Hit time residuals [ns]");
    h1->GetYaxis()->SetTitle("Normalised Counts");
    h1->GetYaxis()->SetTitleOffset(1.2);
    t1->AddEntry( h1, hnom, "l" );
    h1->Draw("same");
  }
  t1->Draw();
}
