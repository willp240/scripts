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

#include <string>


void CompareBiasEnergies(){

  //  TCanvas c_zbias("c1", "c1", 1024, 768);

  std::string title = "Bias in Z";

  TFile *_file0 = TFile::Open("Nov6_multipdf_0p5MeVAllR.root");
  TFile *_file1 = TFile::Open("Nov6_multipdf_1p5MeVAllR.root");
  TFile *_file2 = TFile::Open("Nov6_multipdf_2p5MeVAllR.root");
  TFile *_file3 = TFile::Open("Nov6_multipdf_3p5MeVAllR.root");
  TFile *_file4 = TFile::Open("Nov6_multipdf_4p5MeVAllR.root");
  TFile *_file5 = TFile::Open("Nov6_multipdf_5p5MeVAllR.root");

  TGraphErrors* h0 = (TGraphErrors*)_file0->Get("z_bias")->Clone();
  TGraphErrors* h1 = (TGraphErrors*)_file1->Get("z_bias")->Clone();
  TGraphErrors* h2 = (TGraphErrors*)_file2->Get("z_bias")->Clone();
  TGraphErrors* h3 = (TGraphErrors*)_file3->Get("z_bias")->Clone();
  TGraphErrors* h4 = (TGraphErrors*)_file4->Get("z_bias")->Clone();
  TGraphErrors* h5 = (TGraphErrors*)_file5->Get("z_bias")->Clone();

  h0->SetLineColor(kRed);
  h1->SetLineColor(kMagenta);
  h2->SetLineColor(kGreen);
  h3->SetLineColor(kCyan);
  h4->SetLineColor(kBlue);
  h5->SetLineColor(kBlack);

  TLegend* t1 = new TLegend( 0.6, 0.7, 0.9, 0.9 );
  t1->SetNColumns(2);
  t1->AddEntry( h0, "0.5 MeV", "l" );
  t1->AddEntry( h1, "1.5 MeV", "l" );
  t1->AddEntry( h2, "2.5 MeV", "l" );
  t1->AddEntry( h3, "3.5 MeV", "l" );
  t1->AddEntry( h4, "4.5 MeV", "l" );
  t1->AddEntry( h5, "5.5 MeV", "l" );
 
  h0->GetYaxis()->SetRangeUser(-50,100);
  h0->SetTitle(title.c_str());
  
  h0->Draw("AP");
  h1->Draw("same P");
  h2->Draw("same P");
  h3->Draw("same P");
  h4->Draw("same P");
  h5->Draw("same P");
  t1->Draw("same");

  TLine *line = new TLine(200,0,6050,0);
  line->Draw("same");

}
 
void CompareResEnergies(){

  //  TCanvas c_zbias("c1", "c1", 1024, 768);

  std::string title = "Resolution in Z";

  TFile *_file0 = TFile::Open("Nov6_multipdf_0p5MeVAllR.root");
  TFile *_file1 = TFile::Open("Nov6_multipdf_1p5MeVAllR.root");
  TFile *_file2 = TFile::Open("Nov6_multipdf_2p5MeVAllR.root");
  TFile *_file3 = TFile::Open("Nov6_multipdf_3p5MeVAllR.root");
  TFile *_file4 = TFile::Open("Nov6_multipdf_4p5MeVAllR.root");
  TFile *_file5 = TFile::Open("Nov6_multipdf_5p5MeVAllR.root");

  TGraphErrors* h0 = (TGraphErrors*)_file0->Get("z_resolution")->Clone();
  TGraphErrors* h1 = (TGraphErrors*)_file1->Get("z_resolution")->Clone();
  TGraphErrors* h2 = (TGraphErrors*)_file2->Get("z_resolution")->Clone();
  TGraphErrors* h3 = (TGraphErrors*)_file3->Get("z_resolution")->Clone();
  TGraphErrors* h4 = (TGraphErrors*)_file4->Get("z_resolution")->Clone();
  TGraphErrors* h5 = (TGraphErrors*)_file5->Get("z_resolution")->Clone();

  TLegend* t1 = new TLegend( 0.6, 0.7, 0.9, 0.9 );
  t1->SetNColumns(2);
  t1->AddEntry( h0, "0.5 MeV", "l" );
  t1->AddEntry( h1, "1.5 MeV", "l" );
  t1->AddEntry( h2, "2.5 MeV", "l" );
  t1->AddEntry( h3, "3.5 MeV", "l" );
  t1->AddEntry( h4, "4.5 MeV", "l" );
  t1->AddEntry( h5, "5.5 MeV", "l" );

  h0->SetLineColor(kRed);
  h1->SetLineColor(kMagenta);
  h2->SetLineColor(kGreen);
  h3->SetLineColor(kCyan);
  h4->SetLineColor(kBlue);
  h5->SetLineColor(kBlack);

  h0->GetYaxis()->SetRangeUser(110,240);
  h0->SetTitle(title.c_str());

  h0->Draw("AP");
  h1->Draw("same P");
  h2->Draw("same P");
  h3->Draw("same P");
  h4->Draw("same P");
  h5->Draw("same P");
  t1->Draw("same");

}

void VarBiasEnergies(){

  //  TCanvas c_zbias("c1", "c1", 1024, 768);

  std::string title = "Biases as Function of R";

  TFile *_file0 = TFile::Open("./Nov6_multipdf_0to10MeVAllR.root");

  TGraphErrors* h0 = (TGraphErrors*)_file0->Get("x_bias")->Clone();
  TGraphErrors* h1 = (TGraphErrors*)_file0->Get("y_bias")->Clone();
  TGraphErrors* h2 = (TGraphErrors*)_file0->Get("z_bias")->Clone();

  h0->SetLineColor(kBlue);
  h1->SetLineColor(kRed);
  h2->SetLineColor(kGreen+2);

  TLegend* t1 = new TLegend( 0.7, 0.8, 0.9, 0.9 );
  t1->SetNColumns(3);
  t1->AddEntry( h0, "X", "l" );
  t1->AddEntry( h1, "Y", "l" );
  t1->AddEntry( h2, "Z", "l" );


  h0->GetYaxis()->SetRangeUser(-30,50);
  h0->SetTitle(title.c_str());

  h0->Draw("AP");
  h1->Draw("same P");
  h2->Draw("same P");
  t1->Draw("same");

  //TLine *line = new TLine(0.95,0,10,0);
  TLine *line = new TLine(200,0,6050,0);
  line->Draw("same");
}

void VarResEnergies(){

  //  TCanvas c_zbias("c1", "c1", 1024, 768);

  std::string title = "Resolutions as Function of R";

  TFile *_file0 = TFile::Open("./Nov6_multipdf_0to10MeVAllR.root");

  TGraphErrors* h0 = (TGraphErrors*)_file0->Get("x_resolution")->Clone();
  TGraphErrors* h1 = (TGraphErrors*)_file0->Get("y_resolution")->Clone();
  TGraphErrors* h2 = (TGraphErrors*)_file0->Get("z_resolution")->Clone();

  h0->SetLineColor(kBlue);
  h1->SetLineColor(kRed);
  h2->SetLineColor(kGreen+2);

  TLegend* t1 = new TLegend( 0.7, 0.8, 0.9, 0.9 );
  t1->SetNColumns(3);
  t1->AddEntry( h0, "X", "l" );
  t1->AddEntry( h1, "Y", "l" );
  t1->AddEntry( h2, "Z", "l" );


  h0->GetYaxis()->SetRangeUser(80,160);
  h0->SetTitle(title.c_str());

  h0->Draw("AP");
  h1->Draw("same P");
  h2->Draw("same P");
  t1->Draw("same");

}
