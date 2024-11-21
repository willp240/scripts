#include <sstream>
#include <string>

void plotAntinuAsmvDist(  )
{

   // strings for each bg file paths
  std::string reacibdfilepath = "/data/snoplus3/parkerw/antinu/pruned_pdf/reactor_nubar.root";
  std::string geothibdfilepath = "/data/snoplus3/parkerw/antinu/pruned_pdf/geonu_Th.root";
  std::string geouibdfilepath = "/data/snoplus3/parkerw/antinu/pruned_pdf/geonu_U.root";
  std::string alphanfilepath = "/data/snoplus3/parkerw/antinu/pruned_data/Prod_7_0_8_9_Preliminary_Scintillator_Gold/alphan_CScatter.root";

  TFile *reacfile = new TFile(reacibdfilepath.c_str(),"READ");
  TFile *geothfile = new TFile(geothibdfilepath.c_str(),"READ");
  TFile *geoufile = new TFile(geouibdfilepath.c_str(),"READ");
  TFile *alphanfile = new TFile(alphanfilepath.c_str(),"READ");
  
  // delcare trees for each (in a vector)
  TTree* reactree = (TTree*)reacfile->Get("output");
  TTree* geothtree = (TTree*)geothfile->Get("output");;
  TTree* geoutree = (TTree*)geoufile->Get("output");;
  TTree* alphantree = (TTree*)alphanfile->Get("output");;

  double maxy = 3.0;
  double minx = 0.9;
  double maxx = 8;
  double numbins = 71;
  
  TH1F *htemp = new TH1F("htemp", "Energy", numbins, minx, maxx);
  TH1F *htemp2 = new TH1F("htemp2", "Energy", numbins, minx, maxx);
  TH1F *htemp3 = new TH1F("htemp3", "Energy", numbins, minx, maxx);
  TH1F *htemp4 = new TH1F("htemp4", "Energy", numbins, minx, maxx);
  TH1F *htotal = new TH1F("htotal", "Energy", numbins, minx, maxx);
  
  // declare canvas and histograms
  TCanvas *c = new TCanvas("canv", "canv", 900, 600);
  gStyle->SetOptStat(0);
  gStyle->SetLineWidth(2);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  // for each tree draw energy
  reactree->Draw("energy >> htemp");
  htemp->Scale(27.9/htemp->Integral());
  htemp->GetXaxis()->SetTitle("Energy, MeV");
  htemp->GetYaxis()->SetTitle("Events");
  htemp->SetLineColor(kBlue);
  htemp->SetLineWidth(2);
  htemp->GetYaxis()->SetRangeUser(0,maxy);
  
  geoutree->Draw("energy >> htemp2");
  htemp2->Scale(5.36/htemp2->Integral());
  htemp2->GetXaxis()->SetTitle("Energy, MeV");
  htemp2->GetYaxis()->SetTitle("Events");
  htemp2->GetYaxis()->SetRangeUser(0,maxy);
  htemp2->SetLineColor(kMagenta+2);
  htemp2->SetLineWidth(2);

  geothtree->Draw("energy >> htemp3");
  htemp3->GetYaxis()->SetRangeUser(0,maxy);
  htemp3->GetXaxis()->SetTitle("Energy, MeV");
  htemp3->GetYaxis()->SetTitle("Events");
  htemp3->SetLineColor(kMagenta);
  htemp3->SetLineWidth(2);
  htemp3->Scale(1.44/htemp3->Integral());
  
  alphantree->Draw("energy >> htemp4");
  htemp4->GetYaxis()->SetRangeUser(0,maxy);
  htemp4->GetXaxis()->SetTitle("Energy, MeV");
  htemp4->GetYaxis()->SetTitle("Events");
  htemp4->SetLineColor(kRed);
  htemp4->SetLineWidth(2);
  htemp4->Scale(19.8/htemp4->Integral());

  htemp2->Add(htemp3);
  
  htotal->Add(htemp);
  htotal->Add(htemp2);
  htotal->Add(htemp4);
  htotal->GetXaxis()->SetTitle("Energy, MeV");
  htotal->GetYaxis()->SetTitle("Events");
  htotal->GetYaxis()->SetRangeUser(0,maxy);
  htotal->SetLineColor(kBlack);
  htotal->SetLineWidth(2);
  
  TLegend* t1 = new TLegend( 0.6, 0.6, 0.8, 0.8 );
  t1->AddEntry( htotal, "Total MC Prediction");
  t1->AddEntry( htemp, "Reactor IBD", "l" );
  t1->AddEntry( htemp2, "Geo IBD", "l" );
  t1->AddEntry( htemp4, "Alpha N", "l" );
  t1->SetLineWidth(2);
  
  htemp->Draw("Hist");
  htemp2->Draw("Histsame");
  htemp4->Draw("Histsame");
  htotal->Draw("Histsame");
  t1->Draw("same");
}
