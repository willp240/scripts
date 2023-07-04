#include <TTree.h>

void FillMyTree(TTree* t){

  bool x;

  t->SetBranchAddress("x", &x);

  for(int i=0; i<10; i++){
    x = i%2;
    //    t->Fill();
  }

}

int testTreeBranch(){

  TTree* t = new TTree("tname", "ttitle");

  t->Branch("x", 0 , "x/O");

  FillMyTree(t);

  t->Draw("x");

  return 0;  
}
