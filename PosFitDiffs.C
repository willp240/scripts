#include <iostream>
#include <TLine.h>
#include <TStyle.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TAxis.h>


void PosFitDiffs( ) {

  gStyle->SetOptStat(0);
  gStyle->SetPalette(51);
  
  RAT::DB::Get()->SetAirplaneModeStatus(true);
  // DSReader on both sets of files
  // RAT::DU::DSReader dsreaderOrig("/data/snoplus/parkerw/ratSimulations/Apr20_1to10_e7/scint*25.root");
  //RAT::DU::DSReader dsreaderUpdated("/data/snoplus/parkerw/ratSimulations/May2_1to10_Quad500/scint*25.root");

  double mcx, mcy, mcz, mcr, mce, fitx1, fity1, fitz1, fitr1, fitx2, fity2, fitz2, fitr2;
  
  // Declare tree with branches: mcx mcy mcz mcr mce fitx1 fity1 fitz1 fitr1 fitx2 fity2 fitz2 fitr2
  TTree *tree = new TTree("T","events");
  tree->Branch("mcx", &mcx);
  tree->Branch("mcy", &mcy);
  tree->Branch("mcz", &mcz);
  tree->Branch("mcr", &mcr);
  tree->Branch("mce", &mce);
  tree->Branch("fitx1", &fitx1);
  tree->Branch("fity1", &fity1);
  tree->Branch("fitz1", &fitz1);
  tree->Branch("fitr1", &fitr1);
  tree->Branch("fitx2", &fitx2);
  tree->Branch("fity2", &fity2);
  tree->Branch("fitz2", &fitz2);
  tree->Branch("fitr2", &fitr2);

  for(int iFile = 0; iFile < 100; iFile++)
    {

      TString fname1 = Form("/data/snoplus/parkerw/ratSimulations/Apr20_1to10_e7/scintFitter_%d.root",iFile);
      TString fname2 = Form("/data/snoplus/parkerw/ratSimulations/May2_1to10_Quad500/scintFitterInput_%d.root",iFile);

      std::string f1 = std::string(fname1);
      std::string f2 = std::string(fname2);
      
      std::cout << f1 << std::endl;
      std::cout << f2 << std::endl;
      
      RAT::DU::DSReader dsreaderOrig(f1);
      RAT::DU::DSReader dsreaderUpdated(f2);
      
      if (dsreaderOrig.GetEntryCount() != dsreaderUpdated.GetEntryCount())
	std::cout << dsreaderOrig.GetEntryCount() << " " << dsreaderUpdated.GetEntryCount() << std::endl;

      for( size_t iEntry = 0; iEntry < dsreaderOrig.GetEntryCount(); iEntry++ )
	{
	  const RAT::DS::Entry& rDS1 = dsreaderOrig.GetEntry( iEntry );
	  const RAT::DS::Entry& rDS2 = dsreaderUpdated.GetEntry( iEntry );
	  
	  //	  std::cout << iEntry << std::endl;
	  
	  // Loop over events
	  for(size_t iEv = 0; iEv< rDS1.GetEVCount(); iEv++){
	    
	    // Get from file 1
	    const RAT::DS::EV& rev1 = rDS1.GetEV(iEv);
	    // Get from file 2
	    const RAT::DS::EV& rev2 = rDS2.GetEV(iEv);
	    
	    // Check MC xy,z,e matches in both
	    mcx = rDS1.GetMC().GetMCParticle(0).GetPosition().X();
	    mcy = rDS1.GetMC().GetMCParticle(0).GetPosition().Y();
	    mcz = rDS1.GetMC().GetMCParticle(0).GetPosition().Z();
	    mcr = rDS1.GetMC().GetMCParticle(0).GetPosition().Mag();
	    mce = rDS1.GetMC().GetMCParticle(0).GetKineticEnergy();
	    
	    double mcx2 = rDS2.GetMC().GetMCParticle(0).GetPosition().X();
	    double mcy2 = rDS2.GetMC().GetMCParticle(0).GetPosition().Y();
	    double mcz2 = rDS2.GetMC().GetMCParticle(0).GetPosition().Z();
	    double mcr2 = rDS2.GetMC().GetMCParticle(0).GetPosition().Mag();
	    double mce2 = rDS2.GetMC().GetMCParticle(0).GetKineticEnergy();
	    
	    if ( abs(mcx-mcx2) > 0.001 || abs(mcy-mcy2) > 0.001 || abs(mcz-mcz2) > 0.001 || abs(mce-mce2) > 0.001 )
	      {
		std::cout << "mcx1: " << mcx << std::endl;
		std::cout << "mcy1: " << mcy << std::endl;
		std::cout << "mcz1: " << mcz << std::endl;
		std::cout << "mcr1: " << mcr << std::endl;
		std::cout << "mce1: " << mce << std::endl;
		std::cout << "mcx2: " << mcx2 << std::endl;
		std::cout << "mcy2: " << mcy2 << std::endl;
		std::cout << "mcz2: " << mcz2 << std::endl;
		std::cout << "mcr2: " << mcr2 << std::endl;
		std::cout << "mce2: " << mce2 << std::endl;
	      }

	     try{
	       const RAT::DS::FitVertex& rVertex1 = rev1.GetFitResult("scintFitter").GetVertex(0);
	       if(!(rVertex1.ValidPosition() && rVertex1.ValidTime()))
                  continue; // fit invalid

	       fitx1 = rVertex1.GetPosition().X();
	       fity1 = rVertex1.GetPosition().Y();
	       fitz1 = rVertex1.GetPosition().Z();
	       fitr1 = rVertex1.GetPosition().Mag();
	       
	       const RAT::DS::FitVertex& rVertex2 = rev2.GetFitResult("scintFitter").GetVertex(0);
	       if(!(rVertex2.ValidPosition() && rVertex2.ValidTime()))
		 continue; // fit invalid

	       fitx2 = rVertex2.GetPosition().X();
	       fity2 = rVertex2.GetPosition().Y();
	       fitz2 = rVertex2.GetPosition().Z();
	       fitr2 = rVertex2.GetPosition().Mag();

	     }
	     catch(const RAT::DS::FitCollection::NoResultError&){
	       continue;
	     }
	    // Fill tree
	    tree->Fill();
	  }
	}

    }
      
  // Now make 1D plots: fitx1-fitx2, fity1-fity2, fitz1-fitz2, fitr1-fitr2, distance pos1 to pos2
  TCanvas* cX = new TCanvas("cX", "cX", 1500,800);
  cX->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hX = new TH1F("hX","hX", 1000, -6000, 6000);
  hX->SetTitle("");
  tree->Draw("fitx1-fitx2 >> hX", "", "");
  hX->SetLineWidth(2);
  hX->SetLineColor(kBlue+2);
  hX->GetXaxis()->SetTitle("Original Fit X - Updated Fit X, mm");
  hX->GetYaxis()->SetTitle("Events");
  hX->Draw();
  cX->SaveAs("May30X2.pdf");
  
  TCanvas* cY = new TCanvas("cY", "cY", 1500,800);
  cY->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hY = new TH1F("hY","hY", 1000, -6000, 6000);
  hY->SetTitle("");
  tree->Draw("fity1-fity2 >> hY", "", "");
  hY->SetLineWidth(2);
  hY->SetLineColor(kBlue+2);
  hY->GetXaxis()->SetTitle("Original Fit Y - Updated Fit Y, mm");
  hY->GetYaxis()->SetTitle("Events");
  hY->Draw();
  cY->SaveAs("May30Y2.pdf");
  
  TCanvas* cZ = new TCanvas("cZ", "cZ", 1500,800);
  cZ->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hZ = new TH1F("hZ","hZ", 1000, -6000, 6000);
  hZ->SetTitle("");
  tree->Draw("fitz1-fitz2 >> hZ", "", "");
  hZ->SetLineWidth(2);
  hZ->SetLineColor(kBlue+2);
  hZ->GetXaxis()->SetTitle("Original Fit Z - Updated Fit Z, mm");
  hZ->GetYaxis()->SetTitle("Events");
  hZ->Draw();
  cZ->SaveAs("May30Z2.pdf");
  
  TCanvas* cR = new TCanvas("cR", "cR", 1500,800);
  cR->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hR = new TH1F("hR","hR", 1000, -6000, 6000);
  hR->SetTitle("");
  tree->Draw("fitr1-fitr2 >> hR", "", "");
  hR->SetLineWidth(2);
  hR->SetLineColor(kBlue+2);
  hR->GetXaxis()->SetTitle("Original Fit R - Updated Fit R, mm");
  hR->GetYaxis()->SetTitle("Events");
  hR->Draw();
  cR->SaveAs("May30R2.pdf");
  
  TCanvas* cD = new TCanvas("cD", "cD", 1500,800);
  cD->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  TH1F *hD = new TH1F("hD","hD", 1000, 0, 12000);
  hD->SetTitle("");
  tree->Draw("sqrt( (fitx1-fitx2)*(fitx1-fitx2) + (fity1-fity2)*(fity1-fity2) + (fitz1-fitz2)*(fitz1-fitz2) ) >> hD", "", "");
  hD->SetLineWidth(2);
  hD->SetLineColor(kBlue+2);
  hD->GetXaxis()->SetTitle("Original - Updated Fit Distance, mm");
  hD->GetYaxis()->SetTitle("Events");
  hD->Draw();
  cD->SaveAs("May30D2.pdf");
  
  
  // 2D plots distance:r distance:e distance:truedistance1
  
  TCanvas* cDR = new TCanvas("cDR", "cDR", 1500,800);
  cDR->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *hDR = new TH2F("hDR","hDR", 1000, 0, 6000, 1000, 0, 12000);
  hDR->SetTitle("");
  tree->Draw("sqrt( (fitx1-fitx2)*(fitx1-fitx2) + (fity1-fity2)*(fity1-fity2) + (fitz1-fitz2)*(fitz1-fitz1)):mcr >> hDR", "", "");
  hDR->SetLineWidth(2);
  hDR->GetYaxis()->SetTitle("Original - Updated Fit Distance, mm");
  hDR->GetXaxis()->SetTitle("True Radius, mm");
  hDR->Draw("colz");
  cDR->SaveAs("May30DR2.pdf");
  
  TCanvas* cDE = new TCanvas("cDE", "cDE", 1500,800);
  cDE->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *hDE = new TH2F("hDE","hDE", 1000, 1, 10, 1000, 0, 12000);
  hDE->SetTitle("");
  tree->Draw("sqrt( (fitx1-fitx2)*(fitx1-fitx2) + (fity1-fity2)*(fity1-fity2) + (fitz1-fitz2)*(fitz1-fitz1)):mce >> hDE", "", "");
  hDE->SetLineWidth(2);
  hDE->GetYaxis()->SetTitle("Original - Updated Fit Distance, mm");
  hDE->GetXaxis()->SetTitle("True Energy, MeV");
  hDE->Draw("colz");
  cDE->SaveAs("May30DE2.pdf");
  
  TCanvas* cDD = new TCanvas("cDD", "cDD", 1500,800);
  cDD->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  gPad->SetLogx();
  TH2F *hDD = new TH2F("hDD","hDD", 1000, 0, 12000, 1000, 0, 12000);
  hDD->SetTitle("");
  tree->Draw("sqrt( (fitx1-fitx2)*(fitx1-fitx2) + (fity1-fity2)*(fity1-fity2) + (fitz1-fitz2)*(fitz1-fitz1)): sqrt( (mcx-fitx1)*(mcx-fitx1) + (mcy-fity1)*(mcy-fity1) + (mcz-fitz1)*(mcz-fitz1) ) >> hDD", "", "");
  hDD->SetLineWidth(2);
  hDD->GetXaxis()->SetTitle("True - Original Fit Distance, mm");
  hDD->GetYaxis()->SetTitle("Original - Updated Fit Distance, mm");
  hDD->Draw("colz");
  cDD->SaveAs("May30DD2.pdf");
  
  TCanvas* cXX = new TCanvas("cXX", "cXX", 1500,800);
  cXX->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *hXX = new TH2F("hXX","hXX", 1000, -6000, 6000, 1000, -6000, 6000);
  hXX->SetTitle("");
  tree->Draw("fitx2:fitx1 >> hXX", "", "");
  hXX->SetLineWidth(2);
  hXX->GetXaxis()->SetTitle("Original Fit X, mm");
  hXX->GetYaxis()->SetTitle("Updated Fit X, mm");
  hXX->Draw("colz");
  cXX->SaveAs("May30XX2.pdf");
  
  TCanvas* cYY = new TCanvas("cYY", "cYY", 1500,800);
  cXX->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *hYY = new TH2F("hYY","hYY", 1000, -6000, 6000, 1000, -6000, 6000);
  hYY->SetTitle("");
  tree->Draw("fity2:fity1 >> hYY", "", "");
  hYY->SetLineWidth(2);
  hYY->GetXaxis()->SetTitle("Original Fit Y, mm");
  hYY->GetYaxis()->SetTitle("Updated Fit Y, mm");
  hYY->Draw("colz");
  cYY->SaveAs("May30YY2.pdf");
  
  TCanvas* cZZ = new TCanvas("cZZ", "cZZ", 1500, 800);
  cZZ->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *hZZ = new TH2F("hZZ","hZZ", 1000, -6000, 6000, 1000, -6000, 6000);
  hZZ->SetTitle("");
  tree->Draw("fitz2:fitz1 >> hZZ", "", "");
  hZZ->SetLineWidth(2);
  hZZ->GetXaxis()->SetTitle("Original Fit Z, mm");
  hZZ->GetYaxis()->SetTitle("Updated Fit Z, mm");
  hZZ->Draw("colz");
  cZZ->SaveAs("May30ZZ2.pdf");
  
  TCanvas* cRR = new TCanvas("cRR", "cRR", 1500, 800);
  cRR->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  TH2F *hRR = new TH2F("hRR","hRR", 1000, 0, 6000, 1000, 0, 6000);
  hRR->SetTitle("");
  tree->Draw("fitr2:fitr1 >> hRR", "", "");
  hRR->SetLineWidth(2);
  hRR->GetXaxis()->SetTitle("Original Fit R, mm");
  hRR->GetYaxis()->SetTitle("Updated Fit R, mm");
  hRR->Draw("colz");
  cRR->SaveAs("May30RR2.pdf");
  
}
