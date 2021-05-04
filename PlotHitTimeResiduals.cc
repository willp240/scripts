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
#include "TFile.h"
#include "TPaveText.h"

#include <string>
using namespace std;

/// Plot the hit time residuals for the MC position
///
/// @param[in] fileName of the RAT::DS root file to analyse
/// @return the histogram plot
TH1D* PlotHitTimeResidualsMCPosition( const string& fileName )
{
  TH1D* hHitTimeResiduals = new TH1D( "hHitTimeResidualsMC", "Hit time residuals using the MC position", 401, -100.5, 300.5 );
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
      const TVector3 eventPosition = rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed
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
TH1D* PlotHitTimeResidualsFitPosition( const string& fileName, std::string fitName = "")
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
void PlotHitTimeResiduals( const string& fileName )
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

void PlotHitTimeResidualsComp( const string& fileName, const string& fileName2 )
{
  gStyle->SetFillColor( kWhite );
  TCanvas* c1 = new TCanvas();
  TH1D* mc = PlotHitTimeResidualsMCPosition( fileName );
  TH1D* fit = PlotHitTimeResidualsFitPosition( fileName );
  TH1D* mc2 = PlotHitTimeResidualsMCPosition( fileName2 );
  TH1D* fit2 = PlotHitTimeResidualsFitPosition( fileName2 );
  mc->SetLineColor( kBlue );
  mc->Draw();
  fit->SetLineColor( kBlue+2 );
  fit->SetLineStyle( 2 );
  fit->Draw("SAME");

  mc2->SetLineColor( kRed );
  mc2->Draw("SAME");
  fit2->SetLineColor( kRed + 2 );
  fit2->SetLineStyle(2);
  fit2->Draw("SAME");
  
  TLegend* t1 = new TLegend( 0.7, 0.7, 0.9, 0.9 );
  t1->AddEntry( mc, "ScintFitter MC Position", "l" );
  t1->AddEntry( fit, "ScintFitter Fit Position", "l" );
  t1->AddEntry( mc2, "New Fitter MC Position", "l" );
  t1->AddEntry( fit2, "New Fitter Fit Position", "l" );
  t1->Draw();
  c1->Update();
}

void PlotHitTimeResidualsCompFitters( const string& fileName, const string& fitter1, const string& fitter2 )
{
  gStyle->SetFillColor( kWhite );
  TCanvas* c1 = new TCanvas();
  TH1D* mc = PlotHitTimeResidualsMCPosition( fileName );
  TH1D* fit = PlotHitTimeResidualsFitPosition( fileName, fitter1 );
  TH1D* mc2 = PlotHitTimeResidualsMCPosition( fileName );
  TH1D* fit2 = PlotHitTimeResidualsFitPosition( fileName, fitter2 );
  mc->SetLineColor( kBlue );
  //mc->Draw();
  fit->SetLineColor( kBlue+2 );
  //fit->SetLineStyle( 2 );
  fit->Draw();

  mc2->SetLineColor( kRed );
  //mc2->Draw("SAME");
  fit2->SetLineColor( kRed + 2 );
  fit2->SetLineStyle(2);
  fit2->Draw("SAME");

  TLegend* t1 = new TLegend( 0.7, 0.7, 0.9, 0.9 );
  //  t1->AddEntry( mc, "ScintFitter MC Position", "l" );
  t1->AddEntry( fit, "ScintFitter Fit Position", "l" );
  //t1->AddEntry( mc2, "New Fitter MC Position", "l" );
  t1->AddEntry( fit2, "New Fitter Fit Position", "l" );
  t1->Draw();
  c1->Update();
}

TH1D* PlotHitTimeResidualsMCPositionZCut( const string& fileName )
{
  TH1D* hHitTimeResiduals1 = new TH1D( "hHitTimeResidualsMC", "Hit time residuals using the MC position", 700, -200.0, 500.0 );
  TH1D* hHitTimeResiduals2 = new TH1D( "hHitTimeResidualsMC", "Hit time residuals using the MC position", 700, -200.0, 500.0 );
  TH1D* hHitTimeResiduals3 = new TH1D( "hHitTimeResidualsMC", "Hit time residuals using the MC position", 700, -200.0, 500.0 );
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
      const TVector3 eventPosition = rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed
      for( size_t iEV = 0; iEV < rDS.GetEVCount(); iEV++ )
        {
          const RAT::DS::EV& rEV = rDS.GetEV( iEV );
          const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
          for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
            {
              const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );

              lightPath.CalcByPosition( eventPosition, pmtInfo.GetPosition( pmtCal.GetID() ) );

	      double distInAV = lightPath.GetDistInAV();
	      double distInWater = lightPath.GetDistInWater();
	      double distInUpperTarget = lightPath.GetDistInUpperTarget();
	      double distInLowerTarget = lightPath.GetDistInLowerTarget();
	      const double transitTime = groupVelocity.CalcByDistance( distInUpperTarget, distInAV, distInWater+distInLowerTarget );

              // Time residuals estimate the photon emission time relative to the event start so subtract off the transit time
              // hit times are relative to the trigger time, which will depend on event time and detector position so correct for that to line up events
              // The 390ns corrects for the electronics delays and places the pulse in the middle of the window

              if(pmtInfo.GetPosition( pmtCal.GetID() ).Z()<747.48)
                hHitTimeResiduals1->Fill( pmtCal.GetTime() - transitTime - 390 + rDS.GetMCEV(iEV).GetGTTime());
	      //              else if (pmtInfo.GetPosition( pmtCal.GetID() ).Z()<747.48+100)
	      //hHitTimeResiduals2->Fill( pmtCal.GetTime() - transitTime - 390 + rDS.GetMCEV(iEV).GetGTTime());
	      else 
		hHitTimeResiduals3->Fill( pmtCal.GetTime() - transitTime - 390 + rDS.GetMCEV(iEV).GetGTTime());
            }
        }
    }
  hHitTimeResiduals3->GetYaxis()->SetTitle( "Normalised Counts" );
  hHitTimeResiduals3->GetXaxis()->SetTitle( "Hit time residuals [ns]" );
  //hHitTimeResiduals2->Scale(1/hHitTimeResiduals2->Integral());
  hHitTimeResiduals1->Scale(1/hHitTimeResiduals1->Integral());
  hHitTimeResiduals3->Scale(1/hHitTimeResiduals3->Integral());
  hHitTimeResiduals3->Draw();
  hHitTimeResiduals1->SetLineColor(kRed);
  hHitTimeResiduals3->SetLineColor(kBlue);
  hHitTimeResiduals1->Draw("same");
  //hHitTimeResiduals2->Draw("same");
  TLegend* t1 = new TLegend( 0.5, 0.6, 0.9, 0.9 );
  t1->AddEntry( hHitTimeResiduals1, "PMTs with z<Fill-Level", "l" );
  //  t1->AddEntry( hHitTimeResiduals2, "PMTs with Fill-Level<z<Fill-Level+100", "l" );
  t1->AddEntry( hHitTimeResiduals3, "PMTs with z>Fill-Level", "l" );
  t1->Draw();

  return hHitTimeResiduals1;
}


