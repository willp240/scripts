#include <TH1D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <TVector3.h>
#include <TStyle.h>

void PlotRadialBias() {

  TCanvas* c1 = new TCanvas("c", "c", 1500,750);
  c1->Divide(5,3);

  gStyle->SetOptStat(0);

  for(int i=0; i<13; i++){

    c1->cd(i+1);
    float vec = 182 + i*0.5;
    TString fname = Form("/data/snoplus3/parkerw/ratSimulations/Apr7_5MeV_SEVRound1//Template_Macro_%.1f.root", vec);
    TString hname = Form("h%.1f", vec);
    TString htitle = Form("%.1f mm/ns", vec);
    TH1D* h1 = new TH1D(hname, htitle, 100, -500, 500);

    std::cout << vec << " " << fname << std::endl;

    // Read in file
    RAT::DU::DSReader dsreader(fname.Data());

    for(int j=0; j<dsreader.GetEntryCount();j++){

      const RAT::DS::Entry& rds = dsreader.GetEntry(j);
      int nevC = rds.GetEVCount();
      if(nevC<1){
	  std::cout << "no events" << std::endl;
	  continue;
	}
      const TVector3 eventPosition = rds.GetMC().GetMCParticle(0).GetPosition();
      //std::cout << std::endl;
      for(int iev=0;iev<1; iev++){
	
	const RAT::DS::EV& rev = rds.GetEV(iev);
	const TVector3 fitPosition = rev.GetFitResult("positionTimeFit").GetVertex(0).GetPosition();

	if(!rev.GetFitResult("positionTimeFit").GetVertex(0).ValidPosition() || fitPosition.Mag() > 5500)
	  continue;

	if(j%100 == 0)
	  std::cout << "event " << j << ", vertex " << iev << std::endl;

	h1->Fill( (fitPosition - eventPosition).Dot(fitPosition.Unit()) );

      }
    }
    h1->GetXaxis()->SetTitle("Fit R - True R, mm");
    h1->GetYaxis()->SetTitleOffset(1.3);
    h1->GetYaxis()->SetRangeUser(0,250);
    h1->GetYaxis()->SetTitle("Events");
    h1->Fit("gaus");
    h1->Draw();
  } // end loop over files
  //  c1->Draw();
}

