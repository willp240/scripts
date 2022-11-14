////////////////////////////////////////////////////////////////////
/// \file PlotHitTimeResiduals.cc
///
/// \brief Functions to plot hit time residuals.
///
/// \author P G Jones <p.g.jones@qmul.ac.uk>
///
/// REVISION HISTORY:\n
///     2014-03-27 : P G Jones - First Revision.\n
///
/// \details EV Calibrated hit times are plotted minus transit times
/// based on the MC position or the fitted position.
///
////////////////////////////////////////////////////////////////////

#include <RAT/DU/DSReader.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/DU/LightPathCalculator.hh>
#include <RAT/DU/GroupVelocity.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/MC.hh>
#include <RAT/DS/EV.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/FitResult.hh>

#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TFile.h>

#include <string>

double CalcLLH(TH1D* hist);
double CalcProb(TH1D* hist);


void* PositionScan( const std::string& fileName )
{
  
  int col[6] = {1,4,9,40,46,2};
  TGraph* hScanX = new TGraph();

  std::vector<TH1D*> histvec;

  RAT::DB::Get()->SetAirplaneModeStatus(true);

  RAT::DU::DSReader dsReader( fileName );

  // RAT::DU::Utility::Get()->GetLightPathCalclator() must be called *after* the RAT::DU::DSReader constructor.
  RAT::DU::LightPathCalculator lightPath = RAT::DU::Utility::Get()->GetLightPathCalculator(); // To calculate the light's path
  const RAT::DU::GroupVelocity& groupVelocity = RAT::DU::Utility::Get()->GetGroupVelocity(); // To get the group velocity
  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
      TVector3 eventPosition = rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed

      for(int iPos = 0; iPos < 1200; iPos++){
      
	eventPosition.SetXYZ(-6000+iPos*10,0,0);
	std::cout << eventPosition.X() << " " << eventPosition.Y() << " " << eventPosition.Z() << std::endl;

	TH1D* hHitTimeResiduals = new TH1D( "hHitTimeResidualsMC", "Hit time residuals", 400, -100.0, 300.0 );

	for( size_t iEV = 0; iEV < rDS.GetEVCount(); iEV++ )
	  {
	    const RAT::DS::EV& rEV = rDS.GetEV( iEV );
	    const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
	    for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
	      {
		const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );
		
		lightPath.CalcByPosition( eventPosition, pmtInfo.GetPosition( pmtCal.GetID() ) );
		double distInInnerAV = lightPath.GetDistInInnerAV();
		double distInAV = lightPath.GetDistInAV();
		double distInWater = lightPath.GetDistInWater();
		
		const double transitTime = groupVelocity.CalcByDistance( distInInnerAV, distInAV, distInWater ); // Assumes a 400nm photon
		// Time residuals estimate the photon emission time relative to the event start so subtract off the transit time
		// hit times are relative to the trigger time, which will depend on event time and detector position so correct for that to line up events
		// The 390ns corrects for the electronics delays and places the pulse in the middle of the window
		hHitTimeResiduals->Fill( pmtCal.GetTime() - transitTime - 390 + rDS.GetMCEV(iEV).GetGTTime());
	      }
	  }
	//hHitTimeResiduals->Scale(1/hHitTimeResiduals->Integral());
	//hHitTimeResiduals->SetLineColor(col[iPos]);
	histvec.push_back(hHitTimeResiduals);

	double LLH = CalcLLH(histvec.at(iPos));
	std::cout << -6000+iPos*10 << " " << LLH << std::endl;
	hScanX->SetPoint(iPos, -6000+iPos*10, LLH);
      }

      /*
      for(int i=0; i < histvec.size(); i++){
	if(i==0)
	  histvec.at(i)->Draw();
	else
	  histvec.at(i)->Draw("same");
	  }*/
      hScanX->Draw("ap");
    }
}

double CalcProb(TH1D* hist){

  TFile f1 = TFile("outscan1.root","READ");
  TH1D* pdf = (TH1D*)f1.Get("fpdfhist");

  //  hist->Scale(1/hist->Integral());

  double llh=0;
  for(int i=0; i<pdf->GetXaxis()->GetNbins(); i++){
    if(hist->GetBinContent(i) == 0)
      continue;
    double prob = hist->GetBinContent(i)*1000*pdf->GetBinContent(i);
    llh+=log(prob);
  }

  return llh;
}


double CalcLLH(TH1D* hist){

  TFile f1 = TFile("outscan1.root","READ");
  TH1D* pdf = (TH1D*)f1.Get("fpdfhist");

  pdf->Rebin(4);
  hist->Rebin(4);

  //  hist->Scale(1/hist->Integral());

  double llh=0;
  for(int i=0; i<pdf->GetXaxis()->GetNbins(); i++){

    double mc = pdf->GetBinContent(i);
    double dat = hist->GetBinContent(i); 

    if(dat == 0)
      continue;
    double prob = mc - dat*log(mc) ;
    llh+=prob;
  }

  return llh;
}



