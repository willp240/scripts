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

int misreco() {

  // Read in file
  //  RAT::DU::DSReader dsreader("./testPartialFitter.root");
  RAT::DU::DSReader dsreader("/data/snoplus/parkerw/ratSimulations/batch/Nov19_multipdf_multipath_3MeV/multiPDF_*.root");

  //  TH2D* hScaledXPath = new TH2D("hScaledXPath","hScaledXPath",100,5,15,60,-6000,6000);
  TH2D* hScaledXPDF = new TH2D("hScaledXPDF","hScaledXPDF",100,5,15,20000,-10000,10000);
  //TH2D* hScaledYPath = new TH2D("hScaledYPath","hScaledYPath",100,5,15,60,-6000,6000);
  TH2D* hScaledYPDF = new TH2D("hScaledYPDF","hScaledYPDF",100,5,15,20000,-10000,10000);
  //TH2D* hScaledZPath = new TH2D("hScaledZPath","hScaledZPath",100,5,15,60,-6000,6000);
  TH2D* hScaledZPDF = new TH2D("hScaledZPDF","hScaledZPDF",100,5,15,20000,-10000,10000);
  //  TH2D* hScaledRPath = new TH2D("hScaledRPath","hScaledRPath",100,5,15,60,0,6000);
  //TH2D* hScaledRPDF = new TH2D("hScaledRPDF","hScaledRPDF",100,5,15,60,0,6000);

  TFile* outfile = new TFile("nov9MultiPDF_MultiPath_LogL_XX.root", "RECREATE");

  for(int i=0; i<dsreader.GetEntryCount();i++){
    RAT::DS::Entry& rds = dsreader.GetEntry(i);
    if(i%100==0)
      std::cout<< "Event " << i << " of " << dsreader.GetEntryCount() << std::endl;

    int nevC = rds.GetEVCount();

    for(int iev=0;iev<nevC; iev++){
      RAT::DS::EV& rev = rds.GetEV(iev);
      std::vector<std::string> fitnames = rev.GetFitNames();
      RAT::DS::MC mc = rds.GetMC();
        
      if(fitnames.size()>1){
	RAT::DS::FitResult fitResultPDF = rev.GetFitResult( "MultiPDFMethod" );
	RAT::DS::FitVertex fitVertexPDF = fitResultPDF.GetVertex(0);
	std::vector<std::string> fomnamesPDF = fitResultPDF.GetFOMNames();
	double LogLPDF = fitResultPDF.GetFOM("PositionPositionLogL");
	double scaleLogLPDF = double(fitResultPDF.GetFOM("PositionPositionLogL")/fitResultPDF.GetFOM("PositionPositionSelectedNHit"));
	double xbias = fitVertexPDF.GetPosition().X() - mc.GetMCParticle(0).GetPosition().X();
	double ybias = fitVertexPDF.GetPosition().Y() - mc.GetMCParticle(0).GetPosition().Y();
	double zbias = fitVertexPDF.GetPosition().Z() - mc.GetMCParticle(0).GetPosition().Z();
	hScaledXPDF->Fill(scaleLogLPDF, xbias);
	hScaledYPDF->Fill(scaleLogLPDF, ybias);
	hScaledZPDF->Fill(scaleLogLPDF, zbias);
	//	hScaledRPDF->Fill(scaleLogLPDF,sqrt( mc.GetMCParticle(0).GetPosition().X()*mc.GetMCParticle(0).GetPosition().X() + mc.GetMCParticle(0).GetPosition().Y()*mc.GetMCParticle(0).GetPosition().Y() + mc.GetMCParticle(0).GetPosition().Z()*mc.GetMCParticle(0).GetPosition().Z() ));
      }
    }
  }
  hScaledXPDF->Write();
  //  hScaledXPath->Write();
  hScaledYPDF->Write();
  //hScaledYPath->Write();
  hScaledZPDF->Write();
  //hScaledZPath->Write();
  //hScaledRPDF->Write();
  // hScaledRPath->Write();
}
