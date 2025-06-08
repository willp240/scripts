#include <TFile.h>
#include <TKey.h>
#include <TTree.h>
#include <TObject.h>
#include <iostream>

void TestSize(const char* filename) {
  TFile* f = TFile::Open(filename);
  if (!f || f->IsZombie()) {
    std::cerr << "Could not open file: " << filename << std::endl;
    return;
  }

  TIter next(f->GetListOfKeys());
  TKey* key;

  std::cout << "Object sizes in " << filename << ":\n";
  while ((key = (TKey*)next())) {
    TObject* obj = key->ReadObj();
    std::string className = obj->ClassName();
    std::string objName = key->GetName();

    // Default size from TKey
    Long64_t keySize = key->GetNbytes();
    Long64_t actualSize = keySize;

    // If it's a TTree, get total compressed basket size
    if (className == "TTree") {
      TTree* tree = dynamic_cast<TTree*>(obj);
      if (tree) {
        actualSize = tree->GetZipBytes();  // Total compressed size of baskets
      }
    }

    std::cout << objName << " (" << className << ")"
              << " : " << actualSize/(1E9) << " GB"
              << (className == "TTree" ? " [tree basket data]" : " [key metadata]")
              << std::endl;

    delete obj; // Clean up memory
  }

  delete f;
}






/*
void TestSize()
{

  std::string filename = "/home/parkerw/Software/rat_master/SNOP_0000356178_000_2ndpass.root" ;
  //  TFile* f = TFile::Open("/data/snoplus3/SNOplusData/processing/fullFill/rat-8.0.1/ratds/Analysis15_bMR_r0000360523_s003_p000.root");
  TFile* f = TFile::Open("/home/parkerw/Software/rat_master/SNOP_0000356178_000_2ndpass.root");
  TIter next(f->GetListOfKeys());
  TKey* key;

  while ((key = (TKey*)next())) {
    TObject* obj = key->ReadObj();
    std::cout << key->GetName()
              << " (" << obj->ClassName() << ")"
              << " : " << key->GetNbytes() << " bytes on disk" << std::endl;
  }

}

*/
