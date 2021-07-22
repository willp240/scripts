#include <RAT/DU/DSReader.hh>
#include <RAT/DS/Entry.hh>

#include <TH1D.h>
#include <TFile.h>
#include <TVector3.h>
#include <TF1.h>

void photonThinPlot10x()
{

  TString eventFile, eventFile2;
  TFile *outtfile = new TFile("Jul15_Thinning_Poisson30Comp.root","RECREATE");

  int date = 15;
  
  eventFile = Form("/data/snoplus/parkerw/ratSimulations/batch/Jul%d_Thinning1_30poiss/photon_thinning*.root", date);
  eventFile2 = Form("/data/snoplus/parkerw/ratSimulations/batch/Jul%d_Thinning3_30poiss/photon_thinning*.root", date);

  //  eventFile = Form("/data/snoplus/parkerw/ratSimulations/batch/Jun%d_Thinning/photon_thinning_1.0.root", date);
  //eventFile2 = Form("/data/snoplus/parkerw/ratSimulations/batch/Jun%d_Thinning/photon_thinning_3.0.root", date);
  std::cout << eventFile << std::endl;
  RAT::DU::DSReader* dsReader = new RAT::DU::DSReader(eventFile.Data());
  RAT::DU::DSReader* dsReader2 = new RAT::DU::DSReader(eventFile2.Data());
  
  //    TH1D *hMCPEs = new TH1D("hMCPEs", "Simulated photoelectrons with photon thinning;MCPE count;Events per bin", 12500, 0, 2500);
  TH1D *hMCPEs = new TH1D("hMCPEs", "Simulated photoelectrons with photon thinning;MCPE count;Events per bin", 100, 0, 5000);
  TH1D *hMCPEs2 = new TH1D("hMCPEs2", "Simulated photoelectrons with photon thinning;MCPE count;Events per bin", 100, 0, 5000);

  for( size_t iEntry = 0; iEntry < dsReader->GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS = dsReader->GetEntry( iEntry );
      hMCPEs->Fill(rDS.GetMC().GetMCPECount());
    }
  
  for( size_t iEntry = 0; iEntry < dsReader2->GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS2 = dsReader2->GetEntry( iEntry );
      hMCPEs2->Fill(rDS2.GetMC().GetMCPECount());
    }
  
  
  hMCPEs->Draw();
  hMCPEs2->SetLineColor(kRed);
  hMCPEs2->Draw();
  
  outtfile->cd();
  hMCPEs->Write();
  hMCPEs2->Write();
  outtfile->Close();
}
