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


void Compare3Fitters(){

  TFile *_fileScint = TFile::Open("scint100Z.root");
  TFile *_fileAll = TFile::Open("all100Z.root");
  TFile *_file2Z = TFile::Open("2zsplit100Z.root");

  TGraphErrors* hScint = (TGraphErrors*)_fileScint->Get("z_bias")->Clone();
  TGraphErrors* hAll = (TGraphErrors*)_fileAll->Get("z_bias")->Clone();
  TGraphErrors* h2Z = (TGraphErrors*)_file2Z->Get("z_bias")->Clone();

  hScint->SetLineColor(kRed);
  hAll->SetLineColor(kBlack);
  h2Z->SetLineColor(kBlue);

  TLegend* t1 = new TLegend( 0.6, 0.7, 0.9, 0.9 );
  t1->AddEntry( hScint, "ScintFitter", "l" );
  t1->AddEntry( hAll, "1 PDF", "l" );
  t1->AddEntry( h2Z, "2 PDFs", "l" );

  hScint->GetYaxis()->SetRangeUser(-600,600);
  //hAbove->GetXaxis()->SetTitle("Hit time residuals [ns]");
  //hAbove->GetYaxis()->SetTitle("Normalised Counts");

  hScint->Draw();
  hAll->Draw("same");
  h2Z->Draw("same");
  t1->Draw("same");

}

void Compare4Fitters(){

  //  TCanvas c1("c1", "c1", 1024, 768);

  std::string title = "Electrons Simulated in Whole Scintillator";
  //  std::string title = "Electrons with R > 1.5m";
  //std::string title = "Electrons with R < 1.5m";

  //  std::string name = "Rgt1.5_RecoordBiases1000.png";
  //  std::string name = "Rlt1.5_RecoordBiases1000.png";
  std::string name = "AllR_RecoordBiases1000.png";

  TFile *_fileScint = TFile::Open("scintFitter.root");
  TFile *_fileAll = TFile::Open("partialTestFitter_1Z.root");
  TFile *_file2Z = TFile::Open("partialTestFitter_2Z.root");
  TFile *_file4Z = TFile::Open("partialTestFitter_4Z.root");

  TGraphErrors* hScint = (TGraphErrors*)_fileScint->Get("z_bias")->Clone();
  TGraphErrors* hAll = (TGraphErrors*)_fileAll->Get("z_bias")->Clone();
  TGraphErrors* h2Z = (TGraphErrors*)_file2Z->Get("z_bias")->Clone();
  TGraphErrors* h4Z = (TGraphErrors*)_file4Z->Get("z_bias")->Clone();

  hScint->SetLineColor(kRed);
  hAll->SetLineColor(kBlack);
  h2Z->SetLineColor(kBlue);
  h4Z->SetLineColor(kGreen);
  
  TLegend* t1 = new TLegend( 0.6, 0.7, 0.9, 0.9 );
  t1->AddEntry( hScint, "ScintFitter", "l" );
  t1->AddEntry( hAll, "1 PDF", "l" );
  t1->AddEntry( h2Z, "2 PDFs", "l" );
  t1->AddEntry( h4Z, "4 PDFs", "l" );

  hScint->GetYaxis()->SetRangeUser(-600,600);
  //hAbove->GetXaxis()->SetTitle("Hit time residuals [ns]");
  //hAbove->GetYaxis()->SetTitle("Normalised Counts");
  hScint->SetTitle(title.c_str());

  hScint->Draw("AP");
  hAll->Draw("same P");
  h2Z->Draw("same P");
  h4Z->Draw("same P");
  t1->Draw("same");

  TLine *line = new TLine(hScint->GetXaxis()->GetXmax(),0,hScint->GetXaxis()->GetXmin(),0);
  line->Draw("same");

  //  c1.SaveAs(name.c_str());
}

