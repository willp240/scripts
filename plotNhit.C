void plotNhit( const std::string& fileName )
{

  TCanvas *c = new TCanvas("canv", "canv", 900, 600);

  gStyle->SetOptStat(0);
  gStyle->SetLineWidth(2);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  TH1D* hNhit = new TH1D( "hNhit", "Nhit", 10, -0.5, 9.5 );

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

      int nhits = rEV.GetNhits();
      hNhit->SetBinContent( iEntry+1, nhits );
      std::cout << "done eve" << std::endl;
    }
  
  hNhit->GetXaxis()->SetTitle( "Event Number" );
  hNhit->GetYaxis()->SetTitle( "Nhits" );
  hNhit->SetMarkerStyle(50);
  hNhit->GetYaxis()->SetTitleOffset(1.3);
  hNhit->Draw("P");
  hNhit->SetLineWidth(2);
  hNhit->SetMarkerColor(kBlue+2);
  hNhit->SetMarkerSize(4);
  hNhit->GetYaxis()->SetRangeUser(0,3500);
}
