#include <TFile.h>
#include <TTree.h>
#include <TString.h>

#include <iostream>
#include <string>
#include <vector>

void one_pair_open(const char* reprocfile, const char* origfile, const char* outfilename) {


  double fitx1, fity1, fitz1, fitr1, fitx2, fity2, fitz2, fitr2;
  int nhits;

  // Declare tree with branches: mcx mcy mcz mcr mce fitx1 fity1 fitz1 fitr1 fitx2 fity2 fitz2 fitr2
  TFile *file = TFile::Open(outfilename, "RECREATE");
  TTree *tree = new TTree("T","events");

  tree->SetAutoSave(0);
  tree->SetAutoFlush(-100000);
  tree->SetMaxVirtualSize(64*1024*1024);
  
  tree->Branch("fitx1", &fitx1);
  tree->Branch("fity1", &fity1);
  tree->Branch("fitz1", &fitz1);
  tree->Branch("fitr1", &fitr1);
  tree->Branch("fitx2", &fitx2);
  tree->Branch("fity2", &fity2);
  tree->Branch("fitz2", &fitz2);
  tree->Branch("fitr2", &fitr2);
  tree->Branch("nhits", &nhits);

  RAT::DU::DSReader dsreaderOrig(reprocfile);
  RAT::DU::DSReader dsreaderUpdated(origfile);
      
  for( size_t iEntry = 0; iEntry < dsreaderOrig.GetEntryCount(); iEntry++ )
    {

      if(iEntry % 10000 == 0)
	std::cout << iEntry << " of " << dsreaderOrig.GetEntryCount() << std::endl;

      RAT::DS::Entry rDS1;
      RAT::DS::Entry rDS2;
      
      try
	{
	  rDS1 = dsreaderOrig.GetEntry( iEntry );
	  rDS2 = dsreaderUpdated.GetEntry( iEntry );
	}
      catch (const RAT::DS::DataNotFound&)
	{
	  continue;
	}
	  
      // Loop over events
      for(size_t iEv = 0; iEv< rDS1.GetEVCount(); iEv++){
	
	// Get from file 1
	const RAT::DS::EV& rev1 = rDS1.GetEV(iEv);
	// Get from file 2
	const RAT::DS::EV& rev2 = rDS2.GetEV(iEv);

	RAT::DS::FitVertex rVertex1;
	RAT::DS::FitVertex rVertex2;
	
	try
	  {
	    rVertex1 = rev1.GetFitResult("scintFitter").GetVertex(0);
	    rVertex2 = rev2.GetFitResult("scintFitter").GetVertex(0);
	  }
	catch (const RAT::DS::DataNotFound&)
	  {
	    continue;
	  }
	
	if (rev1.GetGTID() != rev2.GetGTID())
	  {
	    continue;
	  }
	    
	if(!(rVertex1.ValidPosition() && rVertex1.ValidTime()))
	  {
	    continue; // fit invalid
	  }
	    
	fitx1 = rVertex1.GetPosition().X();
	fity1 = rVertex1.GetPosition().Y();
	fitz1 = rVertex1.GetPosition().Z();
	fitr1 = rVertex1.GetPosition().Mag();
	
	if(!(rVertex2.ValidPosition() && rVertex2.ValidTime()))
	  {
	    continue; // fit invalid
	  }
	
	fitx2 = rVertex2.GetPosition().X();
	fity2 = rVertex2.GetPosition().Y();
	fitz2 = rVertex2.GetPosition().Z();
	fitr2 = rVertex2.GetPosition().Mag();

	nhits = rev1.GetNhits();
	
	// Fill tree
	tree->Fill();
	tree->AutoSave("SaveSelf");
	
      } // end loop evs
      
    } // end loop entries
  
  // make sure we’re writing to the output file
  file->cd();
  
  tree->Write("", TObject::kOverwrite);
  file->Write();
  file->Close();
  
  std::cout << "Done.\n";
}
