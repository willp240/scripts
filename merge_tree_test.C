
int merge_tree_test(){

  TFile *f3 = TFile::Open("treefile3.root", "RECREATE");
  TTree *t3 = new TTree("T", "A tree");
  int eventNumber;
  int runNumber;

  t3->Branch("eventNumber", &eventNumber);
  t3->Branch("runNumber", &runNumber);
  for (int i_event = 0; i_event < 10; i_event++) {
    eventNumber = i_event;
    runNumber = 3;
    t3->Fill();
  }
  t3->Write();
  delete f3;

  TFile *f2 = TFile::Open("treefile2.root", "RECREATE");
  TTree *t2 = new TTree("T", "A tree");
  int eventNumber;
  int runNumber;

  t2->Branch("eventNumber", &eventNumber);
  t2->Branch("runNumber", &runNumber);
  for (int i_event = 0; i_event < 10; i_event++) {
    eventNumber = i_event;
    runNumber = 2;
    t2->Fill();
  }
  t2->Write();
  delete f2;

  TFile *f1 = TFile::Open("treefile1.root", "RECREATE");
  TTree *t1 = new TTree("T", "A tree");
  int eventNumber;
  int runNumber;

  t1->Branch("eventNumber", &eventNumber);
  t1->Branch("runNumber", &runNumber);
  for (int i_event = 0; i_event < 10; i_event++) {
    eventNumber = i_event;
    runNumber = 1;
    t1->Fill();
  }
  t1->Write();
  delete f1;

  TList* l = new TList;

  TFile *fo1 = TFile::Open("treefile1.root", "OPEN");
  TTree* tree1 = (TTree*) fo1->Get("T");
  TFile *fo2 = TFile::Open("treefile2.root", "OPEN");
  TTree* tree2 = (TTree*) fo2->Get("T");
  TFile *fo3 = TFile::Open("treefile3.root", "OPEN");
  TTree* tree3 = (TTree*) fo3->Get("T");

  l->Add(tree1);
  l->Add(tree2);
  l->Add(tree3);

  TTree *c = TTree::MergeTrees(l);

  int chainEventNum;
  int chainRunNum;

  c->SetBranchAddress( "eventNumber", &chainEventNum );
  c->SetBranchAddress( "runNumber", &chainRunNum );

  for(int i_entry = 0; i_entry < c->GetEntries(); i_entry++){
    c->GetEntry(i_entry);
    std::cout << "Run " << chainRunNum << ", Event " << chainEventNum << std::endl;
  }

  return 0;
}