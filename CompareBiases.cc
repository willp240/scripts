#include <TH1D.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"
#include <algorithm>
#include <cctype>
#include <string>
#include <iostream>

#include <string>


void CompareBiases(){

  std::string coord[3] = {"z", "x", "y"};
  std::string title[2] = {"MultiPDF", "MultiPath"};
  // double ymax[6] = {60, 160, 30, 180, 30, 180};
  //double ymin[6] = {-75, 80, -30, 100, -30, 100};

  gStyle->SetOptStat(0);

  //  TFile *_pdffile = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Nov26_MultiPDF_NoRangeCut120bins.root");
  TFile *_pdffile = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Nov30_MultiPDF_4PDFs.root");
  TFile *_pathfile = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Nov26_MultiPath_NoRangeCut120bins.root");

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  //c1->Divide(4,3);
  
  for(int i=3; i<15; i++){
      
      std::stringstream gname;
      gname << "y_" << i;
      std::cout << "getting " << gname.str() << std::endl;

      TH1D* hPDF = (TH1D*)_pdffile->Get(gname.str().c_str());
      TH1D* hPath = (TH1D*)_pathfile->Get(gname.str().c_str()); 

      TF1 *pdfgaus = (TF1*)hPDF->GetFunction("myGaus");
      TF1 *pathgaus = (TF1*)hPath->GetFunction("myGaus");
      
      hPath->SetLineColor(kBlue);
      hPDF->SetLineColor(kRed);
      pdfgaus->SetLineColor(kRed);
      pathgaus->SetLineColor(kBlue);
      hPath->GetYaxis()->SetRangeUser(0,2*hPath->GetMaximum());
      hPath->SetMinimum(0.5);
      hPDF->SetMinimum(0.5);

      TLegend* t1 = new TLegend( 0.6, 0.7, 0.9, 0.9 );
      t1->AddEntry( hPath, "MultiPath", "l" );
      t1->AddEntry( hPDF, "MultiPDF 4PDF", "l" );
  
      //      c1->cd(i-2);
      gPad->SetLogy();
      hPath->Draw();
      hPDF->Draw("same");
      t1->Draw("same");

  }
}

