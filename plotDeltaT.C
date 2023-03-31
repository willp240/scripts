void plotDeltaT( const std::string& fileName, std::string fitName = "" )
{

  TCanvas *c = new TCanvas("canv", "canv", 900, 600);

  gStyle->SetOptStat(0);
  gStyle->SetLineWidth(2);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  TH1D* hDeltaT = new TH1D( "hDeltaT", "Delta T, ns", 10, -0.5, 9.5 );

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
      Long64_t t1 = ( rEV.GetClockCount50()*20 & 0x7FFFFFFFFFF );

      const RAT::DS::Entry& rDS2 = dsReader.GetEntry( iEntry+1 );
      const RAT::DS::EV& rEV2 = rDS2.GetEV( 0 );

      Long64_t t2 = ( rEV2.GetClockCount50()*20 & 0x7FFFFFFFFFF );

      Long64_t dt = ( (t2-t1) ); 

      hDeltaT->SetBinContent( iEntry+1, dt );
      std::cout << "done eve " << t1 << " " << t2 << " " << dt << std::endl;
    }
  
  hDeltaT->GetXaxis()->SetTitle( "Event Number" );
  hDeltaT->GetYaxis()->SetTitle( "Delta T, ns" );
  hDeltaT->SetMarkerStyle(50);
  hDeltaT->GetYaxis()->SetTitleOffset(1.3);
  hDeltaT->Draw("P");
  hDeltaT->SetLineWidth(2);
  hDeltaT->SetMarkerColor(kBlue+2);
  hDeltaT->SetMarkerSize(4);
}