TCanvas* PlotHitTimeResidualsMCPositionZCut1m( const string& fileName )
{

  TCanvas *c = new TCanvas("c1", "c1", 1580, 1080);
  c->SetTopMargin(0.07);
  c->SetBottomMargin(0.11);
  c->SetLeftMargin(0.10);
  c->SetRightMargin(0.1);
  c->cd();

  int numHistos = 8;
  TH1D* hHitTimeResiduals[numHistos];
  for (int i=0; i<numHistos; i++){
    hHitTimeResiduals[i] = new TH1D( Form("h%d",i), "Hit time residuals using the MC position", 700, -200.0, 500.0 );
  }

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
      const TVector3 eventPosition = rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed
      for( size_t iEV = 0; iEV < rDS.GetEVCount(); iEV++ )
        {
          const RAT::DS::EV& rEV = rDS.GetEV( iEV );
          const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
          for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
            {
              const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );

	      lightPath.CalcByPositionPartial( eventPosition, pmtInfo.GetPosition( pmtCal.GetID() ) );

              double distInAV = lightPath.GetDistInAV();
              double distInWater = lightPath.GetDistInWater();
              double distInUpperTarget = lightPath.GetDistInUpperTarget();
              double distInLowerTarget = lightPath.GetDistInLowerTarget();
              const double transitTime = groupVelocity.CalcByDistance( distInUpperTarget, distInAV, distInWater+distInLowerTarget );

	      //lightPath.CalcByPosition( eventPosition, pmtInfo.GetPosition( pmtCal.GetID() ) );
              //double distInInnerAV = lightPath.GetDistInInnerAV();
              //double distInAV = lightPath.GetDistInAV();
              //double distInWater = lightPath.GetDistInWater();
              //const double transitTime = groupVelocity.CalcByDistance( distInInnerAV, distInAV, distInWater ); // Assumes a 400nm photon

              // Time residuals estimate the photon emission time relative to the event start so subtract off the transit time
              // hit times are relative to the trigger time, which will depend on event time and detector position so correct for that to line up events
              // The 390ns corrects for the electronics delays and places the pulse in the middle of the window

	      //round up height to nearest m to find boundary. /1000 to get in m, add 3 to index histos from 0
	      int histnum = ceil(pmtInfo.GetPosition( pmtCal.GetID() ).Z()/1000) + 3;
	      if(histnum<0)
		histnum=0;
	      if(histnum>numHistos-1)
		histnum=numHistos-1;

	      hHitTimeResiduals[histnum]->Fill( pmtCal.GetTime() - transitTime - 390 + rDS.GetMCEV(iEV).GetGTTime());
            }
        }
    }

  double max = 0.0;
  for(int i=0; i<numHistos; i++){
    hHitTimeResiduals[i]->Scale(1/hHitTimeResiduals[i]->Integral());
    if(hHitTimeResiduals[i]->GetMaximum() > max)
      max=hHitTimeResiduals[i]->GetMaximum();
  }
  hHitTimeResiduals[0]->GetYaxis()->SetTitle( "Normalised Counts" );
  hHitTimeResiduals[0]->GetXaxis()->SetTitle( "Hit time residuals [ns]" );
  hHitTimeResiduals[0]->GetYaxis()->SetRangeUser(0,max*1.1);
  hHitTimeResiduals[0]->GetXaxis()->SetRangeUser(-5,100);

  int col[8] = {1,4,7,3,5,6,2,28}; 

  for(int i=0; i<numHistos; i++){
    hHitTimeResiduals[i]->SetLineColor(col[i]);
  }

  hHitTimeResiduals[0]->Draw();
  for(int i=0; i<numHistos; i++){
    hHitTimeResiduals[i]->Draw("same");
  }

  TLegend* t1 = new TLegend( 0.5, 0.6, 0.88, 0.9);
  t1->AddEntry( hHitTimeResiduals[0], "PMTs with z<-3m", "l" );
  for(int i=1; i<numHistos-1; i++){
    t1->AddEntry( hHitTimeResiduals[i], Form("PMTs with %dm<z<%dm",i-4,i-3), "l" );
  }
  t1->AddEntry( hHitTimeResiduals[numHistos-1], "PMTs with z>3m", "l" );

  t1->Draw();

  return c;
}


