#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TObjArray.h>
#include <TString.h>
#include <iostream>
#include <vector>
#include <algorithm>

// Struct to store branch info
struct BranchInfo {
  std::string fullName;
  Long64_t zipBytes;
};

// Recursive function to collect all subbranches
void CollectBranches(TBranch* br, const std::string& prefix, std::vector<BranchInfo>& allBranches) {
  if (!br) return;

  std::string fullName = prefix.empty() ? br->GetName() : prefix + "." + br->GetName();
  allBranches.push_back({fullName, br->GetZipBytes()});

  TObjArray* subBranches = br->GetListOfBranches();
  for (int i = 0; i < subBranches->GetEntries(); ++i) {
    TBranch* subBr = (TBranch*)subBranches->At(i);
    CollectBranches(subBr, fullName, allBranches);
  }
}

void TestBranches(const char* filename, const char* treename = "T") {
  TFile* file = TFile::Open(filename);
  if (!file || file->IsZombie()) {
    std::cerr << "Could not open file: " << filename << std::endl;
    return;
  }

  TTree* tree = nullptr;
  file->GetObject(treename, tree);
  if (!tree) {
    std::cerr << "Could not find tree: " << treename << std::endl;
    return;
  }

  std::vector<BranchInfo> allBranches;

  TObjArray* branches = tree->GetListOfBranches();
  for (int i = 0; i < branches->GetEntries(); ++i) {
    TBranch* br = (TBranch*)branches->At(i);
    CollectBranches(br, "", allBranches);
  }

  // Sort branches by size, descending
  std::sort(allBranches.begin(), allBranches.end(), [](const BranchInfo& a, const BranchInfo& b) {
    return a.zipBytes > b.zipBytes;
  });

  // Print result
  Long64_t totalSize = 0;
  std::cout << "\nBranch size breakdown (sorted by compressed size):\n";
  for (const auto& info : allBranches) {
    std::cout << std::setw(10) << info.zipBytes / (1E6) << " megabytes : " << info.fullName << std::endl;
    totalSize += info.zipBytes;
  }

  std::cout << "\nTotal compressed size of all branches: " << totalSize / (1E6) << " megabytes" << std::endl;

  delete file;
}
