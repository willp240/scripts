void count_steps( const std::string& fileName )
{

  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();

  TString hname = "hname";
  TString htitle = "Number of Steps per Track: 2.5 MeV e-";
  TH1D* h = new TH1D(hname, htitle, 20, 0, 50);
  h->GetXaxis()->SetTitle("Number of Steps");
  h->GetYaxis()->SetTitle("Tracks");
  h->GetYaxis()->SetTitleOffset(1.1);
  h->SetLineWidth(2);
  h->SetLineColor(kBlue+2);

  gStyle->SetOptStat(111111); // to show overflow / underflow stats
  // The process summary flag:
  RAT::DS::MCTrack::ESummaryFlag flag = RAT::DS::MCTrack::OpReemission;

  // If this is being done on data that does not require remote database connection
  // eg.: a simple simulation with default run number (0)
  // We can disable the remote connections:
  //
  // NOTE: Don't do this if you are using real data!!!
  RAT::DB::Get()->SetAirplaneModeStatus(true);

  RAT::DU::DSReader dsReader( fileName );

  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
    {

      double count =0;
      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
      const RAT::DS::MC& mc = rDS.GetMC();
      std::vector<unsigned int> mcTrackIDs = mc.GetMCTrackIDs();
      for( size_t iTrack = 0; iTrack < mcTrackIDs.size(); iTrack++ )
        {
          const RAT::DS::MCTrack& mcTrack = mc.GetMCTrack( mcTrackIDs.at(iTrack));
	  count += mcTrack.GetMCTrackStepCount();
	  h->Fill(mcTrack.GetMCTrackStepCount());
        }
      count = count / mcTrackIDs.size();
      std::cout << "Average step count = " << count << "for " << mcTrackIDs.size() << " tracks" << std::endl;
    }

  h->Draw();
  c1->SaveAs("tracksteps.pdf");
}
