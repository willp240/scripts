#include <TH1D.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"
#include "TTree.h"
#include <algorithm>
#include <cctype>
#include <string>

#include <string>


void PlotXYZRComp(){
  
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TFile *file1 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Dec12_1000to2000mm_Tree.root");
  TTree* tree1 = (TTree*)file1->Get("eveTree")->Clone("tree");

  TFile *file2 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Dec12_2000to3000mm_Tree.root");
  TTree* tree2 = (TTree*)file2->Get("eveTree")->Clone("tree2");

  TFile *file3 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Dec12_3000to4000mm_Tree.root");
  TTree* tree3 = (TTree*)file3->Get("eveTree")->Clone("tree3");

  TFile *file4 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Dec12_4000to5000mm_Tree.root");
  TTree* tree4 = (TTree*)file4->Get("eveTree")->Clone("tree4");

  TFile *file5 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Dec12_5000to6000mm_Tree.root");
  TTree* tree5 = (TTree*)file5->Get("eveTree")->Clone("tree5");


  ////////////////////////////////////////////////////////////////////////////////

  TH1F *hx = new TH1F("hx", "X Bias", 100, -1000, 1000);
  TH1F *hx2 = new TH1F("hx2", "hx2", 100, -1000, 1000);
  TH1F *hx3 = new TH1F("hx3", "hx3", 100, -1000, 1000);
  TH1F *hx4 = new TH1F("hx4", "hx4", 100, -1000, 1000);
  TH1F *hx5 = new TH1F("hx5", "hx5", 100, -1000, 1000);

  TCanvas* cx = new TCanvas("cx", "cx", 800,600);
  //c1->SetLeftMargin(0.13);
  cx->SetGrid();

  tree1->Draw(" xFit - xTrue  >> hx");
  hx->GetXaxis()->SetTitle("Fit - Truth X, mm");
  hx->GetYaxis()->SetTitle("Events");
  hx->GetXaxis()->SetRangeUser(-500,500);
  hx->GetYaxis()->SetRangeUser(0,200);
  hx->SetLineColor(kBlack);
  hx->SetLineWidth(2);
  hx->GetYaxis()->SetTitleOffset(1.4);
  TF1  *fx1 = new TF1("fx1","gaus");
  fx1->SetLineColor(kBlack);
  hx->Fit("fx1","0");

  tree2->Draw(" xFit - xTrue >> hx2","","same");
  hx2->GetXaxis()->SetTitle("Fit - Truth X, mm");
  hx2->GetYaxis()->SetTitle("Events");
  hx2->GetXaxis()->SetRangeUser(-500,500);
  hx2->SetLineColor(kRed+2);
  hx2->SetLineWidth(2);
  TF1  *fx2 = new TF1("fx2","gaus");
  fx2->SetLineColor(kRed+2);
  hx2->Fit("fx2","0");

  tree3->Draw(" xFit - xTrue  >> hx3");
  hx3->GetXaxis()->SetTitle("Fit - Truth X, mm");
  hx3->GetYaxis()->SetTitle("Events");
  hx3->GetXaxis()->SetRangeUser(-500,500);
  hx3->SetLineColor(kMagenta+2);
  hx3->SetLineWidth(2);
  TF1  *fx3 = new TF1("fx3","gaus");
  fx3->SetLineColor(kMagenta+2);
  hx3->Fit("fx3","0");

  tree4->Draw(" xFit - xTrue >> hx4","","same");
  hx4->GetXaxis()->SetTitle("Fit - Truth X, mm");
  hx4->GetYaxis()->SetTitle("Events");
  hx4->GetXaxis()->SetRangeUser(-500,500);
  hx4->SetLineColor(kBlue+2);
  hx4->SetLineWidth(2);
  TF1  *fx4 = new TF1("fx4","gaus");
  fx4->SetLineColor(kBlue+2);
  hx4->Fit("fx4","0");

  tree5->Draw(" xFit - xTrue >> hx5","","same");
  hx5->GetXaxis()->SetTitle("Fit - Truth X, mm");
  hx5->GetYaxis()->SetTitle("Events");
  hx5->GetXaxis()->SetRangeUser(-500,500);
  hx5->SetLineColor(kGreen+2);
  hx5->SetLineWidth(2);
  TF1  *fx5 = new TF1("fx5","gaus");
  fx5->SetLineColor(kGreen+2);
  hx5->Fit("fx5","0");

  hx->Draw("");
  hx2->Draw("same");
  hx3->Draw("same");
  hx4->Draw("same");
  hx5->Draw("same");

  TLegend* tx = new TLegend( 0.14, 0.5, 0.39, 0.8 );
  tx->AddEntry( hx, Form("0-2 m, Mean %.2f mm", fx1->GetParameter(1)), "l" );
  tx->AddEntry( hx2, Form("2-3 m, Mean %.2f mm", fx2->GetParameter(1)), "l" );
  tx->AddEntry( hx3, Form("3-4 m, Mean %.2f mm", fx3->GetParameter(1)), "l" );
  tx->AddEntry( hx4, Form("4-5 m, Mean %.2f mm", fx4->GetParameter(1)), "l" );
  tx->AddEntry( hx5, Form("5-6 m, Mean %.2f mm", fx5->GetParameter(1)), "l");
  tx->SetLineWidth(2);
  tx->Draw("same");

  cx->SaveAs("Dec12_bands_x.pdf");


  ////////////////////////////////////////////////////////////////////////////////

  TH1F *hy = new TH1F("hy", "Y Bias", 100, -1000, 1000);
  TH1F *hy2 = new TH1F("hy2", "hy2", 100, -1000, 1000);
  TH1F *hy3 = new TH1F("hy3", "hy3", 100, -1000, 1000);
  TH1F *hy4 = new TH1F("hy4", "hy4", 100, -1000, 1000);
  TH1F *hy5 = new TH1F("hy5", "hy5", 100, -1000, 1000);

  TCanvas* cy = new TCanvas("cy", "cy", 800,600);
  //c1->SetLeftMargin(0.13);
  cy->SetGrid();

  tree1->Draw(" yFit - yTrue  >> hy");
  hy->GetXaxis()->SetTitle("Fit - Truth Y, mm");
  hy->GetYaxis()->SetTitle("Events");
  hy->GetXaxis()->SetRangeUser(-500,500);
  hy->GetYaxis()->SetRangeUser(0,200);
  hy->SetLineColor(kBlack);
  hy->SetLineWidth(2);
  hy->GetYaxis()->SetTitleOffset(1.4);
  TF1  *fy1 = new TF1("fy1","gaus");
  fy1->SetLineColor(kBlack);
  hy->Fit("fy1","0");

  tree2->Draw(" yFit - yTrue >> hy2","","same");
  hy2->GetXaxis()->SetTitle("Fit - Truth Y, mm");
  hy2->GetYaxis()->SetTitle("Events");
  hy2->GetXaxis()->SetRangeUser(-500,500);
  hy2->SetLineColor(kRed+2);
  hy2->SetLineWidth(2);
  TF1  *fy2 = new TF1("fy2","gaus");
  fy2->SetLineColor(kRed+2);
  hy2->Fit("fy2","0");

  tree3->Draw(" yFit - yTrue  >> hy3");
  hy3->GetXaxis()->SetTitle("Fit - Truth Y, mm");
  hy3->GetYaxis()->SetTitle("Events");
  hy3->GetXaxis()->SetRangeUser(-500,500);
  hy3->SetLineColor(kMagenta+2);
  hy3->SetLineWidth(2);
  TF1  *fy3 = new TF1("fy3","gaus");
  fy3->SetLineColor(kMagenta+2);
  hy3->Fit("fy3","0");

  tree4->Draw(" yFit - yTrue >> hy4","","same");
  hy4->GetXaxis()->SetTitle("Fit - Truth Y, mm");
  hy4->GetYaxis()->SetTitle("Events");
  hy4->GetXaxis()->SetRangeUser(-500,500);
  hy4->SetLineColor(kBlue+2);
  hy4->SetLineWidth(2);
  TF1  *fy4 = new TF1("fy4","gaus");
  fy4->SetLineColor(kBlue+2);
  hy4->Fit("fy4","0");

  tree5->Draw(" yFit - yTrue >> hy5","","same");
  hy5->GetXaxis()->SetTitle("Fit - Truth Y, mm");
  hy5->GetYaxis()->SetTitle("Events");
  hy5->GetXaxis()->SetRangeUser(-500,500);
  hy5->SetLineColor(kGreen+2);
  hy5->SetLineWidth(2);
  TF1  *fy5 = new TF1("fy5","gaus");
  fy5->SetLineColor(kGreen+2);
  hy5->Fit("fy5","0");

  hy->Draw("");
  hy2->Draw("same");
  hy3->Draw("same");
  hy4->Draw("same");
  hy5->Draw("same");

  TLegend* ty = new TLegend( 0.14, 0.5, 0.39, 0.8 );
  ty->AddEntry( hy, Form("0-2 m, Mean %.2f mm", fy1->GetParameter(1)), "l" );
  ty->AddEntry( hy2, Form("2-3 m, Mean %.2f mm", fy2->GetParameter(1)), "l" );
  ty->AddEntry( hy3, Form("3-4 m, Mean %.2f mm", fy3->GetParameter(1)), "l" );
  ty->AddEntry( hy4, Form("4-5 m, Mean %.2f mm", fy4->GetParameter(1)), "l" );
  ty->AddEntry( hy5, Form("5-6 m, Mean %.2f mm", fy5->GetParameter(1)), "l");
  ty->SetLineWidth(2);
  ty->Draw("same");

  cy->SaveAs("Dec12_bands_y.pdf");


  ////////////////////////////////////////////////////////////////////////////////

  TH1F *hz = new TH1F("hz", "Z Bias", 100, -1000, 1000);
  TH1F *hz2 = new TH1F("hz2", "hz2", 100, -1000, 1000);
  TH1F *hz3 = new TH1F("hz3", "hz3", 100, -1000, 1000);
  TH1F *hz4 = new TH1F("hz4", "hz4", 100, -1000, 1000);
  TH1F *hz5 = new TH1F("hz5", "hz5", 100, -1000, 1000);

  TCanvas* cz = new TCanvas("cz", "cz", 800,600);
  //c1->SetLeftMargin(0.13);
  cz->SetGrid();

  tree1->Draw(" zFit - zTrue  >> hz");
  hz->GetXaxis()->SetTitle("Fit - Truth Z, mm");
  hz->GetYaxis()->SetTitle("Events");
  hz->GetXaxis()->SetRangeUser(-500,500);
  hz->GetYaxis()->SetRangeUser(0,200);
  hz->SetLineColor(kBlack);
  hz->SetLineWidth(2);
  hz->GetYaxis()->SetTitleOffset(1.4);
  TF1  *fz1 = new TF1("fz1","gaus");
  fz1->SetLineColor(kBlack);
  hz->Fit("fz1","0");

  tree2->Draw(" zFit - zTrue >> hz2","","same");
  hz2->GetXaxis()->SetTitle("Fit - Truth Z, mm");
  hz2->GetYaxis()->SetTitle("Events");
  hz2->GetXaxis()->SetRangeUser(-500,500);
  hz2->SetLineColor(kRed+2);
  hz2->SetLineWidth(2);
  TF1  *fz2 = new TF1("fz2","gaus");
  fz2->SetLineColor(kRed+2);
  hz2->Fit("fz2","0");

  tree3->Draw(" zFit - zTrue  >> hz3");
  hz3->GetXaxis()->SetTitle("Fit - Truth Z, mm");
  hz3->GetYaxis()->SetTitle("Events");
  hz3->GetXaxis()->SetRangeUser(-500,500);
  hz3->SetLineColor(kMagenta+2);
  hz3->SetLineWidth(2);
  TF1  *fz3 = new TF1("fz3","gaus");
  fz3->SetLineColor(kMagenta+2);
  hz3->Fit("fz3","0");

  tree4->Draw(" zFit - zTrue >> hz4","","same");
  hz4->GetXaxis()->SetTitle("Fit - Truth Z, mm");
  hz4->GetYaxis()->SetTitle("Events");
  hz4->GetXaxis()->SetRangeUser(-500,500);
  hz4->SetLineColor(kBlue+2);
  hz4->SetLineWidth(2);
  TF1  *fz4 = new TF1("fz4","gaus");
  fz4->SetLineColor(kBlue+2);
  hz4->Fit("fz4","0");

  tree5->Draw(" zFit - zTrue >> hz5","","same");
  hz5->GetXaxis()->SetTitle("Fit - Truth Z, mm");
  hz5->GetYaxis()->SetTitle("Events");
  hz5->GetXaxis()->SetRangeUser(-500,500);
  hz5->SetLineColor(kGreen+2);
  hz5->SetLineWidth(2);
  TF1  *fz5 = new TF1("fz5","gaus");
  fz5->SetLineColor(kGreen+2);
  hz5->Fit("fz5","0");

  hz->Draw("");
  hz2->Draw("same");
  hz3->Draw("same");
  hz4->Draw("same");
  hz5->Draw("same");

  TLegend* tz = new TLegend( 0.14, 0.5, 0.39, 0.8 );
  tz->AddEntry( hz, Form("0-2 m, Mean %.2f mm", fz1->GetParameter(1)), "l" );
  tz->AddEntry( hz2, Form("2-3 m, Mean %.2f mm", fz2->GetParameter(1)), "l" );
  tz->AddEntry( hz3, Form("3-4 m, Mean %.2f mm", fz3->GetParameter(1)), "l" );
  tz->AddEntry( hz4, Form("4-5 m, Mean %.2f mm", fz4->GetParameter(1)), "l" );
  tz->AddEntry( hz5, Form("5-6 m, Mean %.2f mm", fz5->GetParameter(1)), "l");
  tz->SetLineWidth(2);
  tz->Draw("same");

  cz->SaveAs("Dec12_bands_z.pdf");


  ////////////////////////////////////////////////////////////////////////////////

  TH1F *hr = new TH1F("hr", "Radial Bias", 100, -1000, 1000);
  TH1F *hr2 = new TH1F("hr2", "hr2", 100, -1000, 1000);
  TH1F *hr3 = new TH1F("hr3", "hr3", 100, -1000, 1000);
  TH1F *hr4 = new TH1F("hr4", "hr4", 100, -1000, 1000);
  TH1F *hr5 = new TH1F("hr5", "hr5", 100, -1000, 1000);

  TCanvas* cr = new TCanvas("cr", "cr", 800,600);
  //c1->SetLeftMargin(0.13);
  cr->SetGrid();

  tree1->Draw(" ( (xFit-xTrue)*xFit + (yFit-yTrue)*yFit + (zFit-zTrue)*zFit ) / sqrt(xFit*xFit + yFit*yFit + zFit*zFit)  >> hr");
  hr->GetXaxis()->SetTitle("Radial Bias, mm");
  hr->GetYaxis()->SetTitle("Events");
  hz->GetXaxis()->SetRangeUser(-500,500);
  hr->GetYaxis()->SetRangeUser(0,200);
  hr->SetLineColor(kBlack);
  hr->SetLineWidth(2);
  hr->GetYaxis()->SetTitleOffset(1.4);
  TF1  *fr1 = new TF1("fr1","gaus");
  fr1->SetLineColor(kBlack);
  hr->Fit("fr1","0");

  tree2->Draw(" ( (xFit-xTrue)*xFit + (yFit-yTrue)*yFit + (zFit-zTrue)*zFit ) / sqrt(xFit*xFit + yFit*yFit + zFit*zFit) >> hr2","","same");
  hr2->GetXaxis()->SetTitle("Radial Bias, mm");
  hr2->GetYaxis()->SetTitle("Events");
  hr2->GetXaxis()->SetRangeUser(-500,500);
  hr2->SetLineColor(kRed+2);
  hr2->SetLineWidth(2);
  TF1  *fr2 = new TF1("fr2","gaus");
  fr2->SetLineColor(kRed+2);
  hr2->Fit("fr2","0");

  tree3->Draw(" ( (xFit-xTrue)*xFit + (yFit-yTrue)*yFit + (zFit-zTrue)*zFit ) / sqrt(xFit*xFit + yFit*yFit + zFit*zFit)  >> hr3");
  hr3->GetXaxis()->SetTitle("Radial Bias, mm");
  hr3->GetYaxis()->SetTitle("Events");
  hr3->GetXaxis()->SetRangeUser(-500,500);
  hr3->SetLineColor(kMagenta+2);
  hr3->SetLineWidth(2);
  TF1  *fr3 = new TF1("fr3","gaus");
  fr3->SetLineColor(kMagenta+2);
  hr3->Fit("fr3","0");

  tree4->Draw(" ( (xFit-xTrue)*xFit + (yFit-yTrue)*yFit + (zFit-zTrue)*zFit ) / sqrt(xFit*xFit + yFit*yFit + zFit*zFit) >> hr4","","same");
  hr4->GetXaxis()->SetTitle("Radial Bias, mm");
  hr4->GetYaxis()->SetTitle("Events");
  hr4->GetXaxis()->SetRangeUser(-500,500);
  hr4->SetLineColor(kBlue+2);
  hr4->SetLineWidth(2);
  TF1  *fr4 = new TF1("fr4","gaus");
  fr4->SetLineColor(kBlue+2);
  hr4->Fit("fr4","0");

  tree5->Draw(" ( (xFit-xTrue)*xFit + (yFit-yTrue)*yFit + (zFit-zTrue)*zFit ) / sqrt(xFit*xFit + yFit*yFit + zFit*zFit) >> hr5","","same");
  hr5->GetXaxis()->SetTitle("Radial Bias, mm");
  hr5->GetYaxis()->SetTitle("Events");
  hr5->GetXaxis()->SetRangeUser(-500,500);
  hr5->SetLineColor(kGreen+2);
  hr5->SetLineWidth(2);
  TF1  *fr5 = new TF1("fr5","gaus");
  fr5->SetLineColor(kGreen+2);
  hr5->Fit("fr5","0");

  hr->Draw("");
  hr2->Draw("same");
  hr3->Draw("same");
  hr4->Draw("same");
  hr5->Draw("same");

  TLegend* tr = new TLegend( 0.14, 0.5, 0.39, 0.8 );
  tr->AddEntry( hr, Form("0-2 m, Mean %.2f mm", fr1->GetParameter(1)), "l" );
  tr->AddEntry( hr2, Form("2-3 m, Mean %.2f mm", fr2->GetParameter(1)), "l" );
  tr->AddEntry( hr3, Form("3-4 m, Mean %.2f mm", fr3->GetParameter(1)), "l" );
  tr->AddEntry( hr4, Form("4-5 m, Mean %.2f mm", fr4->GetParameter(1)), "l" );
  tr->AddEntry( hr5, Form("5-6 m, Mean %.2f mm", fr5->GetParameter(1)), "l");
  tr->SetLineWidth(2);
  tr->Draw("same");

  cr->SaveAs("Dec12_bands_r.pdf");


  ////////////////////////////////////////////////////////////////////////////////

  TH1F *htemp = new TH1F("htemp", "Fit to Truth Distance", 100, -1000, 1000);
  TH1F *htemp2 = new TH1F("htemp2", "htemp2", 100, -1000, 1000);
  TH1F *htemp3 = new TH1F("htemp3", "htemp3", 100, -1000, 1000);
  TH1F *htemp4 = new TH1F("htemp4", "htemp4", 100, -1000, 1000);
  TH1F *htemp5 = new TH1F("htemp5", "htemp5", 100, -1000, 1000);

  TCanvas* c1 = new TCanvas("c1", "c1", 800,600);
  //c1->SetLeftMargin(0.13);
  c1->SetGrid();

  tree1->Draw(" sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue) )  >> htemp");
  htemp->GetXaxis()->SetTitle("Fit to Truth Distance, mm");
  htemp->GetYaxis()->SetTitle("Events");
  htemp->GetXaxis()->SetRangeUser(-500,500);
  htemp->GetYaxis()->SetRangeUser(0,200);
  htemp->SetLineColor(kBlack);
  htemp->SetLineWidth(2);
  htemp->GetYaxis()->SetTitleOffset(1.4);
  TF1  *ft1 = new TF1("ft1","gaus");
  ft1->SetLineColor(kBlack);
  htemp->Fit("ft1","0");

  tree2->Draw(" sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue) ) >> htemp2","","same");
  htemp2->GetXaxis()->SetTitle("Fit to Truth Distance, mm");
  htemp2->GetYaxis()->SetTitle("Events");
  htemp2->GetXaxis()->SetRangeUser(-500,500);
  htemp2->SetLineColor(kRed+2);
  htemp2->SetLineWidth(2);
  TF1  *ft2 = new TF1("ft2","gaus");
  ft2->SetLineColor(kRed+2);
  htemp2->Fit("ft2","0");

  tree3->Draw(" sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue) )  >> htemp3");
  htemp3->GetXaxis()->SetTitle("Fit to Truth Distance, mm");
  htemp3->GetYaxis()->SetTitle("Events");
  htemp3->GetXaxis()->SetRangeUser(-500,500);
  htemp3->SetLineColor(kMagenta+2);
  htemp3->SetLineWidth(2);
  TF1  *ft3 = new TF1("ft3","gaus");
  ft3->SetLineColor(kMagenta+2);
  htemp3->Fit("ft3","0");

  tree4->Draw(" sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue) ) >> htemp4","","same");
  htemp4->GetXaxis()->SetTitle("Fit to Truth Distance, mm");
  htemp4->GetYaxis()->SetTitle("Events");
  htemp4->GetXaxis()->SetRangeUser(-500,500);
  htemp4->SetLineColor(kBlue+2);
  htemp4->SetLineWidth(2);
  TF1  *ft4 = new TF1("ft4","gaus");
  ft4->SetLineColor(kBlue+2);
  htemp4->Fit("ft4","0");

  tree5->Draw(" sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue) ) >> htemp5","","same");
  htemp5->GetXaxis()->SetTitle("Fit to Truth Distance, mm");
  htemp5->GetYaxis()->SetTitle("Events");
  htemp5->GetXaxis()->SetRangeUser(-500,500);
  htemp5->SetLineColor(kGreen+2);
  htemp5->SetLineWidth(2);
  TF1  *ft5 = new TF1("ft5","gaus");
  ft5->SetLineColor(kGreen+2);
  htemp5->Fit("ft5","0");
  
  htemp->Draw("");
  htemp2->Draw("same");
  htemp3->Draw("same");
  htemp4->Draw("same");
  htemp5->Draw("same");

  TLegend* t1 = new TLegend( 0.14, 0.5, 0.39, 0.8 );
  t1->AddEntry( htemp, Form("0-2 m, Mean %.2f mm", ft1->GetParameter(1)), "l" );
  t1->AddEntry( htemp2, Form("2-3 m, Mean %.2f mm", ft2->GetParameter(1)), "l" );
  t1->AddEntry( htemp3, Form("3-4 m, Mean %.2f mm", ft3->GetParameter(1)), "l" );
  t1->AddEntry( htemp4, Form("4-5 m, Mean %.2f mm", ft4->GetParameter(1)), "l" );
  t1->AddEntry( htemp5, Form("5-6 m, Mean %.2f mm", ft5->GetParameter(1)), "l");
  t1->SetLineWidth(2);
  t1->Draw("same");

  c1->SaveAs("Dec12_bands_distance.pdf");
}
