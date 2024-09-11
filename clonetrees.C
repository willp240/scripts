
int clonetrees(){

  TFile* infile = new TFile("/data/snoplus3/weiii/antinu/mycuts/Ntuple_data/data/300733.ntuple.root","open");
  TTree* outputTree1 = (TTree*)infile->Get("PromptT")->CloneTree();
  //outputTree1->SetName("PromptT");
  
  TFile* outfile = new TFile("output.root","RECREATE");
  outputTree1->Write();
  
  return 0;
}
