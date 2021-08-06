void loopStack() {

  const char* inDir = "/data/snoplus/parkerw/bb_sigex/May_20_all/asimovdata/";
  char* dir = gSystem->ExpandPathName(inDir);
  void* dirp = gSystem->OpenDirectory(dir);
  const char* ext = ".root";

  const char* entry;
  const char* filename[100];
  TString mode[100];
  Int_t n = 0;
  TString str;

  //  int col[40] = { 1,2,3,4,5,6,7,8,9,40,41,42,42,44,45,46,47,30,31,32,33,34,35,36,38,39,28,28,27,26,12,20,25,49,21,48 };
  //  int col[40] = {600,632,400,416, 599,631,399,415, 598,630,398,414, 597,629,397,413, 596,628,396,412, 595,627,395,411, 594,626,394,410, 593,625,393,409, 592,624,392,408, 591,623,391,407 };

  int col[40] = { 632,631,630,628,626,623,622, 606,607,609,611,614,619, 603,601,599,597,594,591, 423,426,428,432,433,435, 415,413,412,410,407,406, 390,391,393,395,401,402    }; 

  while((entry = (char*)gSystem->GetDirEntry(dirp))) {
    str = entry;
    if(str.EndsWith(ext)){
      filename[n] = gSystem->ConcatFileName(dir, entry);
      mode[n] = str.ReplaceAll(".root","");
      n++;
    }
  }

  THStack *stack_y = new THStack("asimovStack_y", "asimovStack_y; $(R/R_{Eff.})^3$; Events");
  THStack *stack_x = new THStack("asimovStack_x", "asimovStack_x; $T^{#Beta#Beta}_{Eff.} MeV$; Events");
  std::cout << n << std::endl;

  TLegend *leg = new TLegend(0.69, 0.2, 0.89, 0.88);
  TLegend *leg2 = new TLegend(0.12, 0.2, 0.32, 0.88);

  int count = 0;

  TCanvas* c1 = new TCanvas("c1", "c1", 0, 0, 1600, 1024);
  c1->Print("prefitStack.pdf[");
  for (Int_t i = 0; i < n; i++){
    
    if(mode[i]=="asimovRates")
      continue;

    TFile *f1 = TFile::Open(filename[i]);
    TIter next(f1->GetListOfKeys());
    TKey *key;

    while ((key = (TKey*)next())) {
      TClass *cl = gROOT->GetClass(key->GetClassName());
      if (!cl->InheritsFrom("TH1")) continue;
      TH2D *h2 = (TH2*)key->ReadObj();
      h2->SetTitle(mode[i]);

      if(h2->Integral() < 3 && mode[i] != "y88" && mode[i] != "co60" && mode[i] != "na22" && mode[i] != "i130m"){
	std::cout << "Ignore " << mode[i] << " total " << h2->Integral() << std::endl;
	continue;
      }

      std::cout << mode[i] << std::endl;

      TH1D* h_x =  h2->ProjectionX()->Clone();
      TH1D* h_y =  h2->ProjectionY()->Clone();
      h_x->SetTitle(mode[i]);
      h_y->SetTitle(mode[i]);

      h_x->GetXaxis()->SetTitle("$T^{#Beta#Beta}_{Eff.} MeV");
      h_y->GetXaxis()->SetTitle("$(R/R_{Eff.})^3$");
      h_x->GetYaxis()->SetTitle("Events");
      h_y->GetYaxis()->SetTitle("Events");

      h_x->SetFillColor(col[count]);
      h_x->SetLineColor(col[count]);
      h_y->SetFillColor(col[count]);
      h_y->SetLineColor(col[count]);

      leg->AddEntry(h_x,mode[i],"f");
      leg2->AddEntry(h_y,mode[i],"f");
      stack_x->Add(h_x);

      stack_y->Add(h_y);
      count++;
    }

  }

  stack_x->Draw();
  leg->Draw();
  c1->Print("prefitStack.pdf");
  //  stack_y->Draw();
  //leg2->Draw();
  //c1->Print("prefitStack.pdf");
  c1->Print("prefitStack.pdf]");
}

