#include <RAT/DU/DSReader.hh>
#include <RAT/DS/Entry.hh>

#include <TH1D.h>
#include <TFile.h>
#include <TVector3.h>
#include <TF1.h>
#include <TCanvas.h>
#include <vector>
#include <TStyle.h>

double GetError( double, double, double, double);

void photonThinPlotErrLTScan()
{

  gStyle->SetOptStat(0);

  TFile *outtfile = new TFile("Jul20_Thinning_ScanCutoff.root","RECREATE");

  TCanvas* c1 = new TCanvas("c1", "c1", 1600,800);
  c1->Divide(4,2,0.01,0.05);
  //  c1->Divide(4,2);

  //  vector<int> cutoff{12,30,50,100,250,500,1000,10000};

  const size_t N = 8;
  int a[N] = {12,30,50,100,250,500,1000,10000};
  std::vector<int> cutoff( a, a + N );

  for(int cutNum = 0; cutNum<cutoff.size(); cutNum++){
    
    c1->cd(cutNum+1);

    TString eventFile;
    TString histoName = Form("MCPEhisto_%d",cutoff[cutNum]);
    TString histoTitle = Form("Gaussian Approximation Cutoff %d; Photon Thinning Factor; #Delta PE Count", cutoff[cutNum]);
    TH1D* MCPEhisto = new TH1D(histoName, histoTitle, 31, -0.1, 6.1);
    MCPEhisto->Sumw2();
    
    double thinFactorBase = 1;
    eventFile = Form("/data/snoplus/parkerw/ratSimulations/batch/Jul20_Thinning_FullScan/photon_thinning_%.1f_%d.root", thinFactorBase, cutoff[cutNum]);

    RAT::DU::DSReader* dsReaderBase = new RAT::DU::DSReader(eventFile.Data());
    TH1D *hMCPEsBase = new TH1D("hMCPEs", "Simulated photoelectrons with photon thinning;MCPE count;Events per bin", 25000, 0, 5000);
    TF1 *g1Base = new TF1("g1Base","gaus",0,5000);
    for( size_t iEntry = 0; iEntry < dsReaderBase->GetEntryCount(); iEntry++ )
      {
        const RAT::DS::Entry& rDS = dsReaderBase->GetEntry( iEntry );
	hMCPEsBase->Fill(rDS.GetMC().GetMCPECount());
      }

    hMCPEsBase->Fit(g1Base);
    double basePEMean=1, baseNHitsMean=1, basePEError =0;
    basePEMean = g1Base->GetParameter(1);
    basePEError = g1Base->GetParError(1);
    
    for(int i=0; i<21; i++){
      
      double thinFactor = 0.8 + i*0.2;
      eventFile = Form("/data/snoplus/parkerw/ratSimulations/batch/Jul20_Thinning_FullScan/photon_thinning_%.1f_%d.root", thinFactor, cutoff[cutNum]);

      std::cout << eventFile.Data() << " " << thinFactor << std::endl;

      RAT::DU::DSReader* dsReader = new RAT::DU::DSReader(eventFile.Data());

      TH1D *hMCPEs = new TH1D("hMCPEs", "Simulated photoelectrons with photon thinning;MCPE count;Events per bin", 25000, 0, 5000);
      TF1 *g1 = new TF1("g1","gaus",0,5000);
      
      for( size_t iEntry = 0; iEntry < dsReader->GetEntryCount(); iEntry++ )
	{
	  const RAT::DS::Entry& rDS = dsReader->GetEntry( iEntry );
	  hMCPEs->Fill(rDS.GetMC().GetMCPECount());
	}

      hMCPEs->Fit(g1);
      
      MCPEhisto->SetBinContent(i+5, (g1->GetParameter(1) - basePEMean) / basePEMean);
      if(g1->GetParameter(1) == basePEMean)
	MCPEhisto->SetBinContent(i+5,0.00001);
      MCPEhisto->SetBinError(i+5, GetError( basePEMean, basePEError, g1->GetParameter(1), g1->GetParError(1)) );
    
      delete hMCPEs;
      }
    
    MCPEhisto->GetXaxis()->SetRangeUser(0.5,3.5);
    MCPEhisto->GetYaxis()->SetRangeUser(-0.025,0.01);
    MCPEhisto->GetYaxis()->SetTitleOffset(1.3);
    TF1  *f1 = new TF1("f1","pol1",0.5,3.5);
    f1->SetLineColor(kRed);
    MCPEhisto->Fit(f1);
    MCPEhisto->Draw();
  
    outtfile->cd();
    MCPEhisto->Write();
    delete hMCPEsBase;
    delete g1Base;

  }

  outtfile->cd();
  c1->Write();
  outtfile->Close();
}


double GetError( double baseMean, double baseError, double mean, double error) {

  if( abs(baseMean - mean) < 0.00001)
    return 0;
  
  double binContent = ( mean - baseMean ) / baseMean;
  double sumError = ( error * error + baseError * baseError ) / ( ( mean - baseMean ) * ( mean - baseMean ) );
  double quotientError = ( baseError / baseMean ) * ( baseError / baseMean );

  return sqrt ( ( binContent * binContent) * ( sumError + quotientError ) );
}
