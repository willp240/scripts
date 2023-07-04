void printGTID( const std::string& fileName )
{

  //  TCanvas *c = new TCanvas("canv", "canv", 900, 600);

  //gStyle->SetOptStat(0);
  // gStyle->SetLineWidth(2);
  //gPad->SetFrameLineWidth(2);
  //gPad->SetGrid(1);


  // If this is being done on data that does not require remote database connection
  // eg.: a simple simulation with default run number (0)
  // We can disable the remote connections:
  //
  // NOTE: Don't do this if you are using real data!!!
  //  RAT::DB::Get()->SetAirplaneModeStatus(true);

  RAT::DU::DSReader dsReader( fileName );

  std::cout << dsReader.GetEntryCount() << std::endl;

  // Loop over events in file
  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );

      // Ignoring retriggers here
      if( rDS.GetEVCount() > 0){
	const RAT::DS::EV& rEV = rDS.GetEV( 0 );

	int gtid = rEV.GetGTID();
	std::cout << gtid << std::endl;
      }
    }

}
