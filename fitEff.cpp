#include "TMath.h"
#include <TH2D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <iostream>
#include <vector>
#include <exception>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <ctime>

int fitEff() {

  // Read in file
  //  RAT::DU::DSReader dsreader("./testPartialFitter.root");
  RAT::DU::DSReader dsreader("/data/snoplus/parkerw/ratSimulations/batch/Nov17_multipdf_multipath_3MeV/input_70.root");
  //RAT::DU::DSReader dsreader("/home/parkerw/Software/rat_b/testEff.root");
  int nEvs=0;
  int nPath=0;
  int nPDF=0;


  for(int i=0; i<dsreader.GetEntryCount();i++){
    RAT::DS::Entry& rds = dsreader.GetEntry(i);
    if(i%100==0)
      std::cout<< "Event " << i << " of " << dsreader.GetEntryCount() << std::endl;
    int nevC = rds.GetEVCount();

    for(int iev=0;iev<nevC; iev++){
      nEvs++;
      RAT::DS::EV& rev = rds.GetEV(iev);
      std::vector<std::string> fitnames = rev.GetFitNames();

      if (std::find(fitnames.begin(), fitnames.end(), "partialFitter") != fitnames.end()){
	nPath++;
      }

      if(fitnames.at(0) == "MultiPDFMethod" || fitnames.at(fitnames.size()-1) == "MultiPDFMethod" ){
	nPDF++;
      }
    }
  }
  std::cout << "Of " << nEvs << " events: " << std::endl << nPDF << " MultiPDF " << std::endl << nPath << " MultiPath " << std::endl;
}
