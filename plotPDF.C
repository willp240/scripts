
void plotPDF(std::string FileName1 ) {

  TCanvas *c = new TCanvas("canv", "canv", 900, 600);

  gStyle->SetOptStat(0);
  gStyle->SetLineWidth(2);
  gPad->SetFrameLineWidth(3);
  c->SetGrid();

  TFile *File1 = new TFile(FileName1.c_str(), "OPEN");

  TIter next(File1->GetListOfKeys());
  TKey *key;

  while ((key = (TKey*)next())) {
    
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl->InheritsFrom("TH2")) continue;

    TH2D *h1 = (TH2*)key->ReadObj()->Clone();//generated 
    h1->SetName("h1");
    h1->Scale(115442);
    h1->SetLineColor(kBlue);
    h1->SetLineWidth(2);
    h1->GetXaxis()->SetTitle("Energy, MeV");
    h1->GetYaxis()->SetTitle("Events");
    //    h1->ProjectionX()->Draw("");

    TH2D *h2 = (TH2*)key->ReadObj()->Clone();//asimov
    h2->SetName("h2");
    h2->Scale(16495.11);
    h2->SetLineColor(kRed);
    h2->SetLineWidth(2);
    h2->ProjectionX()->Draw("");

    //    TLegend *leg = new TLegend(0.3, 0.75, 0.5, 0.85);
    //leg->AddEntry(h1, "Generated MC", "l");
    //leg->AddEntry(h2, "Expected Rate", "l");
    //leg->Draw();
  }

  //  std::vector<int> nGeneratedEvents;
  //std::vector<int>* tempVec;
  //File1->GetObject("nGeneratedEvents",tempVec);
  //nGeneratedEvents = *tempVec;
  //File1->Close();
  //std::cout << nGeneratedEvents.at(0) << std::endl;

}

