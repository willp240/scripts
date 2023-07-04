void CompareTimeResiduals(  )
{ 

  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);


  std::vector<std::string> fnames;
  fnames.push_back("/data/snoplus3/parkerw/ratSimulations/Apr15_SEV_NoInterp2.5MeV/*.root");
  fnames.push_back("/data/snoplus3/parkerw/ratSimulations/Apr15_SEV_NoInterp10MeV/*.root");
  
  std::vector<std::string> flabels;
  flabels.push_back("2.5 MeV True Position");
  flabels.push_back("10 MeV True Position");
 
  std::vector<double> fvels;
  fvels.push_back(185.96);
  fvels.push_back(185.96);
  
  std::vector<double> fbiasfac;
  fbiasfac.push_back(0);
  fbiasfac.push_back(0);
  

  TLegend* leg = new TLegend(0.55, 0.6, 0.88, 0.88);

  for (int i = 0; i<fnames.size(); i++){

    TString hname = Form("fittim_%d",i);

    TH1D* hHitTimeResiduals = new TH1D( hname, "Fit Time", 200, 200, 300 );
    RAT::DB::Get()->SetAirplaneModeStatus(true);

    RAT::DU::DSReader dsReader( fnames.at(i) );

    RAT::LP::LightPathStraightScint::BeginOfRun();

    const RAT::DU::EffectiveVelocity& effVelocity = RAT::DU::Utility::Get()->GetEffectiveVelocity(); // To get the group velocity
    const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
    for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
      {
	const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
	TVector3 eventPosition = rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed

	TVector3 unitPos = eventPosition.Unit();
	TVector3 biasVec = fbiasfac.at(i)*unitPos;
	  
	eventPosition = eventPosition + biasVec;

	for( size_t iEV = 0; iEV < rDS.GetEVCount(); iEV++ )
	  {
	    const RAT::DS::EV& rEV = rDS.GetEV( iEV );
	    const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
	    for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
	      {
		const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );
		const TVector3 pmtPos = RAT::DU::Utility::Get()->GetPMTInfo().GetPosition( pmtCal.GetID() );

		double distInAV = 0.0;
		double distInWater = 0.0;
		double distInTarget = 0.0;
		RAT::LP::LightPathStraightScint::GetPath(pmtPos, eventPosition, distInTarget, distInWater);		
		
		double fInnerAVVelocity = fvels.at(i);
		double fAVVelocity = 1.93109181500140664e+02;
		double fWaterVelocity = 2.17554021555098529e+02;
		double fOffset = 0.6;

		//const double transitTime = effVelocity.CalcByDistance( distInTarget, distInAV, distInWater ); // Assumes a 400nm photon
		const double transitTime = distInTarget / fInnerAVVelocity + distInAV / fAVVelocity + distInWater / fWaterVelocity + fOffset;
		hHitTimeResiduals->Fill( pmtCal.GetTime() - transitTime - 390 + rDS.GetMCEV(iEV).GetGTTime());
		//std::cout << i << " " << fvels.at(i) << " " << iEntry << " " << transitTime << " " << pmtCal.GetTime() - transitTime - 390 + rDS.GetMCEV(iEV).GetGTTime() << " " << distInTarget / fInnerAVVelocity << " " << distInAV / fAVVelocity << " " << distInWater / fWaterVelocity << std::endl; 
	      }
	  }
      }

    hHitTimeResiduals->Scale(1/hHitTimeResiduals->Integral());
    hHitTimeResiduals->GetYaxis()->SetTitle( "Normalised Counts" );
    hHitTimeResiduals->GetXaxis()->SetTitle( "Hit time residuals [ns]" );
    hHitTimeResiduals->SetLineColor(i+1);
    hHitTimeResiduals->SetLineWidth(2);

    hHitTimeResiduals->GetXaxis()->SetRangeUser(-10,30);
    hHitTimeResiduals->GetYaxis()->SetRangeUser(0,0.05);
    hHitTimeResiduals->GetYaxis()->SetTitleOffset(1.3);
    
    if(i == 0)
      {
	hHitTimeResiduals->SetLineColor(i-2);
    	hHitTimeResiduals->SetLineStyle(2);
      }

    if(i==0)
      hHitTimeResiduals->Draw();
    else
      hHitTimeResiduals->Draw("same");

    leg->AddEntry(hHitTimeResiduals, flabels.at(i).c_str(), "l");

  }

  gPad->SetGrid(1);
  leg->SetLineWidth(2);
  leg->Draw("same");

}
