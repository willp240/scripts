#include <RAT/DU/DSReader.hh>
#include <RAT/DS/Entry.hh>

#include <TH1D.h>
#include <TFile.h>
#include <TVector3.h>
#include <TF1.h>
#include <TLegend.h>

void seedComp()
{

  TString eventFile1;
  TString eventFile2;
  TFile *outtfile = new TFile("seecCompThinFact3.root","RECREATE");

  eventFile1 = "/data/snoplus/parkerw/ratSimulations/batch/Jul7_ThinFact3_seed/photon_thinning_0.root";
  eventFile2 = "/data/snoplus/parkerw/ratSimulations/batch/Jul7_ThinFact3_seed_2/photon_thinning_0.root";

  RAT::DU::DSReader* dsReader1 = new RAT::DU::DSReader(eventFile1.Data());
  RAT::DU::DSReader* dsReader2 = new RAT::DU::DSReader(eventFile2.Data());

  TH1D *hMCPEs1 = new TH1D("hMCPEs1", "Simulated photoelectrons with photon thinning;MCPE count;Events per bin", 500, 0, 5000);
  TH1D *hMCPEs2 = new TH1D("hMCPEs2", "Simulated photoelectrons with photon thinning;MCPE count;Events per bin", 500, 0, 5000);

  for( size_t iEntry = 0; iEntry < dsReader1->GetEntryCount(); iEntry++ )
    {
	const RAT::DS::Entry& rDS1 = dsReader1->GetEntry( iEntry );
	hMCPEs1->Fill(rDS1.GetMC().GetMCPECount());
	//	std::cout << rDS1.GetMC().GetMCPECount() << " " << iEntry << std::endl;
    }

  for( size_t iEntry = 0; iEntry < dsReader2->GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS2 = dsReader2->GetEntry( iEntry );
      hMCPEs2->Fill(rDS2.GetMC().GetMCPECount());
    }


  //  hMCPEs1->SetLineColor(kRed);
  hMCPEs2->SetLineColor(kBlue);
  hMCPEs1->Draw();
  //hMCPEs2->Draw("same");

  TLegend* leg = new TLegend(.7,.8,.9,.9);
  leg->AddEntry(hMCPEs1, "Run 1", "l");
  leg->AddEntry(hMCPEs2, "Run 2", "l");
  //leg->Draw("Same");

  

  outtfile->cd();
  hMCPEs1->Write();
  hMCPEs2->Write();
  outtfile->Close();
}
