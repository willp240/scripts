

int testdelfile(){

  std::vector<std::string> filenames;

  std::vector<TFile *> filevec;
  std::vector<TTree *> treevec;
  
  filenames.push_back("/data/snoplus3/griddata/Processing_7_0_8_Preliminary_Scintillator_Gold_300000_308097/ntuples/Analysis20R_r0000300079_s003_p000.ntuple.root");
  filenames.push_back("/data/snoplus3/griddata/Processing_7_0_8_Preliminary_Scintillator_Gold_300000_308097/ntuples/Analysis20R_r0000300079_s004_p000.ntuple.root");
  filenames.push_back("/data/snoplus3/griddata/Processing_7_0_8_Preliminary_Scintillator_Gold_300000_308097/ntuples/Analysis20R_r0000300079_s005_p000.ntuple.root");  

  for(int i=0; i<3; i++){

    TString fname = Form("./test%d.root",i);
    
    TFile *outFile = new TFile(fname, "recreate");
    filevec.push_back(outFile);

    TFile *inFile = new TFile(filenames.at(i).c_str(), "read");
    TTree *newTree = (TTree *)inFile->Get("output");

    treevec.push_back(newTree);
    
  }

  std::cout << "made vec " << std::endl;
  
  for(int i=0; i<3; i++){

    filevec.at(i)->cd();
    treevec.at(i)->Write();
    filevec.at(i)->Close();
    delete filevec.at(i);
    delete treevec.at(i);
  }


  return 0;
}