TCanvas* PlotHitTimeResidualsMCPositionZCut4( const string& fileName )
{

  TCanvas *c = new TCanvas("c1", "c1", 1580, 1080);
  c->SetTopMargin(0.07);
  c->SetBottomMargin(0.11);
  c->SetLeftMargin(0.10);
  c->SetRightMargin(0.1);
  c->cd();

  int numHistos = 4;
  TH1D* hHitTimeResiduals[numHistos];
  for (int i=0; i<numHistos; i++){
    hHitTimeResiduals[i] = new TH1D( Form("h%d",i), "Hit time residuals using the MC position", 700, -200.0, 500.0 );
  }

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
      const TVector3 eventPosition = rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed
      for( size_t iEV = 0; iEV < rDS.GetEVCount(); iEV++ )
        {
          const RAT::DS::EV& rEV = rDS.GetEV( iEV );
          const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
          for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
            {
              const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );

              lightPath.CalcByPositionPartial( eventPosition, pmtInfo.GetPosition( pmtCal.GetID() ) );

              double distInAV = lightPath.GetDistInAV();
              double distInWater = lightPath.GetDistInWater();
              double distInUpperTarget = lightPath.GetDistInUpperTarget();
              double distInLowerTarget = lightPath.GetDistInLowerTarget();
              const double transitTime = groupVelocity.CalcByDistance( distInUpperTarget, distInAV, distInWater+distInLowerTarget );

              // Time residuals estimate the photon emission time relative to the event start so subtract off the transit time
              // hit times are relative to the trigger time, which will depend on event time and detector position so correct for that to line up events
              // The 390ns corrects for the electronics delays and places the pulse in the middle of the window

              int histnum;
              if(pmtInfo.GetPosition( pmtCal.GetID() ).Z()<-1000)
		histnum=0;
	      else if(pmtInfo.GetPosition( pmtCal.GetID() ).Z()<0)
                histnum=1;
	      else if(pmtInfo.GetPosition( pmtCal.GetID() ).Z()<1000)
		histnum=2;
	      else
		histnum=3;

	      hHitTimeResiduals[histnum]->Fill( pmtCal.GetTime() - transitTime - 390 + rDS.GetMCEV(iEV).GetGTTime());
            }
        }
    }

  double max = 0.0;
  for(int i=0; i<numHistos; i++){
    if(hHitTimeResiduals[i]->Integral()>0)
      hHitTimeResiduals[i]->Scale(1/hHitTimeResiduals[i]->Integral());
    else
      std::cout << " integral " << i << " <=0 " << hHitTimeResiduals[i]->Integral() << std::endl;
    if(hHitTimeResiduals[i]->GetMaximum() > max)
      max=hHitTimeResiduals[i]->GetMaximum();
  }
  hHitTimeResiduals[0]->GetYaxis()->SetTitle( "Normalised Counts" );
  hHitTimeResiduals[0]->GetXaxis()->SetTitle( "Hit time residuals [ns]" );
  hHitTimeResiduals[0]->GetYaxis()->SetRangeUser(0,max*1.1);
  hHitTimeResiduals[0]->GetXaxis()->SetRangeUser(-5,100);

  int col[4] = {1,4,3,2};

  for(int i=0; i<numHistos; i++){
    hHitTimeResiduals[i]->SetLineColor(col[i]);
  }

  hHitTimeResiduals[0]->Draw();
  for(int i=0; i<numHistos; i++){
    hHitTimeResiduals[i]->Draw("same");
  }

  TLegend* t1 = new TLegend( 0.5, 0.6, 0.88, 0.9);
  t1->AddEntry( hHitTimeResiduals[0], "PMTs with z<-1m", "l" );
  t1->AddEntry( hHitTimeResiduals[1], "PMTs with -1m<z<0", "l" );
  t1->AddEntry( hHitTimeResiduals[2], "PMTs with 0<z<1m", "l" );
  t1->AddEntry( hHitTimeResiduals[3], "PMTs with 1m<z", "l" );

  t1->Draw();

  return c;
}



