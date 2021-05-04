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


void CompareTree(){

  gStyle->SetPalette(51);

  std::string axs[4] = {"xTrue:scaledLLH", "yTrue:scaledLLH", "zTrue:scaledLLH", "sqrt(xTrue*xTrue+yTrue*yTrue+zTrue*zTrue):scaledLLH"};
  std::string title[3] = {"MultiPath", "MultiPDF", "MultiPDF 4PDFs"};
  // double ymax[6] = {60, 160, 30, 180, 30, 180};
  //double ymin[6] = {-75, 80, -30, 100, -30, 100};
  std::string coord[4] = {"X", "Y", "Z", "R"};

  TFile *_pdffile = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/nov26multipdf_plots_notLowR.root");
  TTree* pdfTree = (TTree*)_pdffile->Get("eveTree")->Clone("pdftree");

  TFile *_pathfile = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/nov26multipath_plots.root");
  TTree* pathTree = (TTree*)_pathfile->Get("eveTree")->Clone("pathtree");

  //  TFile *_pdffile4 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/nov26multipdf_plots_notLowR_4PDF_FixLLH.root");
  TFile *_pdffile4 = TFile::Open("/home/parkerw/Software/rat-tools_fork/FitPerformance/Dec1_12PDF_plots.root");
  TTree* pdfTree4 = (TTree*)_pdffile4->Get("eveTree")->Clone("pdftree4");
 

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->Divide(4,3);
  
  for(int i=0; i<4; i++){
    
    c1->cd(i+1);
    pathTree->Draw(axs[i].c_str(),"xFit>-20000","colz");
    TH2F *htemp = (TH2F*)gPad->GetPrimitive("htemp");
    std::string ytitle = "True " + coord[i] + ", mm";
    std::string xtitle = "Scaled LogL";
    htemp->GetXaxis()->SetTitle(xtitle.c_str());
    htemp->GetYaxis()->SetTitle(ytitle.c_str());
    htemp->SetTitle("MultiPath");
    htemp->GetYaxis()->SetTitleOffset(1.3);
    gPad->Update();
    
    c1->cd(i+5);
    pdfTree->Draw(axs[i].c_str(),"scaledLLH>11","colz");
    TH2F *htemp2 = (TH2F*)gPad->GetPrimitive("htemp");
    std::string ytitle2 = "True " + coord[i] + ", mm";
    std::string xtitle2 = "Scaled LogL";
    htemp2->GetXaxis()->SetTitle(xtitle2.c_str());
    htemp2->GetYaxis()->SetTitle(ytitle2.c_str());
    htemp2->SetTitle("MultiPDF");
    htemp2->GetYaxis()->SetTitleOffset(1.3);
    gPad->Update();
    
    c1->cd(i+9);
    pdfTree4->Draw(axs[i].c_str(),"scaledLLH>11","colz");
    TH2F *htemp3 = (TH2F*)gPad->GetPrimitive("htemp");
    std::string ytitle3 = "True " + coord[i] + ", mm";
    std::string xtitle3 = "Scaled LogL";
    htemp3->GetXaxis()->SetTitle(xtitle3.c_str());
    htemp3->GetYaxis()->SetTitle(ytitle3.c_str());
    htemp3->SetTitle("MultiPDF 4PDFs");
    htemp3->GetYaxis()->SetTitleOffset(1.3);
    gPad->Update();
  }

}

