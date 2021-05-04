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
  double ymax[6] = {20, 150, 20, 150, 20, 150};
  double ymin[6] = {-20, 0, -20, 0, -20, 0};


  TFile *_old = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Mar2_ScintFitter_0to10MeV_R.root");
  TFile *_new = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Mar3_StraightPath_0to10MeV_R.root");
  TCanvas* c1 = new TCanvas("c1", "c1", 1600,800);
  c1->Divide(3,2);
  
  for(int j=0; j<2; j++){

    for(int i=0; i<3; i++){
      
      std::string gname = coord[i] + "_" + title[j];
      std::cout << "getting " << gname << " " << 3*j+i+1 << std::endl;

      TGraphErrors* hOld = (TGraphErrors*)_old->Get(gname.c_str())->Clone();
      TGraphErrors* hNew = (TGraphErrors*)_new->Get(gname.c_str())->Clone();
  
      hOld->SetLineColor(kBlue);
      hNew->SetLineColor(kRed);

      TLegend* t1 = new TLegend( 0.45, 0.75, 0.9, 0.9 );
      t1->AddEntry( hOld, "ScintFitter", "l" );
      t1->AddEntry( hNew, "Straight Paths", "l" );

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

void Compare2RFits(){

  std::string coord[4] = {"x", "y", "z", "r"};
  std::string title[2] = {"bias", "resolution"};
  double ymax[8] = {30, 125, 30, 125, 50, 125, 100, 125};
  double ymin[8] = {-30, 0, -30, 0, -50, 0, -100, 0};


  //  TFile *_old = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Mar4_ScintFitter_1to10MeV_0R_E.root");
  //TFile *_new = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Mar4_StraightPath_1to10MeV_0R_E.root");

  TFile *_old = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Apr15_fixBeginOfRunFullSF.root");
  TFile *_new = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Apr25_5GausNorm_dist15.root");

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,700);
  c1->Divide(4,2,0.01,0.05);
  
  for(int j=0; j<2; j++){

    for(int i=0; i<4; i++){
      
      std::string gname = coord[i] + "_" + title[j];
      std::cout << "getting " << gname << " " << 3*j+i+1 << std::endl;

      TGraphErrors* hOld = (TGraphErrors*)_old->Get(gname.c_str())->Clone();
      TGraphErrors* hNew = (TGraphErrors*)_new->Get(gname.c_str())->Clone();
  
      hOld->SetLineColor(kBlue);
      hNew->SetLineColor(kRed);

      TLegend* t1 = new TLegend( 0.4, 0.75, 0.9, 0.9 );
      t1->AddEntry( hOld, "Histogram PDF", "l" );
      t1->AddEntry( hNew, "Full Functional MultiPDF", "l" );

      hOld->GetYaxis()->SetRangeUser(ymin[2*i+j],ymax[2*i+j]);
      gname = coord[i] + " " + title[j];
      hOld->SetTitle(gname.c_str());
  
      c1->cd(4*j + i + 1);

      hOld->Draw("ap");
      hNew->Draw("same p");
      t1->Draw("same");

      if(j==0){
	TLine *line = new TLine(hOld->GetXaxis()->GetXmin(),0,hOld->GetXaxis()->GetBinUpEdge(92),0);
	//	TLine *line = new TLine(-6048,0,hOld->GetXaxis()->GetBinUpEdge(92),0);
	line->Draw("same");
      }
      else{
	hOld->GetYaxis()->SetTitleOffset(1.1);
      }
    }
  }
}


void Compare4Fits(){

  std::string coord[4] = {"x", "y", "z", "r"};
  std::string title[2] = {"bias", "resolution"};
  double ymax[8] = {20, 150, 20, 150, 20, 150, 150, 150};
  double ymin[8] = {-20, 0, -20, 0, -20, 0, -60, 0};


  TFile *_mPath = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Apr8Redo_Mar31_FunctionalPDF_7mTable.root");
  TFile *_mPDF_1 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Apr8_no0.root");
  TFile *_mPDF_4 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Apr16_5Gaus100000.root");
  TFile *_mPDF_8 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Apr16_5Gaus100000.root");
  TCanvas* c1 = new TCanvas("c1", "c1", 1500,700);
  c1->Divide(4,2,0.01,0.05);

  for(int j=0; j<2; j++){

    for(int i=0; i<4; i++){

      std::string gname = coord[i] + "_" + title[j];
      std::cout << "getting " << gname << " " << 4*j+i+1 << std::endl;

      TGraphErrors* hOldPath = (TGraphErrors*)_mPath->Get(gname.c_str())->Clone();
      TGraphErrors* hNewPath = (TGraphErrors*)_mPDF_1->Get(gname.c_str())->Clone();
      TGraphErrors* hOldPDF = (TGraphErrors*)_mPDF_4->Get(gname.c_str())->Clone();
      TGraphErrors* hNewPDF = (TGraphErrors*)_mPDF_8->Get(gname.c_str())->Clone();

      hOldPath->SetLineColor(kBlue);
      hNewPath->SetLineColor(kBlack);
      hOldPDF->SetLineColor(kRed);
      hNewPDF->SetLineColor(kGreen);

      TLegend* t1 = new TLegend( 0.4, 0.65, 0.9, 0.9 );
      t1->AddEntry( hOldPath, "Histogram", "l" );
      t1->AddEntry( hNewPath, "4 Gaussian Function", "l" );
      //t1->AddEntry( hOldPDF, "5 Gaussian", "l" );
      t1->AddEntry( hNewPDF, "5 Gaussian Function", "l" );

      hOldPath->GetYaxis()->SetRangeUser(ymin[2*i+j],ymax[2*i+j]);
      gname = coord[i] + " " + title[j];
      hOldPath->SetTitle(gname.c_str());

      c1->cd(4*j + i + 1);

      hOldPath->Draw("ap");
      hNewPath->Draw("same p");
      hOldPDF->Draw("same p");
      hNewPDF->Draw("same p");
      t1->Draw("same");

      if(j==0){
        TLine *line = new TLine(hOldPath->GetXaxis()->GetXmin(),0,hOldPDF->GetXaxis()->GetBinUpEdge(92),0);
        //TLine *line = new TLine(-6048,0,hOldPDF->GetXaxis()->GetBinUpEdge(92),0);
	line->Draw("same");
      }
      else{
        hOldPath->GetYaxis()->SetTitleOffset(1.1);
      }
    }
  }
}


void Compare2EFits(){

  std::string coord[3] = {"z", "x", "y"};
  std::string title[2] = {"bias", "resolution"};
  double ymax[6] = {60, 160, 30, 200, 30, 200};
  double ymin[6] = {-75, 80, -30, 130, -30, 130};


  TFile *_old = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Dec9_multipathE.root");
  TFile *_new = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Dec9_multipdfE.root");
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
      t1->AddEntry( hOld, "MultiPath", "l" );
      t1->AddEntry( hNew, "MultiPDF", "l" );
  
      //      hOld->GetYaxis()->SetRangeUser(ymin[2*i+j],ymax[2*i+j]);
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

void CompareEfficiencies(){

  TFile *_old = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Mar2_ScintFitter_Seed.root");
  TFile *_new = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Mar2_StraightPath_Powell.root");
  TCanvas* c1 = new TCanvas("c1", "c1", 1600,800);

  TH1D* h1 = (TH1D*)_old->Get("fitValidEfficiency");
  TH1D* h2 = (TH1D*)_new->Get("fitValidEfficiency");

  h1->SetLineColor(kBlue);
  h2->SetLineColor(kRed);
  h2->SetLineStyle(10);

  TLegend* t1 = new TLegend( 0.6, 0.7, 0.9, 0.9 );
  t1->AddEntry( h1, "ScintFitter", "l" );
  t1->AddEntry( h2, "StraightPath", "l" );

  h1->Draw();
  h2->Draw("same");
  t1->Draw();

}

void Compare1Fit(){

  TFile *_old = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Mar31_FunctionalPDF_7m.root");
  TCanvas* c1 = new TCanvas("c1", "c1", 1600,800);
  c1->Divide(2,2);

  c1->cd(1);
  TCanvas* h1 = (TCanvas*)_old->Get("xyz_bias_c");
  h1->DrawClonePad();
  
  c1->cd(2);
  TH1D* h2 = (TH1D*)_old->Get("xyz_resolution_c");
  h2->Draw();

  c1->cd(3);
  TH1D* h3 = (TH1D*)_old->Get("r_bias_c");
  h3->Draw();

  c1->cd(4);
  TH1D* h4 = (TH1D*)_old->Get("r_resolution_c");
  h4->Draw();

}

void CompareRecoRad(){

  TFile *_old = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Mar2_ScintFitter_Seed.root");
  TFile *_new = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Mar2_StraightPath_Powell.root");
  TCanvas* c1 = new TCanvas("c1", "c1", 1600,800);

  TH1D* h1 = (TH1D*)_old->Get("r_fit_distribution_h");
  TH1D* h2 = (TH1D*)_new->Get("r_fit_distribution_h");

  h1->SetLineColor(kBlue);
  h2->SetLineColor(kRed);
  h2->SetLineStyle(10);

  TLegend* t1 = new TLegend( 0.1, 0.7, 0.4, 0.9 );
  t1->AddEntry( h1, "ScintFitter", "l" );
  t1->AddEntry( h2, "StraightPath", "l" );

  h1->Draw();
  h2->Draw("same");
  t1->Draw();

}
