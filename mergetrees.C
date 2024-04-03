#include <RAT/DS/Meta.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/DSReader.hh>
#include <RAT/DS/EV.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/GeoUtils.hh>

int mergetrees(){

  TList *list1 = new TList;
  TList *list2 = new TList;

  TFile* file1 = new TFile("/home/parkerw/Software/rat_fresh/output1.ds.root","open");
  TTree* t1 = (TTree*)file1->Get("T");
  TTree* outputTree1 = t1->CloneTree();
  outputTree1->SetName("t1");
  //list->Add(t1);

  TFile* file2 = new TFile("/home/parkerw/Software/rat_fresh/output2.ds.root","open");
  TTree* t2 = (TTree*)file2->Get("T");
  TTree* outputTree2 = t2->CloneTree();
  outputTree2->SetName("t2");
  list1->Add(outputTree2);

  TFile* file3 = new TFile("/home/parkerw/Software/rat_fresh/output3.ds.root","open");
  TTree* t3 = (TTree*)file3->Get("T");
  TTree* outputTree3 = t3->CloneTree();
  outputTree3->SetName("t3");
  list1->Add(outputTree3);

  outputTree1->CopyEntries(outputTree2);

  TTree* t4 = (TTree*)outputTree1->CloneTree();

  t4->CopyEntries(outputTree3);
  

  std::cout << t4->GetEntries() << std::endl;

  RAT::DU::DSReader* fDSReader = new RAT::DU::DSReader( "/home/parkerw/Software/rat_fresh/output*.ds.root" );

  for(int i=0; i<t4->GetEntries(); i++ ){

    const RAT::DS::Entry& rDS = fDSReader->GetEntry( i);
    const RAT::DS::EV& ev = rDS.GetEV(0);

    int run = rDS.GetRunID();
    int GTID = ev.GetGTID();

    std::cout << run << " " << GTID << std::endl;

  }
  return 0;
}