void Compare4Resolutions(){

  //  TCanvas c1("c1", "c1", 1024, 768);

  std::string title = "Electrons Simulated in Whole Scintillator";
  //std::string title = "Electrons with R > 1.5m";
  //std::string title = "Electrons with R < 1.5m";

  //std::string name = "Rgt1.5_RecoordRes1000.png";
  //  std::string name = "Rlt1.5_RecoordRes1000.png";
  std::string name = "AllR_RecoordRes1000.png";

  TFile *_fileScint = TFile::Open("scintFitter.root");
  TFile *_fileAll = TFile::Open("partialTestFitter_1Z.root");
  TFile *_file2Z = TFile::Open("partialTestFitter_2Z.root");
  TFile *_file4Z = TFile::Open("partialTestFitter_4Z.root");

  TGraphErrors* hScint = (TGraphErrors*)_fileScint->Get("z_resolution")->Clone();
  TGraphErrors* hAll = (TGraphErrors*)_fileAll->Get("z_resolution")->Clone();
  TGraphErrors* h2Z = (TGraphErrors*)_file2Z->Get("z_resolution")->Clone();
  TGraphErrors* h4Z = (TGraphErrors*)_file4Z->Get("z_resolution")->Clone();

  hScint->SetLineColor(kRed);
  hAll->SetLineColor(kBlack);
  h2Z->SetLineColor(kBlue);
  h4Z->SetLineColor(kGreen);

  TLegend* t1 = new TLegend( 0.6, 0.7, 0.9, 0.9 );
  t1->AddEntry( hScint, "ScintFitter", "l" );
  t1->AddEntry( hAll, "1 PDF", "l" );
  t1->AddEntry( h2Z, "2 PDFs", "l" );
  t1->AddEntry( h4Z, "4 PDFs", "l" );

  hScint->GetYaxis()->SetRangeUser(0,150);
  //hAbove->GetXaxis()->SetTitle("Hit time residuals [ns]");
  //hAbove->GetYaxis()->SetTitle("Normalised Counts");
  hScint->SetTitle(title.c_str());

  hScint->Draw("AP");
  hAll->Draw("same P");
  h2Z->Draw("same P");
  h4Z->Draw("same P");
  t1->Draw("same AP");

  TLine *line = new TLine(hScint->GetXaxis()->GetXmax(),0,hScint->GetXaxis()->GetXmin(),0);
  line->Draw("same");

   //c1.SaveAs(name.c_str());
}

void Compare5Fitters(){

  //  TCanvas c1("c1", "c1", 1024, 768);

  std::string title = "Bias in Z";
  //  std::string title = "Electrons with R > 1.5m";
  //std::string title = "Electrons with R < 1.5m";

  //  std::string name = "Rgt1.5_RecoordBiases1000.png";
  //  std::string name = "Rlt1.5_RecoordBiases1000.png";
  std::string name = "AllR_RecoordBiases1000.png";

  //TFile *_fileScint = TFile::Open("scintFitterY_1000e.root");
  //TFile *_fileAll = TFile::Open("partialTestFitterY_1Z_1000e.root");
  //TFile *_file2Z = TFile::Open("partialTestFitterY_2Z_1000e.root");
  //TFile *_file4Z = TFile::Open("partialTestFitterY_4Z_1000e.root");
  //TFile *_file8PDFs = TFile::Open("partialTestFitterY_8PDFs_1000e.root");

  TFile *_fileScint = TFile::Open("testInput_8PDFsResR400.root");
  TFile *_fileAll = TFile::Open("testInput_1PDFsResR400.root");
  TFile *_file2Z = TFile::Open("testInput_2PDFsResR400.root");
  TFile *_file4Z = TFile::Open("testInput_4PDFsResR400.root");
  TFile *_file8PDFs = TFile::Open("refactored_8PDFs.root");

  TGraphErrors* hScint = (TGraphErrors*)_fileScint->Get("z_bias")->Clone();
  TGraphErrors* hAll = (TGraphErrors*)_fileAll->Get("z_bias")->Clone();
  TGraphErrors* h2Z = (TGraphErrors*)_file2Z->Get("z_bias")->Clone();
  TGraphErrors* h4Z = (TGraphErrors*)_file4Z->Get("z_bias")->Clone();
  TGraphErrors* h8PDFs = (TGraphErrors*)_file8PDFs->Get("z_bias")->Clone();

  hScint->SetLineColor(kRed);
  hAll->SetLineColor(kBlack);
  h2Z->SetLineColor(kBlue);
  h4Z->SetLineColor(kGreen);
  h8PDFs->SetLineColor(kMagenta);

  TLegend* t1 = new TLegend( 0.6, 0.7, 0.9, 0.9 );
  //t1->AddEntry( hScint, "ScintFitter", "l" );
  t1->AddEntry( hAll, "1 PDF", "l" );
  t1->AddEntry( h2Z, "2 PDFs", "l" );
  t1->AddEntry( h4Z, "4 PDFs", "l" );
  t1->AddEntry( h8PDFs, "8 PDFs", "l" );

  hAll->GetYaxis()->SetRangeUser(-25,25);
  //hAbove->GetXaxis()->SetTitle("Hit time residuals [ns]");
  //hAbove->GetYaxis()->SetTitle("Normalised Counts");
  hAll->SetTitle(title.c_str());
  //  hAll->GetXaxis()->SetTitle("x [mm]");

  //  hScint->Draw("AP");
  hAll->Draw("AP");
  h2Z->Draw("same P");
  h4Z->Draw("same P");
  h8PDFs->Draw("same P");
  t1->Draw("same");
  
  //  TLine *line = new TLine(hAll->GetXaxis()->GetXmax(),0,hAll->GetXaxis()->GetXmin(),0);
  TLine *line = new TLine(200,0,6050,0);
  line->Draw("same");

  //  c1.SaveAs(name.c_str());
}

