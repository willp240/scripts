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
#include <sstream>
#include <fstream>
#include "TF1.h"
#include <TLatex.h>

#include <RAT/DU/DSReader.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/Meta.hh>

int CopyTree(){

  /*  TFile* fileTree;

  //  fileTree = TFile::Open("/data/snoplus3/paton/DataSplitting/scintFill/rat705/NTuple/Analysis30_r0000302052_s014_p000.ntuple.root");
  fileTree = TFile::Open("outputs/inputfile4.ntuple.root");

  if(TTree* inT2 = (TTree*)fileTree->Get("output"))
    std::cout << "ntuple "<< std::endl;

  TTree* inTree = (TTree*)fileTree->Get("output");

  TTree* newTreePassed = inTree->CloneTree(0);
  
  inTree->GetEntry( 4 );
  newTreePassed->Fill();

  newTreePassed->Print();*/

  stringstream* sp;
  stringstream ss;
  ss << "hello";
  sp = &ss;
  std::cout << sp->str() << std::endl;

  std::ofstream output;
  output.open( "hello.txt" );

  output << sp->str() << std::endl;

  return 0;  
}
