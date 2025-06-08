void TestEntryChain(const char* filename, const char* treename = "T", Long64_t threshold = 1000000) {

  TFile* file = TFile::Open(filename);
  TChain tree("T");

  tree.Add( filename );

  tree.SetCacheSize(10000);
  std::cout << tree.GetCacheSize() << std::endl;
  Long64_t* offsets = tree.GetTreeOffset();
  Int_t nFiles = tree.GetNtrees();
  for (int iFile = 0; iFile < nFiles; iFile++) {
    Long64_t lastEntry = (iFile + 1 < nFiles) ? offsets[iFile + 1] - 1 : tree.GetEntries() - 1;
    //    tree.GetEntry(lastEntry);
    std::cout << "got lastEntry " << lastEntry << std::endl;
  }
  // tree.GetEntry(tree.GetEntries()-1);
  
  Long64_t nEntries = tree.GetEntries();

  for(int i = 0; i < 200; ++i) {
    std::cout << i << std::endl;
    tree.GetEntry(i);

  }

  return;

  for (Long64_t i = 0; i < nEntries; ++i) {
    Long64_t nbytes = tree.GetEntry(i);
    if (nbytes > threshold) {
      std::cout << "Entry " << i << " is large: " << nbytes / (1E6) << " megabytes" << std::endl;
    }
  }

  delete file;
}