void Compare5Resolutions(){

  std::string title = "Resolution in Z";
  //  std::string title = "Electrons with R > 1.5m";
  //std::string title = "Electrons with R < 1.5m";

  //  std::string name = "Rgt1.5_RecoordBiases1000.png";
  //  std::string name = "Rlt1.5_RecoordBiases1000.png";
  std::string name = "AllR_RecoordBiases1000.png";

  //  TFile *_fileScint = TFile::Open("scintFitter_1000e.root");
  //TFile *_fileAll = TFile::Open("partialTestFitter_1Z_1000e.root");
  //TFile *_file2Z = TFile::Open("partialTestFitter_2Z_1000e.root");
  //TFile *_file4Z = TFile::Open("partialTestFitter_4Z_1000e.root");
  //TFile *_file8PDFs = TFile::Open("partialTestFitter_8PDFs_1000e.root");
  TFile *_fileScint = TFile::Open("testInput_1PDFsResR400.root");
  TFile *_fileAll = TFile::Open("testInput_1PDFsResR400.root");
  TFile *_file2Z = TFile::Open("testInput_2PDFsResR400.root");
  TFile *_file4Z = TFile::Open("testInput_4PDFsResR400.root");
  TFile *_file8PDFs = TFile::Open("refactored_8PDFs.root");

  TGraphErrors* hScint = (TGraphErrors*)_fileScint->Get("z_resolution")->Clone();
  TGraphErrors* hAll = (TGraphErrors*)_fileAll->Get("z_resolution")->Clone();
  TGraphErrors* h2Z = (TGraphErrors*)_file2Z->Get("z_resolution")->Clone();
  TGraphErrors* h4Z = (TGraphErrors*)_file4Z->Get("z_resolution")->Clone();
  TGraphErrors* h8PDFs = (TGraphErrors*)_file8PDFs->Get("z_resolution")->Clone();

  //  hScint->SetLineColor(kRed);
  hAll->SetLineColor(kBlack);
  h2Z->SetLineColor(kBlue);
  h4Z->SetLineColor(kGreen);
  h8PDFs->SetLineColor(kMagenta);

  TLegend* t1 = new TLegend( 0.6, 0.7, 0.9, 0.9 );
  //  t1->AddEntry( hScint, "ScintFitter", "l" );
  t1->AddEntry( hAll, "1 PDF", "l" );
  t1->AddEntry( h2Z, "2 PDFs", "l" );
  t1->AddEntry( h4Z, "4 PDFs", "l" );
  t1->AddEntry( h8PDFs, "8 PDFs", "l" );

  hAll->GetYaxis()->SetRangeUser(40,120);
  //  hAll->GetXaxis()->SetRangeUser(-6000,6000);
  //hAbove->GetXaxis()->SetTitle("Hit time residuals [ns]");
  //hAbove->GetYaxis()->SetTitle("Normalised Counts");
  hAll->SetTitle(title.c_str());
  //  hAll->GetXaxis()->SetTitle("x [mm]");

  // hScint->Draw("AP");
  hAll->Draw("AP");
  h2Z->Draw("same P");
  h4Z->Draw("same P");
  h8PDFs->Draw("same P");
  t1->Draw("same");

  TLine *line = new TLine(300,0,6050,0);
  line->Draw("same");

  //  c1.SaveAs(name.c_str());
}


