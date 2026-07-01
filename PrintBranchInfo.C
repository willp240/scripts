#include <iostream>
#include <iomanip>

void PrintBranchInfo()
{
    TFile *f = TFile::Open("/data/snoplus3/SNOplusData/processing/fullFill/rat-8.2.0/Analysis50_bMR/ntuples/Analysis50_bMR_r0000354815_s015_p000.ntuple.root");

    if (!f || f->IsZombie()) {
        std::cerr << "Couldn't open file\n";
        return;
    }

    // Uncomment this if you're not sure of the tree name
    // f->ls();

    TTree *tree = (TTree*)f->Get("output");
    if (!tree) {
        std::cerr << "Couldn't find tree\n";
        return;
    }

    TObjArray *branches = tree->GetListOfBranches();

    std::cout << std::left
          << std::setw(5)  << "Br"
          << std::setw(35) << "Branch"
          << std::setw(30) << "Type"
          << std::right
          << std::setw(12) << "File Size"
          << std::setw(10) << "Comp."
          << std::endl;

    std::cout << std::string(92, '-') << std::endl;

    for (int i = 0; i < branches->GetEntries(); ++i) {
      
      TBranch *b = (TBranch*)branches->At(i);
    
      std::string type = b->GetClassName();
      if (type.empty()) {
        if (TLeaf *leaf = (TLeaf*)b->GetListOfLeaves()->At(0))
	  type = leaf->GetTypeName();
      }
      
      Long64_t zip = b->GetZipBytes();
      Long64_t tot = b->GetTotBytes();
      double comp = zip ? double(tot)/zip : 0.0;
      
      std::cout << std::left
		<< std::setw(5)  << i
		<< std::setw(35) << b->GetName()
		<< std::setw(30) << type
		<< std::right
		<< std::setw(12) << zip
		<< std::setw(10) << std::fixed << std::setprecision(2) << comp
		<< '\n';
    }

    f->Close();
}
