#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TObjArray.h>
#include <cstdio>

void branchSizes(const char* fname) {
  TFile *f = TFile::Open(fname, "READ");
  if (!f || f->IsZombie()) { printf("Failed to open %s\n", fname); return; }

  TTree *t = (TTree*)f->Get("T");
  if (!t) { printf("No 'T' tree\n"); f->ls(); return; }

  auto *branches = t->GetListOfBranches();
  for (int i = 0; i < branches->GetEntries(); ++i) {
    auto *br = (TBranch*)branches->At(i);
    printf("%-80s %10.1f MB\n", br->GetName(), br->GetTotBytes()/1024.0/1024.0);
  }

  f->Close();
  delete f;
}
