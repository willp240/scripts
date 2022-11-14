#include <TH2D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>


int Getters() {

  // Read in file
  // RAT::DU::DSReader dsreader("/home/parkerw/Software/rat_b/testpartialmpdf.root");
  RAT::DU::DSReader dsreader("/data/snoplus/parkerw/ratSimulations/batch/Jul21_recoordMPDF_2p2gl_perf_2p5MeV/recoord_MPDF_99.root");

  //Get pmtInfo
  //  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo();
  for(int i=0; i<dsreader.GetEntryCount();i++){
    RAT::DS::Entry& rds = dsreader.GetEntry(i);

    int nevC = rds.GetEVCount();
    std::endl;
    for(int iev=0;iev<nevC; iev++){

      RAT::DS::EV& rev = rds.GetEV(iev);
      std::cout << "event " << i << ", vertex " << iev << std::endl;

      std::vector<std::string> fittynames = rev.GetFitNames();
      for(int i_fit=0; i_fit<fittynames.size(); i_fit++){
	std::cout << fittynames.at(i_fit) << std::endl;
	RAT::DS::FitResult fitResult = rev.GetFitResult( fittynames.at(i_fit) );
	RAT::DS::FitVertex fitVertex = fitResult.GetVertex(0);
	std::vector<std::string> fomnames = fitResult.GetFOMNames();
	for(int i_names=0; i_names<fomnames.size(); i_names++){
	  std::cout << fomnames.at(i_names) << std::endl;
	}
      }
    }
  }
}
