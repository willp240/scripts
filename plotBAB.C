void plotBAB( const std::string& fileName )
{

  TCanvas *c = new TCanvas("canv", "canv", 900, 600);

  gStyle->SetOptStat(0);
  gStyle->SetLineWidth(2);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  TH1D* hBAB = new TH1D( "hBAB", "BAB", 10, -0.5, 9.5 );

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

      double bab = rEV.GetClassifierResult( "BerkeleyAlphaBeta:scintFitter" ).GetClassification( "likelihood" );
      hBAB->SetBinContent( iEntry+1, bab );
    }
  
  hBAB->GetXaxis()->SetTitle( "Event Number" );
  hBAB->GetYaxis()->SetTitle( "BAB" );
  hBAB->SetMarkerStyle(50);
  hBAB->GetYaxis()->SetTitleOffset(1.3);
  hBAB->SetLineWidth(2);
  hBAB->SetMarkerColor(kBlue+2);
  hBAB->SetMarkerSize(4);
  hBAB->GetYaxis()->SetRangeUser(-50,0);
  hBAB->Draw("P");
}
