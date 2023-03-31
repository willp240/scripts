void plotFittedR( const std::string& fileName, std::string fitName = "")
{

  TCanvas *c = new TCanvas("canv", "canv", 900, 600);

  gStyle->SetOptStat(0);
  gStyle->SetLineWidth(2);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  TH1D* hFittedRadius = new TH1D( "hFittedRadius", "Reconstructed Radius", 10, -0.5, 9.5 );

  // If this is being done on data that does not require remote database connection
  // eg.: a simple simulation with default run number (0)
  // We can disable the remote connections:
  //
  // NOTE: Don't do this if you are using real data!!!
  RAT::DB::Get()->SetAirplaneModeStatus(true);

  RAT::DU::DSReader dsReader( fileName );

  // Loop over events in file
  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
    {
      std::cout << iEntry << std::endl;
      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
      std::cout << "got entry" << std::endl;
      // Ignoring retriggers here
      const RAT::DS::EV& rEV = rDS.GetEV( 0 );
      std::cout << "got event" << std::endl;
      // Grab the fit information
      if(fitName == "")
	fitName = rEV.GetDefaultFitName();

      // Get fit result
      const RAT::DS::FitVertex& rVertex = rEV.GetFitResult(fitName).GetVertex(0);
      // if(!(rVertex.ValidPosition() && rVertex.ValidTime()))
      //continue; // Ignore if fit invalid

      TVector3 eventPosition = rVertex.GetPosition();
      hFittedRadius->SetBinContent( iEntry+1, eventPosition.Mag() );
      std::cout << "done eve" << std::endl;
    }
  
  hFittedRadius->GetXaxis()->SetTitle( "Event Number" );
  hFittedRadius->GetYaxis()->SetTitle( "Reconstructed Radius, mm" );
  hFittedRadius->SetMarkerStyle(50);
  hFittedRadius->GetYaxis()->SetTitleOffset(1.3);
  hFittedRadius->Draw("P");
  hFittedRadius->SetLineWidth(2);
  hFittedRadius->SetMarkerColor(kBlue+2);
  hFittedRadius->SetMarkerSize(4);
  hFittedRadius->GetYaxis()->SetRangeUser(0,6000);
}
