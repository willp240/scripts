#include <TH2Poly.h>
#include "TMath.h"
#include <TH2D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <THStack.h>
#include <iostream>
#include "TF1.h"
#include <TLatex.h>

void GetGaussian(TH1D *& hpost, TF1 *& gauss, double &central, double &error);

int biascopy(){

  TFile* fileTree = TFile::Open("tree.root");
  TTree* biasTree = (TTree*)fileTree->Get("biasTree");
  float trueX, trueY, trueZ, recoX_4PDFS, recoY_4PDFS, recoZ_4PDFS, recoX_8PDFS, recoY_8PDFS, recoZ_8PDFS;
  biasTree->SetBranchAddress("trueX", &trueX);
  biasTree->SetBranchAddress("trueY", &trueY);
  biasTree->SetBranchAddress("trueZ", &trueZ);
  biasTree->SetBranchAddress("recoX_4PDFs", &recoX_4PDFS);
  biasTree->SetBranchAddress("recoY_4PDFs", &recoY_4PDFS);
  biasTree->SetBranchAddress("recoZ_4PDFs", &recoZ_4PDFS);
  biasTree->SetBranchAddress("recoX_8PDFs", &recoX_8PDFS);
  biasTree->SetBranchAddress("recoY_8PDFs", &recoY_8PDFS);
  biasTree->SetBranchAddress("recoZ_8PDFs", &recoZ_8PDFS);

  Long64_t nentries = biasTree->GetEntries();

  TH1D *hBias4PDFS = new TH1D("hBias4PDFS", "hBias4PDFS", 10, -200, 200);
  TH1D *hBias8PDFS = new TH1D("hBias8PDFS", "hBias8PDFS", 10, -200, 200);

  for(int i=0; i<nentries; i++){
    biasTree->GetEntry(i);
    double r = sqrt(trueX*trueX + trueY*trueY + trueZ*trueZ);
    if(r<=2400 && r>=2000){
      hBias4PDFS->Fill(recoX_4PDFS-trueX);
      hBias8PDFS->Fill(recoX_8PDFS-trueX);
    }
  }

  TFile *outFile = new TFile("testSwitchO.root","RECREATE");
  TF1 *gauss = new TF1("gauss","[0]/sqrt(2.0*3.14159)/[2]*TMath::Exp(-0.5*pow(x-[1],2)/[2]/[2])",-5,5);
  double central, error;
  gauss->SetLineWidth(2);
  gauss->SetLineColor(kOrange-5);

  //  GetGaussian(hBias4PDFS, gauss, central, error);
  
  TF1 *gausFunction = new TF1("myGaus", "gaus(0)");
  hBias4PDFS->SetLineColor(kBlue);
  //  hBias4PDFS->Draw();
  std::cout << hBias4PDFS->GetMean() << " " << hBias4PDFS->GetRMS() << std::endl;
 
  gausFunction->SetParameter(0, hBias4PDFS->GetMaximumBin());
  gausFunction->SetParameter(1, hBias4PDFS->GetBinLowEdge(hBias4PDFS->GetMaximumBin()));
  gausFunction->SetParameter(2, hBias4PDFS->GetStdDev());
  hBias4PDFS->Fit(gausFunction, "QMEL");
  std::cout << gausFunction->GetParameter(1) <<  " " << gausFunction->GetParError(1) << std::endl;
  std::cout << gausFunction->GetParameter(2) <<  " " << gausFunction->GetParError(2) << std::endl;


  /*  TF1 *gausFunction2 = new TF1("myGaus2", "gaus(0)");

  gausFunction2->SetParameter(0, hBias8PDFS->GetMaximumBin());
  gausFunction2->SetParameter(1, hBias8PDFS->GetBinLowEdge(hBias8PDFS->GetMaximumBin()));
  gausFunction2->SetParameter(2, hBias8PDFS->GetStdDev());
  hBias8PDFS->Fit(gausFunction2, "QMEL");
  std::cout << gausFunction2->GetParameter(1) <<  " " << gausFunction2->GetParError(1) << std::endl;
  std::cout << gausFunction2->GetParameter(2) <<  " " << gausFunction2->GetParError(2) << std::endl;*/

  /*
  gauss->SetLineColor(kRed-5);
  GetGaussian(hBias8PDFS, gauss, central, error);
  hBias8PDFS->SetLineColor(kRed);
  hBias8PDFS->Draw("same");
  std::cout << central <<  " " << error<< std::endl*/;
}


// **************************
// Get Gaussian characteristics
void GetGaussian(TH1D *& hpost, TF1 *& gauss, double &central, double &error) {
  // **************************
  double mean = hpost->GetMean();
  double err = hpost->GetRMS();
  double peakval = hpost->GetBinCenter(hpost->GetMaximumBin());
  // Set the range for the Gaussian fit
  gauss->SetRange(mean - 1.5*err , mean + 1.5*err);
  // Set the starting parameters close to RMS and peaks of the histograms
  gauss->SetParameters(hpost->GetMaximum()*err*sqrt(2*3.14), peakval, err);

  // Perform the fit
  hpost->Fit(gauss->GetName(),"Rq");
  hpost->SetStats(0);

  central = gauss->GetParameter(1);
  error = gauss->GetParameter(2);
}
