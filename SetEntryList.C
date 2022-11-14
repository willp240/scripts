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

int SetEntryList(){

  TFile* fileTree = TFile::Open("output.ntuple.root");
  TTree* inTree = (TTree*)fileTree->Get("output");

  double mct;
  int nEntries = inTree->GetEntries();

  inTree->SetBranchAddress("mct",&mct); // grab the event number from the tree

  TEntryList *tlist = new TEntryList(inTree); // initialize entry list for 'TTree* tree'

  // loop over the entries in 'tree'
  for (int j = 0; j < nEntries; ++j)
    {
      inTree->GetEvent(j);

      // if we have not seen this event yet, add it to the set
      // and to the entry list
      if (mct < 2000*pow(10,6))
	{
	  std::cout << j << " " << mct << std::endl;
	  tlist->Enter(j,inTree);
	}
    }

  // apply the entry list to the tree
  inTree->SetEntryList(tlist);

  // histogram of the variable 'var' will be drawn only with the
  // entries specified by the entry list.
  inTree->Draw("mct");

}
