#include <TError.h>
#include <TChain.h>
#include <TFileCollection.h>
#include <iostream>
#include <THashList.h>

int main( ) {

  std::string fnames = "/home/parkerw/Software/rat_master/*.ds.root";

  gErrorIgnoreLevel = kSysError;

  TChain* fTree;
  TChain* tempChain;
  TChain* tempChain2;

  bool isntuple = true;
  bool isds = false;

  TFileCollection * col = new TFileCollection();
  col->Add(fnames.c_str());
  int fnum = col->GetNFiles();
  tempChain = new TChain("output");
  tempChain->AddFileInfoList(dynamic_cast<THashList*>(col->GetList()));
  int num = tempChain->GetEntries();

  gErrorIgnoreLevel= -1;
  std::cout << "level " << gErrorIgnoreLevel << std::endl;
  
  tempChain2 = new TChain("T");
  tempChain2->AddFileInfoList(dynamic_cast<THashList*>(col->GetList()));
  int num2 = tempChain2->GetEntries();
 
  if ( tempChain->GetEntries() > 0 && tempChain2->GetEntries() > 0){
    std::cout << "Error: Found both DS and ntuples files in. Please only input files of one type." << std::endl;
    throw;
  }
  
  if ( tempChain->GetEntries() == 0 && tempChain2->GetEntries() == 0 ){
    std::cout << "Error: Found no DS or ntuples files" << std::endl;
    throw;
  }
  
  if (tempChain->GetEntries() > 0){
    isntuple = true;
    fTree = tempChain;
  }
  else if (tempChain2->GetEntries() > 0){
    isds = true;
    fTree = tempChain2;
  }

  std::cout << "Is DS: " << isds << std::endl;
  std::cout << "Is ntuple: " << isntuple << std::endl;
  std::cout << std::endl;
  //  std::cout << fTree->GetEntries() << std::endl;
  std::cout << std::endl;
  return 0;
}
