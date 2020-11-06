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

int boundary(){

  TFile* fileTree = TFile::Open("redoneoutfit.root");
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
  int count8=0, count4=0;

  for(int i=0; i<nentries; i++){
    biasTree->GetEntry(i);
    float r_True = sqrt(trueX*trueX + trueY*trueY + trueZ*trueZ);
    float r_4pdf = sqrt( recoX_4PDFS*recoX_4PDFS + recoY_4PDFS*recoY_4PDFS + recoZ_4PDFS*recoZ_4PDFS);
    float r_8pdf = sqrt( recoX_8PDFS*recoX_8PDFS + recoY_8PDFS*recoY_8PDFS + recoZ_8PDFS*recoZ_8PDFS);

    //    if( (r_4pdf<1500 && r_8pdf>1500) || (r_4pdf>1500 &&r_8pdf<1500) ){
    if( fabs(recoX_8PDFS-recoX_4PDFS)>1000 || fabs(recoY_8PDFS-recoY_4PDFS)>1000 || fabs(recoZ_8PDFS-recoZ_4PDFS)>1000 ){

      if( fabs(recoX_4PDFS-trueX)>1000 || fabs(recoY_4PDFS-trueY)>1000 || fabs(recoZ_4PDFS-trueZ)>1000 ){
	count4++;
	continue;
      }

      if(fabs(r_True-r_4pdf) < fabs(r_True-r_8pdf))
	continue;
      
      std::cout << "event " << i << "\nR_true: " << r_True << "\t(" << trueX << ",\t" << trueY << ",\t" << trueZ << ")\nR_4pdf: " << r_4pdf << "\t(" << recoX_4PDFS << ",\t" << recoY_4PDFS << ",\t" << recoZ_4PDFS << ")\nR_8pdf: " << r_8pdf << "\t(" << recoX_8PDFS << ",\t" << recoY_8PDFS << ",\t" << recoZ_8PDFS << ")" << std::endl;
      std::cout << std::endl;
      count8++;
    }
  }
  std::cout << count8 << " out of " << nentries << " bad for 8pdfs (" << 100*count8/(float)nentries << "%)" << std::endl;
  std::cout << count4 << " out of " << nentries << " bad for 4 pdfs (" << 100*count4/(float)nentries << "%)" << std::endl;
}