/// Plot the hit time residuals for the MC position
///
/// @param[in] fileName of the RAT::DS root file to analyse
/// @return the histogram plot
TH1D* PlotHitTimeResidualsMCPosition( const std::string& fileName )
{
  TH1D* hHitTimeResiduals = new TH1D( "hHitTimeResidualsMC", "Hit time residuals using the MC position", 1000, -500.0, 500.0 );
  // If this is being done on data that does not require remote database connection
  // eg.: a simple simulation with default run number (0)
  // We can disable the remote connections:
  //
  // NOTE: Don't do this if you are using real data!!!
  RAT::DB::Get()->SetAirplaneModeStatus(true);

  RAT::DU::DSReader dsReader( fileName );

  // RAT::DU::Utility::Get()->GetLightPathCalculator() must be called *after* the RAT::DU::DSReader constructor.
  RAT::DU::LightPathCalculator lightPath = RAT::DU::Utility::Get()->GetLightPathCalculator(); // To calculate the light's path
  const RAT::DU::GroupVelocity& groupVelocity = RAT::DU::Utility::Get()->GetGroupVelocity(); // To get the group velocity
  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
      TVector3 eventPosition = rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed
      eventPosition.SetXYZ(3000,3000,3000);
      std::cout << eventPosition.X() << " " << eventPosition.Y() << " " << eventPosition.Z() << std::endl;
      for( size_t iEV = 0; iEV < rDS.GetEVCount(); iEV++ )
        {
          const RAT::DS::EV& rEV = rDS.GetEV( iEV );
          const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
          for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
            {
              const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );

              lightPath.CalcByPosition( eventPosition, pmtInfo.GetPosition( pmtCal.GetID() ) );
              double distInInnerAV = lightPath.GetDistInInnerAV();
              double distInAV = lightPath.GetDistInAV();
              double distInWater = lightPath.GetDistInWater();

              const double transitTime = groupVelocity.CalcByDistance( distInInnerAV, distInAV, distInWater ); // Assumes a 400nm photon
              // Time residuals estimate the photon emission time relative to the event start so subtract off the transit time
              // hit times are relative to the trigger time, which will depend on event time and detector position so correct for that to line up events
              // The 390ns corrects for the electronics delays and places the pulse in the middle of the window
              hHitTimeResiduals->Fill( pmtCal.GetTime() - transitTime - 390 + rDS.GetMCEV(iEV).GetGTTime());
            }
        }
    }
  hHitTimeResiduals->GetYaxis()->SetTitle( "Count per 1 ns bin" );
  hHitTimeResiduals->GetXaxis()->SetTitle( "Hit time residuals [ns]" );
  hHitTimeResiduals->Draw();
  return hHitTimeResiduals;
}

/// Plot the hit time residuals for the fit position
///
/// @param[in] fileName of the RAT::DS root file to analyse
/// @return the histogram plot
TH1D* PlotHitTimeResidualsFitPosition( const std::string& fileName, std::string fitName = "")
{
  TH1D* hHitTimeResiduals = new TH1D( "hHitTimeResidualsFit", "Hit time residuals using the Fit position", 1000, -500.0, 500.0 );
  // If this is being done on data that does not require remote database connection
  // eg.: a simple simulation with default run number (0)
  // We can disable the remote connections:
  //
  // NOTE: Don't do this if you are using real data!!!
  RAT::DB::Get()->SetAirplaneModeStatus(true);

  RAT::DU::DSReader dsReader( fileName );

  // RAT::DU::Utility::Get()->GetLightPathCalculator() must be called *after* the RAT::DU::DSReader constructor.
  RAT::DU::LightPathCalculator lightPath = RAT::DU::Utility::Get()->GetLightPathCalculator(); // To calculate the light's path
  const RAT::DU::GroupVelocity& groupVelocity = RAT::DU::Utility::Get()->GetGroupVelocity(); // To get the group velocity
  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
      for( size_t iEV = 0; iEV < rDS.GetEVCount(); iEV++ )
        {
          const RAT::DS::EV& rEV = rDS.GetEV( iEV );

          // grab the fit information
          if(fitName == "")
              fitName = rEV.GetDefaultFitName();

          TVector3 eventPosition;
          double   eventTime;

          try{
              const RAT::DS::FitVertex& rVertex = rEV.GetFitResult(fitName).GetVertex(0);
              if(!(rVertex.ValidPosition() && rVertex.ValidTime()))
                  continue; // fit invalid

              eventPosition = rVertex.GetPosition();
              eventTime = rVertex.GetTime();
          }
          catch(const RAT::DS::FitCollection::NoResultError&){
              // no fit result by the name of fitName
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
          // DataNotFound --> implies no fit results are present, don't catch.

          // calculate time residuals
          const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
          for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
            {
              const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );

              lightPath.CalcByPosition( eventPosition, pmtInfo.GetPosition( pmtCal.GetID() ) );
              double distInInnerAV = lightPath.GetDistInInnerAV();
              double distInAV = lightPath.GetDistInAV();
              double distInWater = lightPath.GetDistInWater();
              const double transitTime = groupVelocity.CalcByDistance( distInInnerAV, distInAV, distInWater ); // Assumes a 400nm photon
              // Time residuals estimate the photon emission time relative to the event start so subtract off the transit time and eventTime
              hHitTimeResiduals->Fill( pmtCal.GetTime() - transitTime - eventTime);
            }
        }
    }
  hHitTimeResiduals->GetYaxis()->SetTitle( "Count per 1 ns bin" );
  hHitTimeResiduals->GetXaxis()->SetTitle( "Hit time residuals [ns]" );
  hHitTimeResiduals->Draw();
  return hHitTimeResiduals;
}

/// Plot both the MC and Fitted position residuals
///
/// @param[in] fileName of the RAT::DS root file to analyse
void PlotHitTimeResiduals( const std::string& fileName )
{
  gStyle->SetFillColor( kWhite );
  TCanvas* c1 = new TCanvas();
  TH1D* mc = PlotHitTimeResidualsMCPosition( fileName );
  TH1D* fit = PlotHitTimeResidualsFitPosition( fileName );
  mc->Draw();
  fit->SetLineColor( kGreen + 2 );
  fit->Draw("SAME");
  TLegend* t1 = new TLegend( 0.7, 0.7, 0.9, 0.9 );
  t1->AddEntry( mc, "MC Position", "l" );
  t1->AddEntry( fit, "Fit Position", "l" );
  t1->Draw();
  c1->Update();
}
