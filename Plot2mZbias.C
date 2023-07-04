void Plot2mZbias(  )
{ 

  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);


  std::vector<std::string> fnames;
  fnames.push_back("/data/snoplus3/parkerw/ratSimulations/Jun6_10MeV_2.5MeVSEVand10MeVPDF/*.root");
  fnames.push_back("/data/snoplus3/parkerw/ratSimulations/May29_10MeV_002m_scaledSEV/*.root");

  std::vector<std::string> flabels;
  //flabels.push_back("185.96 ns/mm");
  //flabels.push_back("183.71 ns/mm");
  flabels.push_back("10 MeV PDF");
  flabels.push_back("Scaled SEV");

  std::vector<double> fvels;
  fvels.push_back(185.96);
  fvels.push_back(183.71);
 
  TLegend* leg = new TLegend(0.5, 0.4, 0.85, 0.8);

  for (int i = 0; i<fnames.size(); i++){

    int count = 0;

    TString hname = Form("hFittedZ_%d",i);

    TH1D* hFittedZ = new TH1D(hname, "hFittedZ", 200, 1000, 3000);
    //TH1D* hFittedZ = new TH1D(hname, "hFittedZ", 200, 100, 300);
    RAT::DB::Get()->SetAirplaneModeStatus(true);

    RAT::DU::DSReader dsReader( fnames.at(i) );

    RAT::LP::LightPathStraightScint::BeginOfRun();

    const RAT::DU::EffectiveVelocity& effVelocity = RAT::DU::Utility::Get()->GetEffectiveVelocity(); // To get the group velocity
    const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
    for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
      {
	const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
	TVector3 eventPosition = rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed

	for( size_t iEV = 0; iEV < rDS.GetEVCount(); iEV++ )
	  {

	    count++;
	    if(count%1000 ==0)
	      std::cout << count << std::endl;
	    if (count > 50000){
	      iEV = rDS.GetEVCount()-1;
	      iEntry = dsReader.GetEntryCount()-1;
	    }

	    const RAT::DS::EV& rEV = rDS.GetEV( iEV );
	    const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
	
	    try{
	      const RAT::DS::FitVertex& rVertex = rEV.GetFitResult("multiPDFFit").GetVertex(0);
	      if(!(rVertex.ValidPosition() && rVertex.ValidTime()))
		continue; // fit invalid
	      TVector3 fitpos = rVertex.GetPosition();
	      hFittedZ->Fill(rVertex.GetPosition().Mag());
	      //hFittedZ->Fill(rVertex.GetTime());
	    }
	    catch (const RAT::DS::FitCollection::NoResultError&) {
	      continue;
	    }
	    catch (const RAT::DS::FitResult::NoVertexError&){
              // no fit vertex
              continue;
	    }
	    catch(const RAT::DS::FitVertex::NoValueError&){
              // position or time missing
              continue;
	    }
	    /*
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
		}*/
	      }
	  }

    hFittedZ->GetYaxis()->SetTitle( "Events" );
    hFittedZ->GetXaxis()->SetTitle( "Fitted Radius, mm" );
    hFittedZ->SetLineColor(i+1);
    hFittedZ->SetLineWidth(2);
    
    if(i==0)
      hFittedZ->Draw();
    else
      hFittedZ->Draw("same");

    std::cout << hFittedZ->GetMean() << std::endl;

    leg->AddEntry(hFittedZ, flabels.at(i).c_str(), "l");

  }

  gPad->SetGrid(1);
  leg->Draw("same");

}