void* LoopOverFiles(){
  TCanvas* c = new TCanvas("canv", "canv", 1580, 1080);
  c->SetTopMargin(0.07);
  c->SetBottomMargin(0.11);
  c->SetLeftMargin(0.10);
  c->SetRightMargin(0.1);

  TString path = "/home/parkerw/Software/rat/vesselScan_1MeV/root/";
  TFile *outfile = TFile::Open("canvases.root","RECREATE");
  gStyle->SetOptStat(0);
  c->Print((std::string("canvases4Z.pdf[")).c_str());

  for(int z=1000; z<4000; z+=1000){

    for(int x=0000; x<6000; x+=1000){

      //    for(int z=1000; z<4000; z+=1000){

      TString filename;
      if(z<0)
	filename= Form("X%d_Zm%d",x,abs(z));
      else
	filename= Form("X%d_Z%d",x,z);

      TString fullpath=path+filename+".root";
      std::cout << "do for " <<fullpath <<std::endl;
       
      TCanvas* c1 = PlotHitTimeResidualsMCPositionZCut4((fullpath.Data()));
      c1->cd();
      c1->Draw();
      TPaveText *pt = new TPaveText(.5,.5,.88,.58,"brNDC");
      pt->AddText(Form("X=%dm; Z=%dm",x/1000, z/1000));
      pt->Draw();
      c1->Update();
      c1->Write(filename);
      c1->Print((std::string("canvases4Z.pdf")).c_str());
      c1->Draw();
    }
  }
  c->Print((std::string("canvases4Z.pdf]")).c_str());
}


