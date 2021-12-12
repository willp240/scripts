#include <TH2D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TCanvas.h>


int PhotonRatio() {

  //make histogram vector (two for each energy)
  std::vector<TH1D*> scintHistVec;
  std::vector<TH1D*> cerenkovHistVec;

  Double_t energies[5] = {1,2,5,7,10};
  Double_t xerr[5] = {0,0,0,0,0};
  Double_t ratio[5] = {1,1,1,1,1};
  Double_t ratio_err[5] = {0,0,0,0,0};

  Double_t mins[5] = {9000,20000,55000,77000,110000 };
  Double_t maxs[5] = {15000,26000,65000,87000,120000 };
  Double_t minc[5] = {0,500,0,2500,3000 };
  Double_t maxc[5] = {1000,1500,5000,7500,8000 };

  TFile* outfile = new TFile("photonratio.root", "RECREATE");

  TH1D* hratio = new TH1D("hratio","Ratio of Scintillation to Cerenkov Photons",50,0,1000);
  TGraphErrors *gr  = new TGraphErrors(5,energies,ratio,xerr,ratio_err);
  gr->SetTitle("ratiograph");
  gr->SetMarkerStyle(2);
  gr->SetMarkerColor(kBlue);
  gr->SetLineColor(kBlue);

  TGraphErrors *gr2  = new TGraphErrors(5,energies,ratio,xerr,ratio_err);
  gr2->SetTitle("scintgraph");
  gr2->SetMarkerStyle(2);
  gr2->SetMarkerColor(kRed);
  gr2->SetLineColor(kRed);

  TGraphErrors *gr3  = new TGraphErrors(5,energies,ratio,xerr,ratio_err);
  gr3->SetTitle("cerenkgraph");
  gr3->SetMarkerStyle(2);
  gr3->SetMarkerColor(kBlack);
  gr3->SetLineColor(kBlack);

  //loop over energies{
  for(int i_energy=0; i_energy<5; i_energy++){
    std::cout << energies[i_energy] << " MeV " << std::endl;
    //declare histograms properly
    TString hsname = Form("%.0fMeVScintillation",energies[i_energy]);
    TH1D* hscint = new TH1D(hsname,hsname,50,mins[i_energy],maxs[i_energy]);
    TString hcname = Form("%.0fMeVCerenkov",energies[i_energy]);
    TH1D* hcerenk = new TH1D(hcname,hcname,50,minc[i_energy],maxc[i_energy]);

    // Read in file
    TString fname = Form("/data/snoplus/parkerw/ratSimulations/batch/Sep16_centre_wCerenk_%.0fMeV/*.root", energies[i_energy]);
    
    RAT::DU::DSReader dsreader(fname.Data());
    std::cout << dsreader.GetEntryCount() << std::endl;
    //loop over entries
    for(int i=0; i<1000;i++){
      RAT::DS::Entry& rds = dsreader.GetEntry(i);

      int nevC = rds.GetEVCount();
    
      //loop over events
      for(int iev=0;iev<nevC; iev++){

	RAT::DS::EV& rev = rds.GetEV(iev);
	RAT::DS::MC mc = rds.GetMC();
	
	//fill histogram of cerenkov photons
	double nCher = mc.GetNCherPhotons();
	hcerenk->Fill(nCher);
	//fill histogram of scintillation photons
	double nScint = mc.GetNScintPhotons();
	hscint->Fill(nScint);

      }
    }
    
    //push back histograms to vector
    cerenkovHistVec.push_back(hcerenk);
    scintHistVec.push_back(hscint);

    //Plot the two histograms, save canvas save histos
    TString cname = Form("Canvas%.0fMeV",energies[i_energy]);
    TCanvas* c1 = new TCanvas(cname, cname, 1500,500);
    c1->Divide(2,1);

    c1->cd(1);
    hscint->GetXaxis()->SetTitle("Number of Scintillation Photons");
    hscint->GetYaxis()->SetTitle("Events");
    TF1 *f1 = new TF1("f1","gaus");
    f1->SetLineColor(kRed);
    hscint->Fit(f1);
    hscint->Draw();

    c1->cd(2);
    hcerenk->GetXaxis()->SetTitle("Number of Cerenkov Photons");
    hcerenk->GetYaxis()->SetTitle("Events");
    TF1 *f2 = new TF1("f2","gaus");
    f2->SetLineColor(kRed);
    hcerenk->Fit(f2);
    hcerenk->Draw();
    
    //Get mean of each histo, take ratio and plot}
    double r = f1->GetParameter(1)/f2->GetParameter(1);
    double rerr = ( (f1->GetParError(1)/f1->GetParameter(1)) + (f2->GetParError(1)/f2->GetParameter(1)) ) * r;
    gr->SetPoint(i_energy,energies[i_energy],r);
    gr->SetPointError(i_energy,0,rerr);

    gr2->SetPoint(i_energy,energies[i_energy],f1->GetParameter(1));
    gr2->SetPointError(i_energy,0,f1->GetParError(1));

    gr3->SetPoint(i_energy,energies[i_energy],f2->GetParameter(1));
    gr3->SetPointError(i_energy,0,f2->GetParError(1));

    hscint->Write();
    hcerenk->Write();
    c1->Write();
  }
  //save plot of ratios
  gr->Write("ratiograph");
  gr2->Write("scintgraph");
  gr3->Write("cerenkgraph");
}