void Compare5Efficiency(){

  std::string title = "Fit Efficiency";

  TFile *_fileAll = TFile::Open("testInput_1PDFsResR400.root");
  TFile *_file2Z = TFile::Open("testInput_2PDFsResR400.root");
  TFile *_file4Z = TFile::Open("testInput_4PDFsResR400.root");
  TFile *_file8PDFs = TFile::Open("debug_8PDFs_AllFiles.root");

  TGraphErrors* hAll = (TGraphErrors*)_fileAll->Get("fitExistsEfficiency")->Clone();
  TGraphErrors* h2Z = (TGraphErrors*)_file2Z->Get("fitExistsEfficiency")->Clone();
  TGraphErrors* h4Z = (TGraphErrors*)_file4Z->Get("fitExistsEfficiency")->Clone();
  TGraphErrors* h8PDFs = (TGraphErrors*)_file8PDFs->Get("fitExistsEfficiency")->Clone();

  hAll->SetLineColor(kBlack);
  h2Z->SetLineColor(kBlue);
  h4Z->SetLineColor(kGreen);
  h8PDFs->SetLineColor(kMagenta);

  TLegend* t1 = new TLegend( 0.6, 0.1, 0.9, 0.3 );
  t1->AddEntry( hAll, "1 PDF", "l" );
  t1->AddEntry( h2Z, "2 PDFs", "l" );
  t1->AddEntry( h4Z, "4 PDFs", "l" );
  t1->AddEntry( h8PDFs, "8 PDFs", "l" );

  //  hAll->GetYaxis()->SetRangeUser(0,1.5);
  hAll->SetTitle(title.c_str());

  hAll->Draw("");
  h2Z->Draw("same");
  h4Z->Draw("same");
  h8PDFs->Draw("same");
  t1->Draw("same");
  
}


void Compare2Fitters(){

  std::string title = "Bias in Z";

  TFile *_Old = TFile::Open("Nov9_multipdf_E0.root");
  TFile *_New = TFile::Open("Nov9_multipath_E0.root");

  TGraphErrors* hOld = (TGraphErrors*)_Old->Get("z_bias")->Clone();
  TGraphErrors* hNew = (TGraphErrors*)_New->Get("z_bias")->Clone();

  hOld->SetLineColor(kRed);
  hNew->SetLineColor(kBlue);

  TLegend* t1 = new TLegend( 0.6, 0.7, 0.9, 0.9 );
  t1->AddEntry( hOld, "MultiPDF", "l" );
  t1->AddEntry( hNew, "MultiPath", "l" );
 
  hOld->GetYaxis()->SetRangeUser(-50,50);
  hOld->SetTitle(title.c_str());
  
  hOld->Draw("AP");
  hNew->Draw("same P");
  t1->Draw("same");

  // TLine *line = new TLine(200,0,6050,0);
  TLine *line = new TLine(0,0,10,0);
  line->Draw("same");

}



