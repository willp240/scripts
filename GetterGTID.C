#include <TH2D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>

int GetterGTID() {

  // Read in file
  RAT::DU::DSReader dsreader("/home/parkerw/Software/rat_master/SNOP_0000356178_000_2ndpass.root");

  for(int i=0; i<dsreader.GetEntryCount();i++){

    //if( i < 100 || (i > 102 && i < 143636 ) || i > 143648  )
    //continue;

    const RAT::DS::Entry& rds = dsreader.GetEntry(i);

    std::cout << "event " << i << ", vertex " << rds.GetEV(0).GetGTID() <<  std::endl;

  }
}
