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

int loopFile(){


  for(int i=0; i<100; i++){
    TString filename = Form("/data/snoplus/parkerw/ratSimulations/batch/oct12MultPDFFit/multiPDF_lt1500_%d.root",i);
    TFile* fileTree = TFile::Open(filename);
    TTree* T = (TTree*)fileTree->Get("T");
    std::cout << filename << " " << T->GetEntries() <<  std::endl;;
  }

  for(int i=0; i<100; i++){
    TString filename = Form("/data/snoplus/parkerw/ratSimulations/batch/oct12MultPDFFit/multiPDF_%d.root",i);
    TFile* fileTree = TFile::Open(filename);
    TTree* T = (TTree*)fileTree->Get("T");
    std::cout << filename << " " << T->GetEntries() << std::endl;
  }

}