void Compare2Resolutions(){

  std::string coord[3] = {"x", "y", "z"};

  std::string title = "Resolution in ";

  TFile *_Old = TFile::Open("Nov17Test_multipdf.root");
  TFile *_New = TFile::Open("Nov19_multipdf_R.root");

  for(int i=0; i<3; i++){

    std::string 

  TGraphErrors* hOld = (TGraphErrors*)_Old->Get("z_resolution")->Clone();
  TGraphErrors* hNew = (TGraphErrors*)_New->Get("z_resolution")->Clone();
  
  hOld->SetLineColor(kRed);
  hNew->SetLineColor(kBlue);

  TLegend* t1 = new TLegend( 0.6, 0.7, 0.9, 0.9 );
  t1->AddEntry( hOld, "MultiPDF", "l" );
  t1->AddEntry( hNew, "MultiPath", "l" );
  
  hOld->GetYaxis()->SetRangeUser(80,160);
  hOld->SetTitle(title.c_str());
  
  hOld->Draw("AP");
  hNew->Draw("same P");
  t1->Draw("same");
  
  //  TLine *line = new TLine(300,0,6050,0);
  //line->Draw("same");
  
}

void Compare3Biases(){

  std::string title = "Bias in Z";

  TFile *_1 = TFile::Open("Nov3_0p5_AllR_QuadSeed.root");
  TFile *_2 = TFile::Open("Nov3_0p5_AllR_4PDFBound.root");
  TFile *_3 = TFile::Open("Nov3_0p5_AllR_4PDFBound50cm.root");
  TFile *_4 = TFile::Open("Nov3_0p5_AllR_4PDFSwitch.root");

  //TFile *_1 = TFile::Open("Nov3_0p5_AllR_QuadSeed_Z.root");
  //TFile *_2 = TFile::Open("Nov3_0p5_AllR_4PDFBound_Z.root");
  //TFile *_3 = TFile::Open("Nov3_0p5_AllR_4PDFBound50cm_Z.root");
  //TFile *_4 = TFile::Open("Nov3_0p5_AllR_4PDFSwitch_Z.root");

  TGraphErrors* h1 = (TGraphErrors*)_1->Get("z_bias")->Clone();
  TGraphErrors* h2 = (TGraphErrors*)_2->Get("z_bias")->Clone();
  TGraphErrors* h3 = (TGraphErrors*)_3->Get("z_bias")->Clone();
  TGraphErrors* h4 = (TGraphErrors*)_4->Get("z_bias")->Clone();

  h1->SetLineColor(kRed);
  h2->SetLineColor(kBlue);
  h3->SetLineColor(kMagenta);
  h4->SetLineColor(kGreen+2);

  TLegend* t1 = new TLegend( 0.3, 0.6, 0.9, 0.9 );
  //  t1->AddEntry( h1, "Only Pos.-Dep. Result", "l" );
  t1->AddEntry( h2, "Non-Pos.-Dep. Result within 1m of Boundary", "l" );
  t1->AddEntry( h3, "Non-Pos.-Dep. Result within 0.5m of Boundary", "l" );
  //t1->AddEntry( h4, "Non-Pos.-Dep. Result if Crosses Boundary", "l" );

  h1->GetYaxis()->SetRangeUser(-10,20);
  h1->SetTitle(title.c_str());

  //  h1->Draw("AP");
  h2->Draw("AP");
  h3->Draw("same P");
  //h4->Draw("same P");
  t1->Draw("same");

  TLine *line = new TLine(300,0,6050,0);
  line->Draw("same");

}

void Compare3Resolutions(){

  std::string title = "Resolution in Z";

  TFile *_1 = TFile::Open("Nov3_0p5_AllR_QuadSeed.root");
  TFile *_2 = TFile::Open("Nov3_0p5_AllR_4PDFBound.root");
  TFile *_3 = TFile::Open("Nov3_0p5_AllR_4PDFBound50cm.root");
  TFile *_4 = TFile::Open("Nov3_0p5_AllR_4PDFSwitch.root");

  //TFile *_1 = TFile::Open("Nov3_0p5_AllR_QuadSeed_Z.root");
  //TFile *_2 = TFile::Open("Nov3_0p5_AllR_4PDFBound_Z.root");
  //TFile *_3 = TFile::Open("Nov3_0p5_AllR_4PDFBound50cm_Z.root");
  //TFile *_4 = TFile::Open("Nov3_0p5_AllR_4PDFSwitch_Z.root");

  TGraphErrors* h1 = (TGraphErrors*)_1->Get("z_resolution")->Clone();
  TGraphErrors* h2 = (TGraphErrors*)_2->Get("z_resolution")->Clone();
  TGraphErrors* h3 = (TGraphErrors*)_3->Get("z_resolution")->Clone();
  TGraphErrors* h4 = (TGraphErrors*)_4->Get("z_resolution")->Clone();

  h1->SetLineColor(kRed);
  h2->SetLineColor(kBlue);
  h3->SetLineColor(kMagenta);
  h4->SetLineColor(kGreen+2);

  TLegend* t1 = new TLegend( 0.3, 0.6, 0.9, 0.9 );
  //t1->AddEntry( h1, "Only Pos.-Dep. Result", "l" );
  t1->AddEntry( h2, "Non-Pos.-Dep. Result within 1m of Boundary", "l" );
  t1->AddEntry( h3, "Non-Pos.-Dep. Result within 0.5m of Boundary", "l" );
  //  t1->AddEntry( h4, "Non-Pos.-Dep. Result if Crosses Boundary", "l" );

  h1->GetYaxis()->SetRangeUser(80,160);
  h1->SetTitle(title.c_str());

  //h1->Draw("AP");
  h2->Draw("AP");
  h3->Draw("same P");
  //h4->Draw("same P");
  t1->Draw("same");

  TLine *line = new TLine(300,0,6050,0);
  line->Draw("same");

}
