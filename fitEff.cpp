#include "TMath.h"
#include <TH2D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <iostream>
#include <vector>
#include <exception>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <ctime>

int fitEff() {

  // Read in file
  //  RAT::DU::DSReader dsreader("./testPartialFitter.root");
  RAT::DU::DSReader dsreader("/data/snoplus/parkerw/ratSimulations/batch/Nov17_multipdf_multipath_3MeVredone/input_*.root");
  //RAT::DU::DSReader dsreader("/home/parkerw/Software/rat_b/testEff.root");
  int xBad=0;
  int yBad=0;
  int zBad=0;
  int anyBad=0;
  bool anyBadflag=false;
  int nEv=0;
  int nFit=0;

  TH1D *hX = new TH1D("hX", "hX", 120, -6000, 6000);
  TH1D *hY = new TH1D("hY", "hY", 120, -6000, 6000);
  TH1D *hZ = new TH1D("hZ", "hZ", 60, 0, 6000);
  TH1D *hR = new TH1D("hR", "hR", 60, 0, 6000);
  TH1D *hRFill = new TH1D("hRFill", "hRFill", 60, 0, 6000);
 
  TH1D *hXAll = new TH1D("hXAll", "hXAll", 120, -6000, 6000);
  TH1D *hYAll = new TH1D("hYAll", "hYAll", 120, -6000, 6000);
  TH1D *hZAll = new TH1D("hZAll", "hZAll", 60, 0, 6000);
  TH1D *hRAll = new TH1D("hRAll", "hRAll", 60, 0, 6000);
  TH1D *hRFillAll = new TH1D("hRFillAll", "hRFillAll", 60, 0, 6000); 

  for(int i=0; i<dsreader.GetEntryCount();i++){
   
    RAT::DS::Entry& rds = dsreader.GetEntry(i);
    if(i%100==0)
      std::cout<< "Event " << i << " of " << dsreader.GetEntryCount() << std::endl;
    int nevC = rds.GetEVCount();
    RAT::DS::MC mc = rds.GetMC();
    
    for(int iev=0;iev<1; iev++){
      nEv++;
      RAT::DS::EV& rev = rds.GetEV(iev);
      std::vector<std::string> fitnames = rev.GetFitNames();

      if(fitnames.at(0) == "MultiPDFMethod" || fitnames.at(fitnames.size()-1) == "MultiPDFMethod" ){
	anyBadflag=false;
	nFit++;
	RAT::DS::FitResult fitResult = rev.GetFitResult( "MultiPDFMethod" );
	RAT::DS::FitVertex fitVertex = fitResult.GetVertex(0);

	hXAll->Fill(mc.GetMCParticle(0).GetPosition().X());
	hYAll->Fill(mc.GetMCParticle(0).GetPosition().Y());
	hZAll->Fill(mc.GetMCParticle(0).GetPosition().Z());
	hRAll->Fill(mc.GetMCParticle(0).GetPosition().Mag());
	TVector3 posToFillLevel(mc.GetMCParticle(0).GetPosition().X(), mc.GetMCParticle(0).GetPosition().Y(), mc.GetMCParticle(0).GetPosition().Z()-747.48);
	hRFillAll->Fill(posToFillLevel.Mag());

	if(abs(fitVertex.GetPosition().X()) >8000){
	  xBad++;
	  anyBadflag=true;
	  }
	if(abs(fitVertex.GetPosition().Y()) >8000){
	  yBad++;
	  anyBadflag=true;
	}
        if(abs(fitVertex.GetPosition().Z()) >8000){
	  zBad++;
	  anyBadflag=true;
	}
	if(anyBadflag){
	  anyBad++;
	  hX->Fill(mc.GetMCParticle(0).GetPosition().X());
	  hY->Fill(mc.GetMCParticle(0).GetPosition().Y());
	  hZ->Fill(mc.GetMCParticle(0).GetPosition().Z());
	  hR->Fill(mc.GetMCParticle(0).GetPosition().Mag());
	  // TVector3 posToFillLevel(mc.GetMCParticle(0).GetPosition().X(), mc.GetMCParticle(0).GetPosition().Y(), mc.GetMCParticle(0).GetPosition().Z()-747.48);
	  hRFill->Fill(posToFillLevel.Mag());
	  
	}
      }
    }
  }
  /*
  TCanvas* cx = new  TCanvas("cx","cx", 1000,800);
  hX->Draw();
  TCanvas* cy = new  TCanvas("cy","cy", 1000,800);
  hY->Draw();
  TCanvas* cz = new  TCanvas("cz","cz", 1000,800);
  hZ->Draw();
  TCanvas* cr = new  TCanvas("cr","cr", 1000,800);
  hR->Draw();
  TCanvas* crfill = new  TCanvas("crfill","crfill", 1000,800);
  hRFill->Draw()*/;

  std::cout << "Of " << nEv << " events: " << std::endl << nFit << " Fit " << std::endl << xBad << " xBad " << std::endl << yBad << " yBad " << std::endl << zBad << " zBad " << std::endl << anyBad << " anyBad " << std::endl;

  TFile* outfile = new TFile("out.root","RECREATE");

  hX->Write();
  hY->Write();
  hZ->Write();
  hR->Write();
  hRFill->Write();
  hXAll->Write();
  hYAll->Write();
  hZAll->Write();
  hRAll->Write();
  hRFillAll->Write();
  
}