TH1D* PlotHitTimeResidualsMCPositionZCutOnly( const string& fileName )
{
  TH1D* hHitTimeResiduals1 = new TH1D( "hHitTimeResidualsMC", "Hit time residuals using the MC position", 401, -100.5, 300.5 );
  TH1D* hHitTimeResiduals2 = new TH1D( "hHitTimeResidualsMC", "Hit time residuals using the MC position", 401, -100.5, 300.5 );
  
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
      const TVector3 eventPosition = rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed
      for( size_t iEV = 0; iEV < rDS.GetEVCount(); iEV++ )
        {
          const RAT::DS::EV& rEV = rDS.GetEV( iEV );
          const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
          for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
            {
              const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );

	      lightPath.CalcByPositionPartial( eventPosition, pmtInfo.GetPosition( pmtCal.GetID() ) );

              double distInAV = lightPath.GetDistInAV();
              double distInWater = lightPath.GetDistInWater();
              double distInUpperTarget = lightPath.GetDistInUpperTarget();
              double distInLowerTarget = lightPath.GetDistInLowerTarget();
              const double transitTime = groupVelocity.CalcByDistance( distInUpperTarget, distInAV, distInWater+distInLowerTarget );

              // Time residuals estimate the photon emission time relative to the event start so subtract off the transit time
              // hit times are relative to the trigger time, which will depend on event time and detector position so correct for that to line up events
              // The 390ns corrects for the electronics delays and places the pulse in the middle of the window

              if(pmtInfo.GetPosition( pmtCal.GetID() ).Z()<0)
                hHitTimeResiduals1->Fill( pmtCal.GetTime() - transitTime - 390 + rDS.GetMCEV(iEV).GetGTTime());
              else
                hHitTimeResiduals2->Fill( pmtCal.GetTime() - transitTime - 390 + rDS.GetMCEV(iEV).GetGTTime());
            }
        }
    }
  hHitTimeResiduals2->GetYaxis()->SetTitle( "Normalised Counts" );
  hHitTimeResiduals2->GetXaxis()->SetTitle( "Hit time residuals [ns]" );
  //  hHitTimeResiduals1->Scale(1/hHitTimeResiduals1->Integral());
  // hHitTimeResiduals2->Scale(1/hHitTimeResiduals2->Integral());
  hHitTimeResiduals2->Draw();
  hHitTimeResiduals1->SetLineColor(kRed);
  hHitTimeResiduals2->SetLineColor(kBlue);
  hHitTimeResiduals1->Draw("same");
  //hHitTimeResiduals2->Draw("same");
  TLegend* t1 = new TLegend( 0.5, 0.6, 0.9, 0.9 );
  t1->AddEntry( hHitTimeResiduals1, "PMTs with z<Equator", "l" );
  //  t1->AddEntry( hHitTimeResiduals2, "PMTs with Fill-Level<z<Fill-Level+100", "l" );
  t1->AddEntry( hHitTimeResiduals2, "PMTs with z>Equator", "l" );
  t1->Draw();

  TFile *outfile = TFile::Open("vesselScan_3MeV/pdfs/twoZcut/X000_Z1000_twoZcut.root","RECREATE");

  hHitTimeResiduals1->Write("pdf_zbelow0");
  hHitTimeResiduals2->Write("pdf_zabove0");

  return hHitTimeResiduals1;
}


