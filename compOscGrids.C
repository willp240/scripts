
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


void compOscGrids(){
  
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetPalette(51);
  
  TFile *file1 = TFile::Open("/data/snoplus3/parkerw/antinu/Nov5_oscgridsvecBruce1_1000x1000x140/comp1000x1000x140_500x500x140.root");
  TTree* tree1 = (TTree*)file1->Get("T")->Clone("tree");

  std::string outdir = "/data/snoplus3/parkerw/antinu/Nov5_oscgridsvecBruce1_1000x1000x140/comp1000_500_";
  std::string canv_name;
  
  ////////////////////////////////////////////////////////////////////////////////


  TH2F *hP_E = new TH2F("hP_E", "Probability Difference vs Energy", 100, 1.0, 8.0, 50, -0.1, 0.1);
  TCanvas* cpe = new TCanvas("cpe", "cpe", 800,600);
  cpe->SetRightMargin(0.13);
  cpe->SetGrid();

  tree1->Draw("p1-p2:energy >> hP_E","","colz");
  hP_E->GetXaxis()->SetTitle("Prompt Energy, MeV");
  hP_E->GetYaxis()->SetTitle("P_{1000} - P_{500}");
  canv_name = outdir + "probdiff_E.pdf";
  cpe->SaveAs(canv_name.c_str());


  TH2F *hP_dm = new TH2F("hP_dm", "Probability Difference", 25, 0.00001, 0.000015, 50, -0.1, 0.1);
  TCanvas* cdm = new TCanvas("cdm", "cdm", 800,600);
  cdm->SetRightMargin(0.13);
  cdm->SetGrid();

  tree1->Draw("p1-p2:dm21sq >> hP_dm","","colz");
  hP_dm->GetXaxis()->SetTitle("#Delta m_{21}^{2}, MeV^2");
  hP_dm->GetYaxis()->SetTitle("P_{1000} - P_{500}");
  canv_name = outdir + "probdiff_dm21sq.pdf";
  cdm->SaveAs(canv_name.c_str());
  
  
  TH2F *hP_ss = new TH2F("hP_ss", "Probability Difference", 25, 0, 1, 50, -0.1, 0.1);
  TCanvas* css = new TCanvas("css", "css", 800,600);
  css->SetRightMargin(0.13);
  css->SetGrid();

  tree1->Draw("p1-p2:ssqth21 >> hP_ss","","colz");
  hP_ss->GetXaxis()->SetTitle("sin^2 #theta_{12}");
  hP_ss->GetYaxis()->SetTitle("P_{1000} - P_{500}");
  canv_name = outdir + "probdiff_ssqth12.pdf";
  css->SaveAs(canv_name.c_str());

  
  TH1F *hP = new TH1F("hP", "Probability Difference", 500, -0.1, 0.11);
  TCanvas* cp = new TCanvas("cp", "cp", 800,600);
  cp->SetRightMargin(0.13);
  cp->SetGrid();
  cp->SetLogy();
  
  tree1->Draw("p1-p2 >> hP","","colz");
  hP->GetXaxis()->SetTitle("P_{1000} - P_{500}");
  hP->GetYaxis()->SetTitle("Counts");
  canv_name = outdir + "probdiff.pdf";
  cp->SaveAs(canv_name.c_str());

  
  TH1F *hP_frac = new TH1F("hP_frac", "Probability Difference", 500, -1.0, 1);
  TCanvas* cp_frac = new TCanvas("cp_frac", "cp_frac", 800,600);
  cp_frac->SetRightMargin(0.13);
  cp_frac->SetGrid();
  cp_frac->SetLogy();

  tree1->Draw("(p1-p2)/p1 >> hP_frac","","colz");
  hP_frac->GetXaxis()->SetTitle("(P_{1000} - P_{500}) / P_{1000}");
  hP_frac->GetYaxis()->SetTitle("Counts");
  canv_name = outdir + "probdifffrac.pdf";
  cp_frac->SaveAs(canv_name.c_str());

  
  TH2F *hP_comp = new TH2F("hP_comp", "Probability Difference", 100, 0, 1, 100, 0, 1);
  TCanvas* cp_comp = new TCanvas("cp_comp", "cp_comp", 800,600);
  cp_comp->SetRightMargin(0.13);
  cp_comp->SetGrid();

  tree1->Draw("p1:p2 >> hP_comp","","colz");
  hP_comp->GetXaxis()->SetTitle("P_{500}");
  hP_comp->GetYaxis()->SetTitle("P_{1000}");
  canv_name = outdir + "probcomp_E.pdf";
  cp_comp->SaveAs(canv_name.c_str());
  

  TH2F *hP_frac2d = new TH2F("hP_frac2d", "Probability Difference", 100, 0, 1, 100, -1, 1);
  TCanvas* cp_frac2d = new TCanvas("cp_frac2d", "cp_frac2d", 800,600);
  cp_frac2d->SetRightMargin(0.13);
  cp_frac2d->SetGrid();

  tree1->Draw("(p1-p2)/p1:p1 >> hP_frac2d","","colz");
  hP_frac2d->GetYaxis()->SetTitle("(P_{1000} - P_{500}) / P_{1000}");
  hP_frac2d->GetXaxis()->SetTitle("P_{1000}");
  canv_name = outdir + "probdifffrac_prob.pdf";
  cp_frac2d->SaveAs(canv_name.c_str());
  
}
