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


void ComparePos(){

  gStyle->SetPalette(51);

  std::string axs[4] = {"xFit:xTrue", "yFit:yTrue", "zFit:zTrue", "sqrt(xFit*xFit+yFit*yFit+zFit*zFit):sqrt(xTrue*xTrue+yTrue*yTrue+zTrue*zTrue)"};
  std::string title[3] = {"MultiPath", "MultiPDF", "MultiPDF 4PDFs"};
  // double ymax[6] = {60, 160, 30, 180, 30, 180};
  //double ymin[6] = {-75, 80, -30, 100, -30, 100};
  std::string coord[4] = {"X", "Y", "Z", "R"};

  TFile *_pdffile = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Apr15_fixBeginOfRunFullSF_Tree.root");
  TTree* pdfTree = (TTree*)_pdffile->Get("eveTree")->Clone("pdftree");

  TFile *_pathfile = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Apr23_5GausNorm_Minuit_Tree.root");
  //  TFile *_pathfile = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/");
  TTree* pathTree = (TTree*)_pathfile->Get("eveTree")->Clone("pathtree");

  //  TFile *_pdffile4 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/nov26multipdf_plots_notLowR_4PDF_FixLLH.root");
  //  TFile *_pdffile4 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Dec1_12PDF_plots.root");
  TFile *_pdffile4 = TFile::Open("/home/parkerw/Software/rat-tools2/FitPerformance/Apr23_5GausNorm_Minuit_Tree.root");
  TTree* pdfTree4 = (TTree*)_pdffile4->Get("eveTree")->Clone("pdftree4");
 

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->Divide(4,3);
  
  std::string cut = "";//"zFit<7000 && zFit>0 && xFit<7000 && xFit>-7000 && yFit<7000 && yFit>-7000";

  for(int i=0; i<4; i++){
    c1->cd(i+1);
    gPad->SetRightMargin(0.14);
    pathTree->Draw(axs[i].c_str(),cut.c_str(),"colz");
    TH2F *htemp = (TH2F*)gPad->GetPrimitive("htemp");
    std::string xtitle = "True " + coord[i] + ", mm";
    std::string ytitle = "Fitted " + coord[i] + ", mm";
    htemp->GetXaxis()->SetTitle(xtitle.c_str());
    htemp->GetYaxis()->SetTitle(ytitle.c_str());
    htemp->SetTitle("Full Functional MultiPDF");
    htemp->GetYaxis()->SetTitleOffset(1.3);
    gPad->Update();
    
    c1->cd(i+5);
    gPad->SetRightMargin(0.14);
    pdfTree->Draw(axs[i].c_str(),cut.c_str(),"colz");
    TH2F *htemp2 = (TH2F*)gPad->GetPrimitive("htemp");
    std::string xtitle2 = "True " + coord[i] + ", mm";
    std::string ytitle2 = "Fitted " + coord[i] + ", mm";
    htemp2->GetXaxis()->SetTitle(xtitle2.c_str());
    htemp2->GetYaxis()->SetTitle(ytitle2.c_str());
    htemp2->SetTitle("Current ScintFitter");
    htemp2->GetYaxis()->SetTitleOffset(1.3);
    gPad->Update();
    
    c1->cd(i+9);
    gPad->SetRightMargin(0.14);
    pdfTree4->Draw(axs[i].c_str(),cut.c_str(),"colz");
    TH2F *htemp3 = (TH2F*)gPad->GetPrimitive("htemp");
    std::string xtitle3 = "True " + coord[i] + ", mm";
    std::string ytitle3 = "Fitted " + coord[i] + ", mm";
    htemp3->GetXaxis()->SetTitle(xtitle3.c_str());
    htemp3->GetYaxis()->SetTitle(ytitle3.c_str());
    htemp3->SetTitle("Full Functional MultiPDF");
    htemp3->GetYaxis()->SetTitleOffset(1.3);
    gPad->Update();
  }

}

