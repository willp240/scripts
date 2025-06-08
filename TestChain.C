#include <TH2D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>

void TestChain()
{

  /*  TFile* f = TFile::Open("/data/snoplus/parkerw/dsfiles/bSNOP_0000356178_000_2ndpass.root");
  TTree* fT = (TTree*)f->Get("T");

  fT->LoadTree(fT->GetEntries()-1);
//  fT->GetListOfBranches()->Print();
  for(int i=0; i<fT->GetEntries(); i++)
    {
      fT->GetEntry(i);
      std::cout << "event " << i << std::endl;
    }
  */

  TChain* fT = new TChain("T");;

  TFileCollection * col = new TFileCollection();
  col->Add("/data/snoplus/parkerw/dsfiles/*.root");

   fT->AddFileInfoList(dynamic_cast<TList*>(col->GetList()));

   RAT::DS::Entry* fDS = new RAT::DS::Entry();
   fT->SetBranchAddress("ds", &fDS);
   
   Long64_t* offsets = fT->GetTreeOffset();
   Int_t nFiles = fT->GetNtrees();

   for (int i = 0; i < nFiles; ++i) {
     Long64_t firstEntry = offsets[i];
     Long64_t lastEntry = (i + 1 < nFiles) ? offsets[i + 1] - 1 : fT->GetEntries() - 1;
     fT->GetEntry(lastEntry);
     //if (firstEntry <= lastEntry) {
       // Touching the last entry of this file to trigger streamer/basket loading
       //fT->GetEntry(lastEntry);
     // }
   }

   //   for(int i=0; i<fT->GetEntries(); i++){
   //fT->GetEntry(i);
   // std::cout << "event " << i << std::endl;
   // }
}
