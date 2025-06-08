void TestEntry(const char* filename, const char* treename = "T", Long64_t threshold = 1000000) {
  TFile* file = TFile::Open(filename);
  TTree* tree = nullptr;
  file->GetObject(treename, tree);
  if (!tree) {
    std::cerr << "Tree " << treename << " not found.\n";
    return;
  }

  std::cout << tree->GetCacheSize() << std::endl;
  //  tree->GetEntry(tree->GetEntries()-1);
  
  Long64_t nEntries = tree->GetEntries();

  for(int i = 135000; i < 136000; ++i) {
    std::cout << i << std::endl;
    tree->GetEntry(i);

  }

  return;

  for (Long64_t i = 0; i < nEntries; ++i) {
    Long64_t nbytes = tree->GetEntry(i);
    if (nbytes > threshold) {
      std::cout << "Entry " << i << " is large: " << nbytes / (1E6) << " megabytes" << std::endl;
    }
  }

  delete file;
}
