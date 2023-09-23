#include <TH2D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>


void PlotBiases() {

  // Read in file
  RAT::DU::DSReader dsreader("/data/snoplus3/parkerw/ratSimulations/Jul24_004OldLLH_Grid/*root");

  TCanvas* c1 = new TCanvas("c1", "c1", 800,800);
  TCanvas* c2 = new TCanvas("c2", "c2", 800,800);
  TCanvas* c3 = new TCanvas("c3", "c3", 800,800);
  TCanvas* c4 = new TCanvas("c4", "c4", 800,800);

  TH1D* h1 = new TH1D("xbias", "xbias", 100, -500, 500);
  TH1D* h2 = new TH1D("ybias", "ybias", 100, -500, 500);
  TH1D* h3 = new TH1D("zbias", "zbias", 100, -500, 500);
  TH1D* h4 = new TH1D("rbias", "rbias", 100, -500, 500);


  for(int i=0; i<dsreader.GetEntryCount();i++){
    const RAT::DS::Entry& rds = dsreader.GetEntry(i);

    int nevC = rds.GetEVCount();

    for(int iev=0;iev<nevC; iev++){

      const RAT::DS::EV& rev = rds.GetEV(iev);
      //std::cout << "event " << i << ", vertex " << iev << std::endl;

      const RAT::DS::FitVertex& rVertex = rev.GetFitResult("multiPDFFit").GetVertex(0);

      TVector3 fitPos = rVertex.GetPosition();

      h1->Fill(fitPos.X());
      h2->Fill(fitPos.Y());
      h3->Fill(fitPos.Z()-4000);
      h4->Fill(fitPos.Mag()-4000);//sqrt(3000*3000 + 3000*3000 + 3000*3000));

    }
  }

  std::cout << h1->Integral() << " " << h2->Integral() << " " << h3->Integral() << " " << h4->Integral() << std::endl;

  c1->cd();
  h1->Draw();
  c2->cd();
  h2->Draw();
  c3->cd();
  h3->Draw();
  c4->cd();
  h4->Draw();

}
