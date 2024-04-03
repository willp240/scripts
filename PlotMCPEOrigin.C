
void PlotMCPEOrigin( const std::string& fileName )
{

  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TH1D* hOriginTime = new TH1D( "hOriginTime", "MCPE Photon Origin Time", 400, -100, 300 );
  TH1D* hCorrectedOriginTime = new TH1D( "CorrectedhOriginTime", "MCPE Photon Origin Time (relative to Global Trigger Time)", 400, 200, 600 );
  TH1D* hOriginPosX = new TH1D( "hOriginPosX", "MCPE Photon Origin Position X (mm)", 1700, -8500, 8500 );
  TH1D* hOriginPosY = new TH1D( "hOriginPosY", "MCPE Photon Origin Position Y (mm)", 1700, -8500,8500 );
  TH1D* hOriginPosZ = new TH1D( "hOriginPosZ", "MCPE Photon Origin Position Z (mm)", 1700, -8500,8500 );

  TH2D* hOriginPosZT = new TH2D( "hOriginPosZT", "MCPE Photon Origin Position Z (mm) vs Origin Time (ns)", 400, -100, 300, 1700, -8500, 8500);
 
  RAT::DB::Get()->SetAirplaneModeStatus(true);

  RAT::DU::DSReader dsReader( fileName );
    
  //  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
      const RAT::DS::MC& rMC = rDS.GetMC( );
      const RAT::DS::MCEV& rMCEV = rDS.GetMCEV(0 );
      std::cout << rMCEV.GetGTTime() << std::endl;
      for( size_t iPMT = 0; iPMT < rMC.GetMCPMTCount(); iPMT++ )
	{
	  const RAT::DS::MCPMT& rMCPMT = rMC.GetMCPMT( iPMT );
	  //	  std::cout << rMCPMT.GetMCPECount() << std::endl;
	  for( size_t iPE = 0; iPE < rMCPMT.GetMCPECount(); iPE++ ){
	    hOriginTime->Fill( rMCPMT.GetMCPE( iPE ).GetOriginTime() );
	    hCorrectedOriginTime->Fill( rMCPMT.GetMCPE( iPE ).GetOriginTime() + 390 - rMCEV.GetGTTime());
	    hOriginPosX->Fill( rMCPMT.GetMCPE( iPE ).GetOriginPosition().X() );
	    hOriginPosY->Fill( rMCPMT.GetMCPE( iPE ).GetOriginPosition().Y() );
	    hOriginPosZ->Fill( rMCPMT.GetMCPE( iPE ).GetOriginPosition().Z() );
	    hOriginPosZT->Fill( rMCPMT.GetMCPE( iPE ).GetOriginTime(), rMCPMT.GetMCPE( iPE ).GetOriginPosition().Z() );
	  }
	}
    }

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->Divide(2,2);

  c1->cd(1);
  hOriginTime->GetYaxis()->SetTitle( "Count per 1 s bin" );
  hOriginTime->GetXaxis()->SetTitle( "Emission Time, ns" );
  hOriginTime->Draw();

  c1->cd(2);
  hOriginPosX->GetYaxis()->SetTitle( "Count per 10 mm bin" );
  hOriginPosX->GetXaxis()->SetTitle( "Emission Position X, mm" );
  hOriginPosX->Draw();

  c1->cd(3);
  hOriginPosY->GetYaxis()->SetTitle( "Count per 10 mm bin" );
  hOriginPosY->GetXaxis()->SetTitle( "Emission Position Y, mm" );
  hOriginPosY->Draw();

  c1->cd(4);
  hOriginPosZ->GetYaxis()->SetTitle( "Count per 10 mm bin" );
  hOriginPosZ->GetXaxis()->SetTitle( "Emission Position Z, mm" );
  hOriginPosZ->Draw();

  c1->Draw();

  /*  TCanvas* c2 = new TCanvas("c2", "c2");
  gStyle->SetFrameLineWidth(2);
  c2->cd();
  hCorrectedOriginTime->GetYaxis()->SetTitle( "Count per 1 s bin" );
  hCorrectedOriginTime->GetXaxis()->SetTitle( "Emission Time, ns" );
  hCorrectedOriginTime->GetYaxis()->SetTitleOffset(1.38);
  hCorrectedOriginTime->SetLineWidth(2);
  hCorrectedOriginTime->Draw();*/

  //  hOriginPosZT->Draw("colz");

  TFile fout("MCPEOrigin.root","RECREATE");
  hOriginTime->Write();
  hOriginPosX->Write();
  hOriginPosY->Write();
  hOriginPosZ->Write();
  c1->Write();

  c1->SaveAs("MCPEOrigin.pdf");
  //  c2->SaveAs("CorrectedMCPEOrigin.pdf");
}