TH1D* PlotHitTimeResidualsMCPositionCompFiles( const string& fileName, const string& fileName2 )
{
  TH1D* hHitTimeResiduals1 = new TH1D( "hHitTimeResidualsMC1", "Hit time residuals using the MC position", 401, -100.5, 300.5 );
  TH1D* hHitTimeResiduals2 = new TH1D( "hHitTimeResidualsMC2", "Hit time residuals using the MC position", 401, -100.5, 300.5 );
  TH1D* hHitTimeResiduals3 = PlotHitTimeResidualsMCPosition(fileName2);

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
      const TVector3 eventPosition = rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed
      for( size_t iEV = 0; iEV < rDS.GetEVCount(); iEV++ )
        {
          const RAT::DS::EV& rEV = rDS.GetEV( iEV );
          const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
          for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
            {
              const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );

	      lightPath.CalcByPositionPartial( eventPosition, pmtInfo.GetPosition( pmtCal.GetID() ) );

              double distInAV = lightPath.GetDistInAV();
              double distInWater = lightPath.GetDistInWater();
              double distInUpperTarget = lightPath.GetDistInUpperTarget();
              double distInLowerTarget = lightPath.GetDistInLowerTarget();
              const double transitTime = groupVelocity.CalcByDistance( distInUpperTarget, distInAV, distInWater+distInLowerTarget );

              // Time residuals estimate the photon emission time relative to the event start so subtract off the transit time
              // hit times are relative to the trigger time, which will depend on event time and detector position so correct for that to line up events
              // The 390ns corrects for the electronics delays and places the pulse in the middle of the window

              if(pmtInfo.GetPosition( pmtCal.GetID() ).Z()<0)
                hHitTimeResiduals1->Fill( pmtCal.GetTime() - transitTime - 390 + rDS.GetMCEV(iEV).GetGTTime());
              else
                hHitTimeResiduals2->Fill( pmtCal.GetTime() - transitTime - 390 + rDS.GetMCEV(iEV).GetGTTime());
            }
        }
    }
  hHitTimeResiduals2->GetYaxis()->SetTitle( "Normalised Counts" );

  hHitTimeResiduals1->Scale(1/hHitTimeResiduals1->Integral());
  hHitTimeResiduals2->Scale(1/hHitTimeResiduals2->Integral());
  hHitTimeResiduals3->Scale(1/hHitTimeResiduals3->Integral());
  
  hHitTimeResiduals2->Draw();
  hHitTimeResiduals1->SetLineColor(kRed);
  hHitTimeResiduals2->SetLineColor(kBlue);
  hHitTimeResiduals3->SetLineColor(kBlack);
  hHitTimeResiduals1->Draw("same");
  hHitTimeResiduals3->Draw("same");
  //hHitTimeResiduals2->Draw("same");
  TLegend* t1 = new TLegend( 0.5, 0.6, 0.9, 0.9 );
  t1->AddEntry( hHitTimeResiduals1, "PMTs with z<Equator", "l" );
  //  t1->AddEntry( hHitTimeResiduals2, "PMTs with Fill-Level<z<Fill-Level+100", "l" );
  t1->AddEntry( hHitTimeResiduals2, "PMTs with z>Equator", "l" );
  t1->AddEntry( hHitTimeResiduals3, "ScintFitter (ET1D PDF)", "l" );
  t1->Draw();

  TFile *outfile = TFile::Open("vesselScan_3MeV/pdfs/twoZcut/X000_Z1000_twoZcut.root","RECREATE");

  hHitTimeResiduals1->Write("pdf_zbelow0");
  hHitTimeResiduals2->Write("pdf_zabove0");

  return hHitTimeResiduals1;
}


