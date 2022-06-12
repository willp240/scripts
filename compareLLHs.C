
void compareLLHs(std::string FileName1, std::string label1, std::string FileName2,  std::string label2, std::string outfile) {

  TFile *File1 = new TFile(FileName1.c_str(), "OPEN");
  TFile *File2 = new TFile(FileName2.c_str(), "OPEN");
  TCanvas *c = new TCanvas("canv", "canv", 1500, 1080);

  gStyle->SetOptStat(0);
  gStyle->SetLineWidth(2);
  gPad->SetFrameLineWidth(3);

  c->SetTopMargin(0.1);
  c->SetBottomMargin(0.08);
  c->SetLeftMargin(0.15);
  c->SetRightMargin(0.06);

  c->Print((outfile+".pdf[").c_str());
  c->cd();

  TPaveText *pt = new TPaveText(0.05, 0.05, 0.9, 0.9);
  pt->AddText((FileName1+"_"+FileName2).c_str());
  pt->Draw();
  c->Print((outfile+".pdf").c_str());

  TIter next(File1->GetListOfKeys());
  TKey *key;
  std::string name;

  // Loop through all entries
  while ((key = (TKey*)next())) {

    std::string name = std::string(key->GetName());
    std::string classname = std::string(key->GetClassName());
    
    if (classname == "TH1D") {
      TH1D *plot2 = (TH1D*)File2->Get(name.c_str())->Clone();
      plot2->SetName("h2");
      plot2->SetTitle("h2");
      TH1D *plot1 = (TH1D*)File1->Get(name.c_str())->Clone();
      plot1->GetYaxis()->SetTitleOffset(2.0);
      plot1->SetLineColor(kRed);
      plot2->SetLineColor(kBlue);
      plot1->SetLineStyle(1);
      plot2->SetLineStyle(2);
      plot1->SetLineWidth(2);
      plot2->SetLineWidth(2);
      TLegend *leg = new TLegend(0.3, 0.75, 0.5, 0.85);
      leg->AddEntry(plot1, label1.c_str(), "l");
      leg->AddEntry(plot2, label2.c_str(), "l");
      c->cd();
      c->SetGrid();
      plot1->Draw();
      plot2->Draw("same");
      gPad->Update();
      TLine *asimovline = new TLine(1.0, gPad->GetUymin(), 1.0, gPad->GetUymax() );
      asimovline->SetLineWidth(2);
      asimovline->Draw("same");
      leg->Draw();
      gPad->Update();
      c->Print((outfile+".pdf").c_str());
      delete plot1;
      delete plot2;
    }

  }

  c->Print((outfile+".pdf]").c_str());
}

