void loopfile() {

  const char* inDir = "/data/snoplus/parkerw/bb_sigex_all_3Feb/e_r_class_ntups_pdf/projections";
  char* dir = gSystem->ExpandPathName(inDir);
  void* dirp = gSystem->OpenDirectory(dir);
  const char* ext = ".root";

  const char* entry;
  const char* filename[100];
  Int_t n = 0;
  TString str;

  while((entry = (char*)gSystem->GetDirEntry(dirp))) {
    str = entry;
    if(str.EndsWith(ext))
      filename[n++] = gSystem->ConcatFileName(dir, entry);
  }

  TCanvas c1;
  c1.Print("prefit1ds_all.pdf[");
  for (Int_t i = 0; i < n; i++){
    TFile *f1 = TFile::Open(filename[i]);
    TIter next(f1->GetListOfKeys());
    TKey *key;

    while ((key = (TKey*)next())) {
      TClass *cl = gROOT->GetClass(key->GetClassName());
      if (!cl->InheritsFrom("TH1")) continue;
      TH1 *h = (TH1*)key->ReadObj();
      h->Draw();
      h->SetTitle(filename[i]);
      c1.Print("prefit1ds_all.pdf");
    }
  }
  c1.Print("prefit1ds_all.pdf]");
}

