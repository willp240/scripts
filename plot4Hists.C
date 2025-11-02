#include <sstream>
#include <string>

void plot4Hists(  )
{

  std::string filepath1 = "/data/snoplus/parkerw/antinu/Oct1_ReacNoSyst/asimov_dists/reactor_nubar_dataset1.root";
  std::string filepath2 = "/data/snoplus/parkerw/antinu/Oct1_Reac1.0/asimov_dists/reactor_nubar_dataset1.root";
  std::string filepath3 = "/data/snoplus/parkerw/antinu/Oct1_Reac1.029/asimov_dists/reactor_nubar_dataset1.root";
  std::string filepath4 = "/data/snoplus/parkerw/antinu/Oct1_ANPRNoSyst/asimov_dists/alphan_PRecoil_dataset1.root";
  std::string filepath5 = "/data/snoplus/parkerw/antinu/Oct1_ANPR0.0/asimov_dists/alphan_PRecoil_dataset1.root";
  std::string filepath6 = "/data/snoplus/parkerw/antinu/Oct1_ANPR0.2/asimov_dists/alphan_PRecoil_dataset1.root";
  
  TFile *file1 = new TFile(filepath1.c_str(),"READ");
  TFile *file2 = new TFile(filepath2.c_str(),"READ");
  TFile *file3 = new TFile(filepath3.c_str(),"READ");
  TFile *file4 = new TFile(filepath4.c_str(),"READ");
  TFile *file5 = new TFile(filepath5.c_str(),"READ");
  TFile *file6 = new TFile(filepath6.c_str(),"READ");
    
  TH1D* h1 = (TH1D*)file1->Get("reactor_nubar");
  TH1D* h2 = (TH1D*)file2->Get("reactor_nubar");
  TH1D* h3 = (TH1D*)file3->Get("reactor_nubar");
  TH1D* h4 = (TH1D*)file4->Get("alphan_PRecoil");
  TH1D* h5 = (TH1D*)file5->Get("alphan_PRecoil");
  TH1D* h6 = (TH1D*)file6->Get("alphan_PRecoil");

  TCanvas *c1 = new TCanvas("c1", "c1", 900, 600);
  gStyle->SetOptStat(0);
  gStyle->SetLineWidth(2);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  h1->GetXaxis()->SetTitle("Energy, MeV");
  h1->GetYaxis()->SetTitle("Events");
  h1->SetLineColor(kBlue+2);
  h1->SetLineWidth(2);
  h1->SetTitle("Reactor IBD");

  h2->SetLineColor(kRed+2);
  h2->SetLineWidth(2);
  h2->SetLineStyle(2);

  h3->SetLineColor(kGreen+2);
  h3->SetLineWidth(2);
  h3->SetLineStyle(2);

  TLegend* t1 = new TLegend( 0.6, 0.6, 0.8, 0.8 );
  t1->AddEntry( h1, "No Systematic", "l" );
  t1->AddEntry( h2, "S = 1.0");
  t1->AddEntry( h3, "S = 1.029", "l" );
  t1->SetLineWidth(2);
  
  h1->Draw("");
  h2->Draw("same");
  h3->Draw("same");
  t1->Draw("same");

  TCanvas *c2 = new TCanvas("c2", "c2", 900, 600);
  gStyle->SetOptStat(0);
  gStyle->SetLineWidth(2);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  h4->GetXaxis()->SetTitle("Energy, MeV");
  h4->GetYaxis()->SetTitle("Events");
  h4->SetLineColor(kBlue+2);
  h4->SetLineWidth(2);
  h4->SetTitle("(#alpha,n) Proton Recoil");

  h5->SetLineColor(kRed+2);
  h5->SetLineWidth(2);
  h5->SetLineStyle(2);

  h6->SetLineColor(kGreen+2);
  h6->SetLineWidth(2);
  h6->SetLineStyle(2);
  
  TLegend* t2 = new TLegend( 0.6, 0.6, 0.8, 0.8 );
  t2->AddEntry( h4, "No Systematic");
  t2->AddEntry( h5, "a = 0.0");
  t2->AddEntry( h6, "a = 0.0671", "l" );
  t2->SetLineWidth(2);

  h4->Draw("");
  h5->Draw("same");
  h6->Draw("same");
  t2->Draw("same");
  

}
