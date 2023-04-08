#include <TH1D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <TVector3.h>
#include <TStyle.h>

void Plot1RadialBias() {

  //  gStyle->SetOptStat(0);

  double vec = 186.262;
  TString fname = Form("/data/snoplus3/parkerw/old_ratsim/batch/Nov5RecoordMPDF_perf_2.5MeV/*.root");
  //TString fname = Form("/data/snoplus/parkerw/ratSimulations/batch/Aug24_MPDFRecoord_4m_perf2.5MeV/recoord_MPDF_0.root");
  TString hname = Form("h%.3f", vec);
  TString htitle = Form("%.3f mm/ns", vec);
  TH1D* h1 = new TH1D(hname, htitle, 200, -1000, 1000);
  
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
    
    for(int iev=0;iev<1; iev++){
      
      const RAT::DS::EV& rev = rds.GetEV(iev);
      const TVector3 fitPosition = rev.GetFitResult("multiPDFFit").GetVertex(0).GetPosition();
      //const TVector3 fitPosition = rev.GetFitResult("multiPDFFit").GetVertex(0).GetPosition();
      //bool val = rev.GetFitResult("positionTimeFit").GetVertex(0).GetValidPosition();

      //if(!rev.GetFitResult("multiPDFFit").GetVertex(0).ValidPosition() || fitPosition.Mag() > 5500)
      if(!rev.GetFitResult("multiPDFFit").GetVertex(0).ValidPosition() || fitPosition.Mag()>4000)
	continue;
      
      if(j%100 == 0)
	std::cout << "event " << j << ", vertex " << iev << " " << (fitPosition - eventPosition).Dot(fitPosition.Unit()) << std::endl;
      h1->Fill( (fitPosition - eventPosition).Dot(fitPosition.Unit()) );
      
    }
  }
  h1->GetXaxis()->SetTitle("Radial Bias, mm");
  h1->GetYaxis()->SetRangeUser(0,250);
  h1->GetYaxis()->SetTitle("Events");
  h1->Fit("gaus");
  h1->Draw();

}

