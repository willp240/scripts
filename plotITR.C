void plotITR( const std::string& fileName )
{

  TCanvas *c = new TCanvas("canv", "canv", 900, 600);

  gStyle->SetOptStat(0);
  gStyle->SetLineWidth(2);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  TH1D* hITR = new TH1D( "hITR", "ITR", 10, -0.5, 9.5 );

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

      // Ignoring retriggers here
      const RAT::DS::EV& rEV = rDS.GetEV( 0 );

      double itr = rEV.GetClassifierResult( "ITR:scintFitter" ).GetClassification( "ITR" );
      hITR->SetBinContent( iEntry+1, itr );
    }
  
  hITR->GetXaxis()->SetTitle( "Event Number" );
  hITR->GetYaxis()->SetTitle( "ITR" );
  hITR->SetMarkerStyle(50);
  hITR->GetYaxis()->SetTitleOffset(1.3);
  hITR->SetLineWidth(2);
  hITR->SetMarkerColor(kBlue+2);
  hITR->SetMarkerSize(4);
  hITR->GetYaxis()->SetRangeUser(0,0.5);
  hITR->Draw("P");
}
