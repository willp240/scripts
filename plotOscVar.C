
void plotOscVar( ) {

  TCanvas *c = new TCanvas("canv", "canv", 900, 600);

  gStyle->SetOptStat(0);
  gStyle->SetLineWidth(2);
  gPad->SetFrameLineWidth(3);
  c->SetGrid();

  TFile *File1 = new TFile("/data/snoplus3/parkerw/antinu/Oct4_nosysts/asimov.root", "OPEN");
  TFile *File2 = new TFile("/data/snoplus3/parkerw/antinu/Oct7_oscnom//asimov.root", "OPEN");
  TFile *File3 = new TFile("/data/snoplus3/parkerw/antinu/Oct7_osc_dm21nom_t1210sig/asimov.root", "OPEN");
  TFile *File4 = new TFile("/data/snoplus3/parkerw/antinu/Oct7_osc_dm2110sig_t12nom/asimov.root", "OPEN");
  TFile *File5 = new TFile("/data/snoplus3/parkerw/antinu/Oct7_osc_dm2110sig_t1210sig/asimov.root", "OPEN");
  
  TH1D* h1 = (TH1D*)File1->Get("asimov");
  TH1D* h2 = (TH1D*)File2->Get("asimov");
  TH1D* h3 = (TH1D*)File3->Get("asimov");
  TH1D* h4 = (TH1D*)File4->Get("asimov");
  TH1D* h5 = (TH1D*)File5->Get("asimov");
  
  h1->SetLineColor(kBlack);
  h2->SetLineColor(kRed+2);
  h3->SetLineColor(kBlue+2);
  h4->SetLineColor(kGreen+2);
  h5->SetLineColor(kYellow+2);

  h2->SetLineStyle(2);
  h3->SetLineStyle(2);
  h4->SetLineStyle(2);
  h5->SetLineStyle(2);
  h1->SetLineWidth(2);
  h2->SetLineWidth(2);
  h3->SetLineWidth(2);
  h4->SetLineWidth(2);
  h5->SetLineWidth(2);

  TLegend* leg = new TLegend(0.4,0.5,0.9,0.9);
  leg->AddEntry(h1,"No Oscillations","lf");
  leg->AddEntry(h2,"#Deltam_{21}^{2} = nom.; #theta_{12} = nom.","lf");
  leg->AddEntry(h3,"#Deltam_{21}^{2} = nom.; #theta_{12} = +10#sigma","lf");
  leg->AddEntry(h4,"#Deltam_{21}^{2} = +10#sigma;   #theta_{12} = nom.","lf");
  leg->AddEntry(h5,"#Deltam_{21}^{2} = +10#sigma;   #theta_{12} = +10#sigma","lf");
  
  h1->Draw();
  h2->Draw("same");
  h3->Draw("same");
  h4->Draw("same");
  h5->Draw("same");

  leg->Draw("same");
}
