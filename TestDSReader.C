#include <TH2D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>

void BeginOfRun( RAT::DS::Run* fRun ){
  RAT::DB* db = RAT::DB::Get();
  db->BeginOfRun(*fRun);
  if (!fRun->GetMCFlag() || fRun->GetRunID() != 0) {
    db->LoadDefaultPlaneLocks();
  }
  db->Load( db->GetLink("DETECTOR")->GetS( "geo_file" ) );
  db->Load( db->GetLink("DETECTOR")->GetS( "pmt_info_file" ) );
  RAT::DU::Utility::Get()->BeginOfRun();
  db->EndOfBeginOfRun();
}



void TestDSReader() {

  /*  TChain* fT = new TChain("T");;
  
  TFileCollection * col = new TFileCollection();
  col->Add("/data/snoplus/parkerw/dsfiles/.root");
  //  fT->AddFileInfoList(dynamic_cast<TList*>(col->GetList()));
  
  
  std::vector<int> numEntriesVec;

  TIter next(col->GetList());
  TFileInfo* fi;
  int entryCount=0;
  
  while ((fi = (TFileInfo*)next())) {
    TString path = fi->GetCurrentUrl()->GetUrl();
    std::cout << "Opening file1: " << path << std::endl;

    TFile* f = TFile::Open(path);
    TChain* t = (TChain*)f->Get("T");
    numEntriesVec.push_back(t->GetEntries()+entryCount);
    entryCount = t->GetEntries();
    f->Close();
    }
  */
  /*
  TFileCollection * col2 = new TFileCollection();
  col2->Add("/data/snoplus/parkerw/dsfiles/.root");
  TIter next2(col2->GetList());
  
  while ((fi = (TFileInfo*)next2())) {
    TString path = fi->GetCurrentUrl()->GetUrl();
    std::cout << "Opening file: " << path << std::endl;
    fT->AddFile(path);
  }
  
  for(int i=0; i<numEntriesVec.size(); i++){
    std::cout << numEntriesVec.at(i) << std::endl;
    fT->GetEntry( numEntriesVec.at(i) );
  }

  //fT->GetEntry(fT->GetEntries()-1);
  for(int i=0; i<fT->GetEntries(); i++){
    fT->GetEntry(i);
    std::cout << "event " << i << std::endl;
   }
  */
 
  // Read in file
  // RAT::DU::DSReader dsreader("/home/parkerw/Software/rat_master/SNOP_0000356178_000_2ndpass.root");
  RAT::DU::DSReader dsreader("/data/snoplus/parkerw/dsfiles/*.root");
  for(int i=0; i<dsreader.GetEntryCount();i++){

    const RAT::DS::Entry& rds = dsreader.GetEntry(i);
    std::cout << "event " << i << ", vertex " << rds.GetEV(0).GetGTID() <<  std::endl;

    }
  /*
  std::string filename = "/home/parkerw/Software/rat_master/SNOP_0000356178_000_2ndpass.root" ;
  TFile* f = TFile::Open("/data/snoplus3/SNOplusData/processing/fullFill/rat-8.0.1/ratds/Analysis15_bMR_r0000360523_s003_p000.root");
  TIter next(f->GetListOfKeys());
  TKey* key;
  
  while ((key = (TKey*)next())) {
    TObject* obj = key->ReadObj();
    std::cout << key->GetName()
              << " (" << obj->ClassName() << ")"
              << " : " << key->GetNbytes() << " bytes on disk" << std::endl;
  }
  */
  /*
  // DSReader
  TChain* fT = new TChain("T");
  TChain* fRunT = new TChain("runT");

  //Add( filename );
  TFileCollection * col = new TFileCollection();
  col->Add(filename.c_str());
  fT->AddFileInfoList(dynamic_cast<TList*>(col->GetList()));
  fRunT->AddFileInfoList(dynamic_cast<TList*>(col->GetList()));
  delete col;

  RAT::DS::Entry* fDS = new RAT::DS::Entry();
  fT->SetBranchAddress("ds", &fDS);
  fT->GetEntry( 0 );

  RAT::DS::Run* fRun = new RAT::DS::Run();
  fRunT->SetBranchAddress( "run", &fRun );
  fRunT->GetEntry( 0 );
  //BeginOfRun(fRun);
    
  // Get Entry
  for(int i=0; i<fT->GetEntries(); i++){

    //const unsigned int previousRun = GetRun().GetRunID();
    fT->GetEntry(i);
  
    std::cout << "event " << i << std::endl;
   }
*/
}

