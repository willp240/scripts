void plotDeltaR( const std::string& fileName, std::string fitName = "" )
{

  TCanvas *c = new TCanvas("canv", "canv", 900, 600);

  gStyle->SetOptStat(0);
  gStyle->SetLineWidth(2);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  TH1D* hDeltaR = new TH1D( "hDeltaR", "Delta R, mm", 10, -0.5, 9.5 );

  // If this is being done on data that does not require remote database connection
  // eg.: a simple simulation with default run number (0)
  // We can disable the remote connections:
  //
  // NOTE: Don't do this if you are using real data!!!
  RAT::DB::Get()->SetAirplaneModeStatus(true);

  RAT::DU::DSReader dsReader( fileName );

  // Loop over events in file
  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount()-1; iEntry++ )
    {
      std::cout << iEntry << std::endl;
      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
      std::cout << "got entry" << std::endl;

      // Ignoring retriggers here
      const RAT::DS::EV& rEV = rDS.GetEV( 0 );
      std::cout << "got event" << std::endl;
      if(fitName == "")
        fitName = rEV.GetDefaultFitName();
      const RAT::DS::FitVertex& rVertex = rEV.GetFitResult(fitName).GetVertex(0);
      TVector3 eventPosition = rVertex.GetPosition();


      const RAT::DS::Entry& rDS2 = dsReader.GetEntry( iEntry+1 );
      const RAT::DS::EV& rEV2 = rDS2.GetEV( 0 );
      const RAT::DS::FitVertex& rVertex2 = rEV2.GetFitResult(fitName).GetVertex(0);
      TVector3 eventPosition2 = rVertex2.GetPosition();

      hDeltaR->SetBinContent( iEntry+1, (eventPosition-eventPosition2).Mag() );
      std::cout << "done eve" << std::endl;
    }
  
  hDeltaR->GetXaxis()->SetTitle( "Event Number" );
  hDeltaR->GetYaxis()->SetTitle( "Delta R, mm" );
  hDeltaR->SetMarkerStyle(50);
  hDeltaR->GetYaxis()->SetTitleOffset(1.3);
  hDeltaR->Draw("P");
  hDeltaR->SetLineWidth(2);
  hDeltaR->SetMarkerColor(kBlue+2);
  hDeltaR->SetMarkerSize(4);
}
