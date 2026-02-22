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
#include <RAT/DU/Point3D.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/MC.hh>
#include <RAT/DS/EV.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/FitResult.hh>

#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>

#include <string>

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
  size_t fPSUPSystemId = RAT::DU::Point3D::GetSystemId("innerPMT");

  // RAT::DU::Utility::Get()->GetLightPathCalculator() must be called *after* the RAT::DU::DSReader constructor.
  RAT::DU::LightPathCalculator lightPath = RAT::DU::Utility::Get()->GetLightPathCalculator(); // To calculate the light's path
  const RAT::DU::GroupVelocity& groupVelocity = RAT::DU::Utility::Get()->GetGroupVelocity(); // To get the group velocity
  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
      RAT::DU::Point3D eventPosition(fPSUPSystemId, rDS.GetMC().GetMCParticle(0).GetPosition()); // At least 1 is somewhat guaranteed
      for( size_t iEV = 0; iEV < rDS.GetEVCount(); iEV++ )
        {
          const RAT::DS::EV& rEV = rDS.GetEV( iEV );
          const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
          for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
            {
              const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );
              RAT::DU::Point3D pmtPos(fPSUPSystemId, pmtInfo.GetPosition( pmtCal.GetID() ) );

              lightPath.CalcByPosition( eventPosition, pmtPos );
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
  size_t fPSUPSystemId = RAT::DU::Point3D::GetSystemId("innerPMT");

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

          RAT::DU::Point3D eventPosition(fPSUPSystemId);
          double eventTime;

          try{
              const RAT::DS::FitVertex& rVertex = rEV.GetFitResult(fitName).GetVertex(0);
              if(!(rVertex.ValidPosition() && rVertex.ValidTime()))
                  continue; // fit invalid

              eventPosition.SetXYZ(fPSUPSystemId, rVertex.GetPosition());
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
              RAT::DU::Point3D pmtPos(fPSUPSystemId, pmtInfo.GetPosition( pmtCal.GetID() ) );

              lightPath.CalcByPosition( eventPosition, pmtPos );
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

/// Plot the hit time residuals for the MC emitted position for each photon
///
/// @param[in] fileName of the RAT::DS root file to analyse
/// @return the histogram plot
TH1D* PlotHitTimeResidualsEmitPosition( const std::string& fileName )
{
  TH1D* hHitTimeResiduals = new TH1D( "hHitTimeResidualsMC", "Hit time residuals using the photon emission position", 1000, -500.0, 500.0 );
  TH2D* hTresidEmitT = new TH2D("hHitTimeResidEmitT", "hHitTimeResidEmitT", 100, -100, 50, 100, 0, 100);
  // If this is being done on data that does not require remote database connection
  // eg.: a simple simulation with default run number (0)
  // We can disable the remote connections:
  //
  // NOTE: Don't do this if you are using real data!!!
  RAT::DB::Get()->SetAirplaneModeStatus(true);

  RAT::DU::DSReader dsReader( fileName );
  size_t fPSUPSystemId = RAT::DU::Point3D::GetSystemId("innerPMT");

  // RAT::DU::Utility::Get()->GetLightPathCalculator() must be called *after* the RAT::DU::DSReader constructor.
  RAT::DU::LightPathCalculator lightPath = RAT::DU::Utility::Get()->GetLightPathCalculator(); // To calculate the light's path
  const RAT::DU::GroupVelocity& groupVelocity = RAT::DU::Utility::Get()->GetGroupVelocity(); // To get the group velocity
  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
      const RAT::DS::MC& rMC = rDS.GetMC();

      for( size_t iEV = 0; iEV < rDS.GetEVCount(); iEV++ )
        {
          const RAT::DS::EV& rEV = rDS.GetEV( iEV );
          const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
          for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
            {
              const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );
              RAT::DU::Point3D pmtPos(fPSUPSystemId, pmtInfo.GetPosition( pmtCal.GetID() ) );
              const RAT::DS::MCPE& pe = rMC.GetMCPMT( iPMT ).GetMCPE(0);
              RAT::DU::Point3D emitPos(fPSUPSystemId, pe.GetOriginPos());
              double emitTime = pe.GetOriginTime();

              lightPath.CalcByPosition( emitPos, pmtPos );
              double distInInnerAV = lightPath.GetDistInInnerAV();
              double distInAV = lightPath.GetDistInAV();
              double distInWater = lightPath.GetDistInWater();
              const double transitTime = groupVelocity.CalcByDistance( distInInnerAV, distInAV, distInWater ); // Assumes a 400nm photon
              // Time residuals estimate the photon emission time relative to the event start so subtract off the transit time and eventTime
              hHitTimeResiduals->Fill( pmtCal.GetTime() - transitTime - emitTime - 390 + rDS.GetMCEV(iEV).GetGTTime());
	      hTresidEmitT->Fill( pmtCal.GetTime() - transitTime - emitTime - 390 + rDS.GetMCEV(iEV).GetGTTime(), transitTime );
            }
        }
    }

  hTresidEmitT->Draw("colz");
  hHitTimeResiduals->GetYaxis()->SetTitle( "Count per 1 ns bin" );
  hHitTimeResiduals->GetXaxis()->SetTitle( "Hit time residuals [ns]" );
  //  hHitTimeResiduals->Draw();
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

/// Plot the MC, Fitted, and Emission position residuals
///
/// @param[in] fileName of the RAT::DS root file to analyse
void PlotAllHitTimeResiduals( const std::string& fileName )
{

  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  TH1D* mc = PlotHitTimeResidualsMCPosition( fileName );
  TH1D* fit = PlotHitTimeResidualsFitPosition( fileName );
  TH1D* emit = PlotHitTimeResidualsEmitPosition( fileName );
  emit->SetLineColor( kBlue + 2 );
  emit->SetLineWidth(2);
  emit->SetTitle("");
  //  emit->GetXaxis()->SetRangeUser(-30,100);
  emit->Draw();
  mc->SetLineWidth(2);
  mc->Draw("SAME");
  fit->SetLineColor( kGreen + 2 );
  fit->SetLineWidth(2);
  fit->Draw("SAME");
  TLegend* t1 = new TLegend( 0.5, 0.5, 0.8, 0.8 );
  t1->AddEntry( mc, "MC Position", "l" );
  t1->AddEntry( fit, "Fit Position", "l" );
  t1->AddEntry( emit, "Emission Position", "l" );
  t1->SetLineWidth(2);
  t1->Draw();
  c1->Update();
}
