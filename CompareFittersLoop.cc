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
#include <algorithm>
#include <cctype>
#include <string>

#include <string>


void Compare2Fits(){

  std::string coord[3] = {"z", "x", "y"};
  std::string title[2] = {"bias", "resolution"};
  double ymax[6] = {60, 160, 30, 180, 30, 180};
  double ymin[6] = {-75, 80, -30, 100, -30, 100};


  TFile *_old = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Nov19_multipath.root");
  TFile *_new = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Nov26_MultiPath.root");
  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->Divide(3,2);
  
  for(int j=0; j<2; j++){

    for(int i=0; i<3; i++){
      
      std::string gname = coord[i] + "_" + title[j];
      std::cout << "getting " << gname << " " << 3*j+i+1 << std::endl;

      TGraphErrors* hOld = (TGraphErrors*)_old->Get(gname.c_str())->Clone();
      TGraphErrors* hNew = (TGraphErrors*)_new->Get(gname.c_str())->Clone();
  
      hOld->SetLineColor(kBlue);
      hNew->SetLineColor(kRed);

      TLegend* t1 = new TLegend( 0.6, 0.7, 0.9, 0.9 );
      t1->AddEntry( hOld, "Pre-Recoordination", "l" );
      t1->AddEntry( hNew, "Recoordinated", "l" );
  
      hOld->GetYaxis()->SetRangeUser(ymin[2*i+j],ymax[2*i+j]);
      gname = coord[i] + " " + title[j];
      hOld->SetTitle(gname.c_str());
  
      c1->cd(3*j + i + 1);

      hOld->Draw("ap");
      hNew->Draw("same p");
      t1->Draw("same");

      if(j==0){
	TLine *line = new TLine(hOld->GetXaxis()->GetXmin(),0,hOld->GetXaxis()->GetBinUpEdge(92),0);
	line->Draw("same");
      }
      else{
	hOld->GetYaxis()->SetTitleOffset(1.1);
      }
    }
  }
}


void Compare4Fits(){

  std::string coord[3] = {"z", "x", "y"};
  std::string title[2] = {"bias", "resolution"};
  double ymax[6] = {60, 180, 30, 200, 30, 200};
  double ymin[6] = {-75, 80, -30, 100, -30, 100};


  TFile *_oldPath = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Nov19_multipath.root");
  TFile *_newPath = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Nov26_MultiPath.root");
  TFile *_oldPDF = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Nov19_multipdf.root");
  TFile *_newPDF = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Nov26_MultiPDF.root");
  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->Divide(3,2);

  for(int j=0; j<2; j++){

    for(int i=0; i<3; i++){

      std::string gname = coord[i] + "_" + title[j];
      std::cout << "getting " << gname << " " << 3*j+i+1 << std::endl;

      TGraphErrors* hOldPath = (TGraphErrors*)_oldPath->Get(gname.c_str())->Clone();
      TGraphErrors* hNewPath = (TGraphErrors*)_newPath->Get(gname.c_str())->Clone();
      TGraphErrors* hOldPDF = (TGraphErrors*)_oldPDF->Get(gname.c_str())->Clone();
      TGraphErrors* hNewPDF = (TGraphErrors*)_newPDF->Get(gname.c_str())->Clone();

      hOldPath->SetLineColor(kBlue);
      hNewPath->SetLineColor(kBlue+2);
      hOldPDF->SetLineColor(kRed);
      hNewPDF->SetLineColor(kRed+2);

      TLegend* t1 = new TLegend( 0.4, 0.65, 0.9, 0.9 );
      t1->AddEntry( hOldPath, "MultiPath Pre-Recoordination", "l" );
      t1->AddEntry( hNewPath, "MultiPath Recoordinated", "l" );
      t1->AddEntry( hOldPDF, "MultiPDF Pre-Recoordination", "l" );
      t1->AddEntry( hNewPDF, "MultiPDF Recoordinated", "l" );

      hOldPath->GetYaxis()->SetRangeUser(ymin[2*i+j],ymax[2*i+j]);
      gname = coord[i] + " " + title[j];
      hOldPath->SetTitle(gname.c_str());

      c1->cd(3*j + i + 1);

      hOldPath->Draw("ap");
      hNewPath->Draw("same p");
      hOldPDF->Draw("same p");
      hNewPDF->Draw("same p");
      t1->Draw("same");

      if(j==0){
        TLine *line = new TLine(hOldPath->GetXaxis()->GetXmin(),0,hOldPDF->GetXaxis()->GetBinUpEdge(92),0);
        line->Draw("same");
      }
      else{
        hOldPath->GetYaxis()->SetTitleOffset(1.1);
      }
    }
  }
}
