#include <RAT/DU/DSReader.hh>
#include <RAT/DU/Utility.hh>
// #include <RAT/DU/PMTInfo.hh>
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


  TFile *_old = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Jun27_FitPerf_2.5MeV_1.5Terecoord_R.root");
  TFile *_new = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Jun27_FitPerf_2.5MeV_2.5Terecoord_R.root");
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
      t1->AddEntry( hOld, "1.5% Te", "l" );
      t1->AddEntry( hNew, "2.5% Te", "l" );

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

  gStyle->SetGridStyle(3);
  gStyle->SetFrameLineWidth(2);

  std::string coord[4] = {"x", "y", "z", "r"};
  std::string title[2] = {"bias", "resolution"};
  double ymax[8] = {30, 150, 30, 150, 30, 150, 40, 150};
  double ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};


  TFile *_old = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Apr15_SEV_Interp_2.5MeV_R_Rlt6m.root");
  TFile *_new = TFile::Open("/data/snoplus3/parkerw/ratSimulations/Sep10_testdag/perf_e2p5MeV_tools/perf_e2p5MeV_tools_r.root");
  //TFile *_old = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Jun28_FitPerf_2.5MeV_1.5Terecoord_Z.root");
  //TFile *_new = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Jun28_FitPerf_2.5MeV_2.5Terecoord_Z.root");

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,700);
  c1->Divide(4,2,0.01,0.05);
  c1->SetGrid(1,1);
  
  for(int j=0; j<2; j++){

    for(int i=0; i<4; i++){
      
      std::string gname = coord[i] + "_" + title[j];
      std::cout << "getting " << gname << " " << 3*j+i+1 << std::endl;

      TGraphErrors* hOld = (TGraphErrors*)_old->Get(gname.c_str())->Clone();
      TGraphErrors* hNew = (TGraphErrors*)_new->Get(gname.c_str())->Clone();
  
      hOld->SetLineColor(kBlue+2);
      hNew->SetLineColor(kRed+2);
      hOld->SetLineWidth(2);
      hNew->SetLineWidth(2);

      TLegend* t1 = new TLegend( 0.4, 0.75, 0.9, 0.9 );
      t1->AddEntry( hOld, "Original", "l" );
      t1->AddEntry( hNew, "Auto Recoordinated", "l" );
      t1->SetLineWidth(2);

      hOld->GetYaxis()->SetRangeUser(ymin[2*i+j],ymax[2*i+j]);
      gname = coord[i] + " " + title[j];
      hOld->SetTitle(gname.c_str());
  
      c1->cd(4*j + i + 1);

      hOld->Draw("ap");
      hNew->Draw("same p");
      t1->Draw("same");

      if(j==0){
	std::cout << hOld->GetXaxis()->GetNbins() << std::endl;
	TLine *line = new TLine(hOld->GetXaxis()->GetXmin()+1, 0, 6048, 0);//99
	//TLine *line = new TLine(-6048, 0, 6048, 0);
	//TLine *line = new TLine(1,0,10,0);//99
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
        //TLine *line = new TLine(hOldPath->GetXaxis()->GetXmin(),0,hOldPDF->GetXaxis()->GetBinUpEdge(92),0);
        TLine *line = new TLine(-6048,0,hOldPDF->GetXaxis()->GetBinUpEdge(92),0);
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

  TFile *_old = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Apr15_SEV_Interp_2.5MeV_R_Rlt6m.root");
  TFile *_new = TFile::Open("/data/snoplus3/parkerw/ratSimulations/Sep10_testdag/perf_e2p5MeV_tools/perf_e2p5MeV_tools_r.root");
  //TFile *_old = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Dec9_multipathE.root");
  //TFile *_new = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Dec9_multipdfE.root");
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

  TFile *_old = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/May10_FuncExp_x.root");
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


void Compare6Fits(){

  std::string coord[4] = {"x", "y", "z", "r"};
  std::string title[2] = {"bias", "resolution"};
  double ymax[8] = {20, 150, 20, 150, 20, 150, 150, 150};
  double ymin[8] = {-20, 0, -20, 0, -20, 0, -60, 0};


  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/May5_Table3m_b.root");
  TFile *file2 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/May5_Table3m.root");
  TFile *file3 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/May5_Table7m_b.root");
  TFile *file4 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/May5_Table9m_b.root");
  TFile *file5 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/May5_Table9m.root");
  TFile *file6 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/final/Mar2_StraightPath_Powell.root");

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,700);
  c1->Divide(4,2,0.01,0.05);

  for(int j=0; j<2; j++){

    for(int i=0; i<4; i++){

      std::string gname = coord[i] + "_" + title[j];
      std::cout << "getting " << gname << " " << 4*j+i+1 << std::endl;

      TGraphErrors* h1 = (TGraphErrors*)file1->Get(gname.c_str())->Clone();
      TGraphErrors* h2 = (TGraphErrors*)file2->Get(gname.c_str())->Clone();
      TGraphErrors* h3 = (TGraphErrors*)file3->Get(gname.c_str())->Clone();
      TGraphErrors* h4 = (TGraphErrors*)file4->Get(gname.c_str())->Clone();
      TGraphErrors* h5 = (TGraphErrors*)file5->Get(gname.c_str())->Clone();
      TGraphErrors* h6 = (TGraphErrors*)file6->Get(gname.c_str())->Clone();

      h1->SetLineColor(kBlack);
      h2->SetLineColor(kBlue);
      h3->SetLineColor(kCyan);
      h4->SetLineColor(kGreen);
      h5->SetLineColor(kYellow);
      h6->SetLineColor(kPink);

      TLegend* t1 = new TLegend( 0.4, 0.65, 0.9, 0.9 );
      t1->AddEntry( h1, "3m PDF", "l" );
      t1->AddEntry( h2, "5m PDF", "l" );
      t1->AddEntry( h3, "7m PDF", "l" );
      t1->AddEntry( h4, "9m PDF", "l" );
      t1->AddEntry( h5, "11m PDF", "l" );
      t1->AddEntry( h6, "Full PDF", "l" );

      h1->GetYaxis()->SetRangeUser(ymin[2*i+j],ymax[2*i+j]);
      gname = coord[i] + " " + title[j];
      h1->SetTitle(gname.c_str());

      c1->cd(4*j + i + 1);

      h1->Draw("ap");
      h2->Draw("same p");
      h3->Draw("same p");
      h4->Draw("same p");
      h5->Draw("same p");
      h6->Draw("same p");
      t1->Draw("same");

      if(j==0){
        TLine *line = new TLine(h1->GetXaxis()->GetXmin(),0,h1->GetXaxis()->GetBinUpEdge(92),0);
        //TLine *line = new TLine(-6048,0,hOldPDF->GetXaxis()->GetBinUpEdge(92),0);
        line->Draw("same");
      }
      else{
        h1->GetYaxis()->SetTitleOffset(1.1);
      }

    }
  }
}

void Compare3RFits(){

  gStyle->SetGridStyle(3);
  gStyle->SetFrameLineWidth(2);

  std::string coord[4] = {"x", "y", "z", "r"};
  std::string title[2] = {"bias", "resolution"};
  double ymax[8] = {30, 140, 30, 140, 30, 140, 100, 140};
  double ymin[8] = {-30, 0, -30, 0, -30, 0, -25, 0};
  
  TFile *f1 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/Jul21_recoordMPDF_2p2gl_perf_2p5MeV_R_gaus.root");
  TFile *f2 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/Aug24_MPDFRecoord_4m_perf2.5MeV_R.root");
  TFile *f3 = TFile::Open("/home/parkerw/Software/rat-tools_master/FitPerformance/Sep27_recoordScintEffVelMPDF_Perf_ds.root");

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,700);
  c1->Divide(4,2,0.01,0.05);
  c1->SetGrid(1,1);
  
  for(int j=0; j<2; j++){
    
    for(int i=0; i<4; i++){
      
      std::string gname = coord[i] + "_" + title[j];
      std::cout << "getting " << gname << " " << 3*j+i+1 << std::endl;
      
      TGraphErrors* h1 = (TGraphErrors*)f1->Get(gname.c_str())->Clone();
      TGraphErrors* h2 = (TGraphErrors*)f2->Get(gname.c_str())->Clone();
      TGraphErrors* h3 = (TGraphErrors*)f3->Get(gname.c_str())->Clone();      

      h1->SetLineColor(kBlue+2);
      h2->SetLineColor(kRed+2);
      h3->SetLineColor(kGreen+2);      

      TLegend* t1 = new TLegend( 0.4, 0.75, 0.9, 0.9 );
      t1->AddEntry( h1, "Original ScintEffSpeed, R < 5.5 m in PDF", "l" );
      //      t1->AddEntry( hNew, "Functional PDF * exp(-d/#lambda)", "l" );
      t1->AddEntry( h2, "Original ScintEffSpeed, R < 4.0 m in PDF", "l" );
      t1->AddEntry( h3, "Recoord  ScintEffSpeed, R < 5.5 m in PDF", "l" );
      t1->SetLineWidth(2);

      h1->GetYaxis()->SetRangeUser(ymin[2*i+j],ymax[2*i+j]);
      gname = coord[i] + " " + title[j];
      h1->SetTitle(gname.c_str());
      
      c1->cd(4*j + i + 1);
      
      h1->Draw("ap");
      h2->Draw("same p");
      h3->Draw("same p");
      t1->Draw("same");
      
      if(j==0){
	//	TLine *line = new TLine(h1->GetXaxis()->GetXmin(),0,h1->GetXaxis()->GetBinUpEdge(92),0);
	TLine *line = new TLine(0, 0, 6048, 0);
	//      TLine *line = new TLine(-6048,0,hOld->GetXaxis()->GetBinUpEdge(92),0);
	line->Draw("same");
      }
      else{
	h1->GetYaxis()->SetTitleOffset(1.1);
      }
    }
  }
}


void Compare4RFits(){

  gStyle->SetFrameLineWidth(2);

  std::string coord[4] = {"x", "y", "z", "r"};
  std::string title[2] = {"bias", "resolution"};
  double ymax[8] = {30, 200, 30, 200, 120, 200, 100, 200};
  double ymin[8] = {-30, 0, -30, 0, -120, 0, -20, 0};

  TFile *f1 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Apr15_SEV_Interp_2.5MeV_Z_Rlt6m.root");
  TFile *f2 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/file_2p5_dimz.root");
  TFile *f3 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Jun28_FitPerf_1to10MeV_1.5Terecoord_E.root");
  TFile *f4 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Jun28_FitPerf_1to10MeV_2.5Terecoord_E.root");

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,700);
  c1->Divide(4,2,0.01,0.05);
  c1->SetGrid(1);
  gPad->SetGrid();
  c1->Print("Comp2.5MeV_Z2.2.pdf[");

  for(int j=0; j<2; j++){
    
    for(int i=0; i<4; i++){
      
      std::string gname = coord[i] + "_" + title[j];
      std::cout << "getting " << gname << " " << 3*j+i+1 << std::endl;
      
      TGraphErrors* h1 = (TGraphErrors*)f1->Get(gname.c_str())->Clone();
      TGraphErrors* h2 = (TGraphErrors*)f2->Get(gname.c_str())->Clone();
      TGraphErrors* h3 = (TGraphErrors*)f3->Get(gname.c_str())->Clone();      
      TGraphErrors* h4 = (TGraphErrors*)f4->Get(gname.c_str())->Clone();

      h1->SetLineColor(kRed+2);
      h1->SetMarkerSize(2);
      h2->SetLineColor(kBlue+2);
      h2->SetMarkerSize(2);
      h3->SetLineColor(kGreen+2);
      h3->SetMarkerSize(2);
      h4->SetLineColor(kBlack);
      h4->SetMarkerSize(2);

      TLegend* t1 = new TLegend( 0.4, 0.65, 0.9, 0.9 );
      t1->AddEntry( h1, "Original", "l" );
      t1->AddEntry( h2, "Recoordinated", "l" );
      //t1->AddEntry( h3, "1.5% Te Loaded", "l" );
      //t1->AddEntry( h4, "2.5% Te Loaded", "l" );
      t1->SetLineWidth(2);

      h1->GetYaxis()->SetRangeUser(ymin[2*i+j],ymax[2*i+j]);
      gname = coord[i] + " " + title[j];
      h1->SetTitle(gname.c_str());

      c1->cd(4*j + i + 1);

      h1->Draw("ap");
      h2->Draw("same p");
      //h3->Draw("same p");
      //h4->Draw("same p");
      t1->Draw("same");
      
      if(j==0){
	//	TLine *line = new TLine(h1->GetXaxis()->GetXmin(),0,h1->GetXaxis()->GetBinUpEdge(92),0);
	//TLine *line = new TLine(1, 0, 10, 0);
	TLine *line = new TLine(-6048,0,6048,0);
	//TLine *line = new TLine(0,0,6048,0);
	line->Draw("same");
      }
      h1->GetYaxis()->SetTitleOffset(1.2);
    }
  }
  c1->Print("Comp2.5MeV_Z2.2.pdf");
  c1->Print("Comp2.5MeV_Z2.2.pdf]");
}