TCanvas* PlotHitTimeResidualsMCPositionZCut1m( const string& fileName )
{

  TCanvas *c = new TCanvas("c1", "c1", 1580, 1080);
  c->SetTopMargin(0.07);
  c->SetBottomMargin(0.11);
  c->SetLeftMargin(0.10);
  c->SetRightMargin(0.1);
  c->cd();

  int numHistos = 12;
  TH1D* hHitTimeResiduals[numHistos];
  for (int i=0; i<numHistos; i++){
    hHitTimeResiduals[i] = new TH1D( Form("h%d",i), "Hit time residuals using the MC position", 700, -200.0, 500.0 );
  }

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
      const TVector3 eventPosition = rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed
      for( size_t iEV = 0; iEV < rDS.GetEVCount(); iEV++ )
        {
          const RAT::DS::EV& rEV = rDS.GetEV( iEV );
          const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
          for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
            {
              const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );

	      lightPath.CalcByPositionPartial( eventPosition, pmtInfo.GetPosition( pmtCal.GetID() ) );

              double distInAV = lightPath.GetDistInAV();
              double distInWater = lightPath.GetDistInWater();
              double distInUpperTarget = lightPath.GetDistInUpperTarget();
              double distInLowerTarget = lightPath.GetDistInLowerTarget();
              const double transitTime = groupVelocity.CalcByDistance( distInUpperTarget, distInAV, distInWater+distInLowerTarget );

	      //lightPath.CalcByPosition( eventPosition, pmtInfo.GetPosition( pmtCal.GetID() ) );
              //double distInInnerAV = lightPath.GetDistInInnerAV();
              //double distInAV = lightPath.GetDistInAV();
              //double distInWater = lightPath.GetDistInWater();
              //const double transitTime = groupVelocity.CalcByDistance( distInInnerAV, distInAV, distInWater ); // Assumes a 400nm photon

              // Time residuals estimate the photon emission time relative to the event start so subtract off the transit time
              // hit times are relative to the trigger time, which will depend on event time and detector position so correct for that to line up events
              // The 390ns corrects for the electronics delays and places the pulse in the middle of the window

	      //round up height to nearest m to find boundary. /1000 to get in m, add 3 to index histos from 0
	      int histnum = ceil(pmtInfo.GetPosition( pmtCal.GetID() ).Z()/1000) + 6;
	      if(histnum<0)
		histnum=0;
	      if(histnum>numHistos-1)
		histnum=numHistos-1;

	      hHitTimeResiduals[histnum]->Fill( pmtCal.GetTime() - transitTime - 390 + rDS.GetMCEV(iEV).GetGTTime());
            }
        }
    }

  double max = 0.0;
  for(int i=0; i<numHistos; i++){
    hHitTimeResiduals[i]->Scale(1/hHitTimeResiduals[i]->Integral());
    if(hHitTimeResiduals[i]->GetMaximum() > max)
      max=hHitTimeResiduals[i]->GetMaximum();
  }
  hHitTimeResiduals[0]->GetYaxis()->SetTitle( "Normalised Counts" );
  hHitTimeResiduals[0]->GetXaxis()->SetTitle( "Hit time residuals [ns]" );
  hHitTimeResiduals[0]->GetYaxis()->SetRangeUser(0,max*1.1);
  hHitTimeResiduals[0]->GetXaxis()->SetRangeUser(-5,100);

  int col[8] = {1,4,7,3,5,6,2,28}; 
  int col[16] = {1,13,4,9,38,7,29,8,32,41,5,6,46,48,2,28}

  for(int i=0; i<numHistos; i++){
    hHitTimeResiduals[i]->SetLineColor(col[i]);
  }

  hHitTimeResiduals[0]->Draw();
  for(int i=0; i<numHistos; i++){
    hHitTimeResiduals[i]->Draw("same");
  }

  TLegend* t1 = new TLegend( 0.5, 0.6, 0.88, 0.9);
  t1->AddEntry( hHitTimeResiduals[0], "PMTs with z<-3m", "l" );
  for(int i=1; i<numHistos-1; i++){
    t1->AddEntry( hHitTimeResiduals[i], Form("PMTs with %dm<z<%dm",i-4,i-3), "l" );
  }
  t1->AddEntry( hHitTimeResiduals[numHistos-1], "PMTs with z>3m", "l" );

  t1->Draw();

  return c;
}
