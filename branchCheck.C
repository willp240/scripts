#include <TH2Poly.h>
#include "TMath.h"
#include <TH2D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <THStack.h>
#include <iostream>
#include "TF1.h"
#include <TLatex.h>

int branchCheck()
{

  TChain chain("output");
  chain.Add("/data/snoplus3/SNOplusData/processing/fullFill/rat-7.0.8/ntuples/Analysis20R_r0000301304_s000_p000.ntuple.root");
  chain.Add("/data/snoplus3/SNOplusData/processing/fullFill/rat-7.0.8/ntuples/Analysis20R_r0000301304_s001_p000.ntuple.root");

  TFile *newFile = TFile::Open("newFile.root", "RECREATE");
  TTree *newTree = chain.CloneTree(0);

  float newBranchVar; // Example: A new float branch
  newTree->Branch("newBranchName", &newBranchVar, "newBranchName/F");

  Long64_t nentries =10;
  //= chain.GetEntries();
  for (Long64_t i = 0; i < nentries; i++)
  {
    chain.GetEntry(i);

    // Set the value of newBranchVar for this entry
    newBranchVar = (i); // Function to compute new value

    newTree->Fill(); // Fill the new tree with the original data and the new branch
  }
  newTree->Write();
  newFile->Close();

  return 0;
}
