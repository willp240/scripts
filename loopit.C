TFile *File = NULL;
TCanvas *c = new TCanvas("canv", "canv", 1500, 1080);
std::string filename = "";

void LoopDirectory(TDirectory *dir) {
  TIter next(dir->GetListOfKeys());
  TKey *key;
  std::string name;

  // Loop through all entries
  while ((key = (TKey*)next())) {

    std::string name = std::string(key->GetName());
    std::string classname = std::string(key->GetClassName());
    std::cout << classname << std::endl;
    if (classname == "TDirectoryFile") {
      dir->cd(name.c_str());
      TDirectory *SubDir = gDirectory;
      LoopDirectory(SubDir);
    } 
    std::string dirname = std::string(dir->GetName());
    //    if (dirname.find(".root") != std::string::npos) continue;

    //name = dirname + "/" + name;
    // Select only data plots
    std::cout << name << std::endl;
    if (classname == "TCanvas") {
      TCanvas *canv = (TCanvas*)File->Get(name.c_str())->Clone();
      canv->SetTopMargin(0.06);
      canv->SetRightMargin(0.13);
      canv->Print((filename+".pdf").c_str());
      delete canv;
    } else if (classname == "TH2F") {
      std::cout << "in"<< std::endl;
      TH2D *plot = (TH2D*)File->Get(name.c_str())->Clone();
      c->cd();
      c->SetTopMargin(0.06);
      c->SetRightMargin(0.13);
      plot->Draw("colz");
      c->Print((filename+".pdf").c_str());
      delete plot;
    } else if (classname == "TH1F") {
      std::cout << "in"<< std::endl;
      TH1D *plot1 = (TH1D*)File->Get(name.c_str())->Clone();
      plot1->GetYaxis()->SetTitleOffset(2.0);
      c->cd();
      c->SetGrid();
      plot1->Draw();
      gPad->Update();
      c->Print((filename+".pdf").c_str());
      delete plot1;
    }

  }
}

void loopit(std::string FileName) {
  File = new TFile(FileName.c_str(), "OPEN");
  filename = FileName;

  gStyle->SetOptStat(0);

  c->SetTopMargin(0.1);
  c->SetBottomMargin(0.08);
  c->SetLeftMargin(0.15);
  c->SetRightMargin(0.06);

  c->Print((filename+".pdf[").c_str());
  c->cd();

  TPaveText *pt = new TPaveText(0.05, 0.05, 0.9, 0.9);
  pt->AddText(filename.c_str());
  pt->Draw();
  c->Print((filename+".pdf").c_str());

  LoopDirectory(File);

  c->Print((filename+".pdf]").c_str());
}
