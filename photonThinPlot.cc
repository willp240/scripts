#include <RAT/DU/DSReader.hh>
#include <RAT/DS/Entry.hh>

#include <TH1D.h>
#include <TFile.h>
#include <TVector3.h>
#include <TF1.h>

void photonThinPlot()
{

  TString eventFile;
  TH1D* MCPEhisto = new TH1D("MCPEhisto", "Fractional difference in PE count vs thinning factor; Photon Thinning Factor; #Delta PE Count", 125, -0.04, 9.96);
  //TH1D* NHitshisto = new TH1D("NHitshisto", "Fractional difference in NHits vs thinning factor; Photon Thinning Factor;  #Delta NHits", 125, -.04, 9.96); 

  TFile *outtfile = new TFile("Jul11_Thinning_10x.root","RECREATE");

  double basePEMean=1, baseNHitsMean=1;

  for(int i=0; i<50; i++){

    double thinFactor = 1 + i*0.08;
    int date = 11;
    //    if(i > 49) date = 24; 
    //date = 1;
    if(i % 5 == 0)
      eventFile = Form("/data/snoplus/parkerw/ratSimulations/batch/Jul%d_Thinning_10x/photon_thinning_%.1f.root", date, thinFactor);
    else
      eventFile = Form("/data/snoplus/parkerw/ratSimulations/batch/Jul%d_Thinning_10x/photon_thinning_%.2f.root", date, thinFactor);

    std::cout << eventFile.Data() << " " << thinFactor << std::endl;

    RAT::DU::DSReader* dsReader = new RAT::DU::DSReader(eventFile.Data());

    //    TH1D *hMCPEs = new TH1D("hMCPEs", "Simulated photoelectrons with photon thinning;MCPE count;Events per bin", 12500, 0, 2500);
    TH1D *hMCPEs = new TH1D("hMCPEs", "Simulated photoelectrons with photon thinning;MCPE count;Events per bin", 25000, 0, 50000);
    //TH1D *hNHits = new TH1D("hNHits", "NHits with photon thinning;NHits ;Events per bin", 10000, 0, 2000);
    TF1 *g1 = new TF1("g1","gaus",0,2500);
    //TF1 *g2 = new TF1("g2","gaus",0,2000);

    for( size_t iEntry = 0; iEntry < dsReader->GetEntryCount(); iEntry++ )
      {
	const RAT::DS::Entry& rDS = dsReader->GetEntry( iEntry );
	//	std::cout << "b " << hNHits->Fill(rDS.GetEV(0).GetNhits()) <<std::endl;
	hMCPEs->Fill(rDS.GetMC().GetMCPECount());
	//	if (rDS.GetEV(0).GetNhits())
	//hNHits->Fill(rDS.GetEV(0).GetNhits());
      }

    hMCPEs->Fit(g1);
    //    hNHits->Fit(g2);
    if(i==0){
      basePEMean = g1->GetParameter(1);
      std::cout << basePEMean << std::endl;
      //baseNHitsMean = g2->GetParameter(1);
    }

    MCPEhisto->SetBinContent(i+14, (g1->GetParameter(1) - basePEMean) / basePEMean );
    MCPEhisto->SetBinError(i+14, g1->GetParError(1) / basePEMean);


    // NHitshisto->SetBinContent(i+14, (g2->GetParameter(1) - baseNHitsMean) / baseNHitsMean );
    //NHitshisto->SetBinError(i+14, g2->GetParError(1) / baseNHitsMean);
    delete hMCPEs;
    //delete hNHits;
  }

  MCPEhisto->Draw();
  //  NHitshisto->Draw();
  
  outtfile->cd();
  MCPEhisto->Write();
  //NHitshisto->Write();
  outtfile->Close();
}
