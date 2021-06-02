void plotPDFs(std::string name) {

  std::string inDir = "/data/snoplus/parkerw/bb_sigex/"+name+"/e_r_class_ntups_pdf/";

  gStyle->SetPalette(51);
  gStyle->SetOptStat(0);

  //  std::string inDir = "/data/snoplus/parkerw/bb_sigex_0vb8nueb8numu_15Mar/e_r_class_ntups_pdf/";
  char* dir = gSystem->ExpandPathName(inDir.c_str());
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
  c1.Print((inDir+name+"_pdfplots.pdf[").c_str());
  for (Int_t i = 0; i < n; i++){
    TFile *f1 = TFile::Open(filename[i]);
    TIter next(f1->GetListOfKeys());
    TKey *key;

    while ((key = (TKey*)next())) {
      TClass *cl = gROOT->GetClass(key->GetClassName());
      if (!cl->InheritsFrom("TH1")) continue;

      std::string classname = std::string(key->GetClassName());
      if (classname == "TH2D") {
	TH2D *plot = (TH2D*)key->ReadObj();;
	plot->GetXaxis()->SetTitle("Energy, MeV");
	plot->GetYaxis()->SetTitle("(R/R_{eff})^3");
	plot->SetTitle(filename[i]);
	c1.cd();
	//	c1.SetTopMargin(0.16);
	c1.SetRightMargin(0.13);
	plot->SetMinimum(0);
	plot->Draw("colz");
	c1.Print((inDir+name+"_pdfplots.pdf").c_str());
	TH1D* plotX = plot->ProjectionX();
	plotX->SetMinimum(0);
	plotX->Draw("");
	c1.Print((inDir+name+"_pdfplots.pdf").c_str());
	TH1D* plotY = plot->ProjectionY();
	plotY->SetMinimum(0);
        plotY->Draw("");
	c1.Print((inDir+name+"_pdfplots.pdf").c_str());
	delete plot;
      }
    }
  }
  c1.Print((inDir+name+"_pdfplots.pdf]").c_str());
}

