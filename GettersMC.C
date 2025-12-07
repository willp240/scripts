#include <TH2D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>


int GettersMC() {

  // Read in file
  RAT::DU::DSReader dsreader("/home/parkerw/Software/rat_1/test.root");
  
  for(int i=0; i<dsreader.GetEntryCount();i++){
    const RAT::DS::Entry& rds = dsreader.GetEntry(i);

    const RAT::DS::MC& rMC = rds.GetMC();

    for( size_t iPMT = 0; iPMT < rMC.GetMCPMTCount(); iPMT++ )
      {
	const RAT::DS::MCPMT& pmt = rMC.GetMCPMT( iPMT );
	TVector3 emit = pmt.GetEmitPos();
	double t_em = pmt.GetEmitTime();
	int proc = pmt.GetCreatorProcess();
	int pdg = pmt.GetAncestorPDG();
	int trk = pmt.GetAncestorTrackID();
	
	std::cout << "pos " << emit.X() << " " << emit.Y() << " " << emit.Z() << std::endl;
	std::cout << "t " << t_em << std::endl;
	std::cout << "proc " << proc << std::endl;
	std::cout << "pdg " << pdg << std::endl;
	std::cout << "trk " << trk <<	std::endl << std::endl;
      }
  }
}

