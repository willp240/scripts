#include <TH2Poly.h>
#include "TMath.h"
#include <TH2D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <TEntryList.h>
#include <THStack.h>
#include <iostream>
#include "TF1.h"
#include <TLatex.h>

#include <RAT/DU/DSReader.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/Meta.hh>

int SetEntryList(){


  std::string fileName = "inputfile4.root";
  TFile* fileTree;

  fileTree = TFile::Open("inputfile4.root");

  if(TTree* inT = (TTree*)fileTree->Get("T"))
    std::cout << "ds " << std::endl;

  if(TTree* inT2 = (TTree*)fileTree->Get("output"))
     std::cout << "ntuple "<< std::endl;

  return 0;


  //  TTree* inTree = (TTree*)fileTree->Get("output");
  TFile *outf = TFile::Open("out.root", "RECREATE");
  TTree* inTree = (TTree*)fileTree->Get("output");
  outf->cd();
  TTree* inTree2= (TTree*)inTree->CloneTree();

  double mct;
  int nEntries = inTree2->GetEntries();

  inTree2->SetBranchAddress("mct",&mct); // grab the event number from the tree
  
  TEntryList *tlist = new TEntryList("passedList","passedList",inTree2); // initialize entry list for 'TTree* tree'
  
  // loop over the entries in 'tree'
  for (int j = 0; j < nEntries; ++j)
    {
      inTree2->GetEvent(j);

      // if we have not seen this event yet, add it to the set
      // and to the entry list
      if (mct < 2000*pow(10,6))
	{
	  std::cout << j << " " << mct << std::endl;
	  tlist->Enter(j,inTree2);
	}
    }
  std::cout << "N " << tlist->GetN() << std::endl;

  // apply the entry list to the tree
  inTree2->SetEntryList(tlist);

  // histogram of the variable 'var' will be drawn only with the
  // entries specified by the entry list.
  inTree2->Draw("mct");

  std::cout << tlist->GetN() << " " << std::endl;
  std::cout << inTree2->GetEntries() << std::endl;

  outf->cd();
  inTree2->Write();
  tlist->Write();
  //  outf->Close();
  
}
