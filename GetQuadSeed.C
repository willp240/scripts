#include <TH2D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>


int GetQuadSeed() {

  // Read in file
  RAT::DU::DSReader dsreader("/home/parkerw/Software/rat_cpp17/test.root");

  //Get pmtInfo
  //  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo();
  for(int i=0; i<dsreader.GetEntryCount();i++){
    const RAT::DS::Entry& rds = dsreader.GetEntry(i);

    int nevC = rds.GetEVCount();

    for(int iev=0;iev<nevC; iev++){

      const RAT::DS::EV& rev = rds.GetEV(iev);
      std::cout << "event " << i << ", vertex " << iev << " " << rev.GetNhits() <<  std::endl;

      RAT::DS::FitResult seedResult = rev.GetFitResult( "scintFitter" );
      std::cout << "quad pos " << seedResult.GetVertex(1).GetPosition().X() << " " << seedResult.GetVertex(1).GetPosition().Y() << " " << seedResult.GetVertex(1).GetPosition().Z() << std::endl;
      std::cout << "quad pos errors " << seedResult.GetVertex(1).GetPositivePositionError().X() << " " << seedResult.GetVertex(1).GetPositivePositionError().Y() << " " << seedResult.GetVertex(1).GetPositivePositionError().Z() << std::endl;
      std::cout << "quad neg errors " << seedResult.GetVertex(1).GetNegativePositionError().X() << " " << seedResult.GetVertex(1).GetNegativePositionError().Y() << " " << seedResult.GetVertex(1).GetNegativePositionError().Z() << std::endl;
      
    }
  }
}
