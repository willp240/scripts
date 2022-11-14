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

int TestTree(){

  std::string line = "abcdefghijklmnopqrstuvwxyz";

  size_t pos = line.find( "f" );
  size_t posb = line.find("k");

  std::cout << pos << " " << posb << std::endl;
  std::cout << line.substr( 0, pos ) << "-------" << line.substr( pos+1, posb-pos-1) << std::endl;

  return 0;  
}
