
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
#include <RAT/LightPathStraightScint.hh>
#include <RAT/DU/TimeResidualCalculator.hh>

#include <TFile.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TMath.h>

#include <string>

/// Plot the hit time residuals for the fit position
///
/// @param[in] fileName of the RAT::DS root file to analyse
/// @return the histogram plot
void* PlotTResid( const std::string& fileName)
{

  gStyle->SetOptStat(0);

  //  TH2D* hnewPDF = new TH2D( "newPDF", "True cos(#theta_{#gamma}) - Time Residual of PMT Hits", 400, -100, 300, 20, -1.0, 1.0 );

  TH1D* hTResid = new TH1D( "tResid", "Time Residual of PMT Hits", 400, -100, 300);

  // If this is being done on data that does not require remote database connection
  // eg.: a simple simulation with default run number (0)
  // We can disable the remote connections:
  //
  // NOTE: Don't do this if you are using real data!!!
  RAT::DB::Get()->SetAirplaneModeStatus(true);

  RAT::DU::DSReader dsReader( fileName );

  RAT::DU::TimeResidualCalculator timeResCalc = RAT::DU::Utility::Get()->GetTimeResidualCalculator(); // To calculate the light's path
  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry<dsReader.GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
      for( size_t iEV = 0; iEV < rDS.GetEVCount(); iEV++ )
        {
	  if(iEV>0)// man don't want no retriggers
	    continue;
          const RAT::DS::EV& rEV = rDS.GetEV( iEV );

	  const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
          for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
            {
              const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );

	      TVector3 truePos = rDS.GetMC().GetMCParticle(0).GetPosition();
	      double trueTime = 390 - rDS.GetMCEV(0).GetGTTime();
	      //TVector3 trueDirection = rDS.GetMC().GetMCParticle(0).GetMomentum().Unit();

	      double distInAV = 0.0;
              double distInWater = 0.0;
              double distInTarget = 0.0;

	      TVector3 pmtpos = pmtInfo.GetPosition( pmtCal.GetID() );

	      RAT::LP::LightPathStraightScint::GetPath(pmtpos, truePos, distInTarget, distInWater);

              float trueTransitTime = RAT::DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( distInTarget, distInAV, distInWater );
              float trueCorrectedTime = pmtCal.GetTime() - trueTransitTime - trueTime;

              //TVector3 truePhotonDir = (pmtpos - truePos).Unit();
              //double trueCosAngle = truePhotonDir.Dot(trueDirection);

	      double TimeResidual = timeResCalc.CalcTimeResidual(pmtCal, truePos, 390 - rDS.GetMCEV(0).GetGTTime(), true);

              hTResid->Fill(TimeResidual);
            }
        }

    }

  //  gStyle->SetPalette(55);
  TCanvas* c1 = new TCanvas();
  hTResid->GetXaxis()->SetTitle( "Time Residual, ns" );
  hTResid->GetYaxis()->SetTitle( "Events" );
  hTResid->Draw();

  TFile* file = new TFile("timeResid1.root","RECREATE");
  hTResid->Write();

  // return hHitTimeResiduals;
}

void* CompareTResid( const std::string& fileName1, const std::string& fileName2, const std::string& label1, const std::string& label2)
{

  gStyle->SetOptStat(0);

  TH1D* hTResid1 = new TH1D( "tResid1", "Time Residual of PMT Hits", 400, -100, 300);
  TH1D* hTResid2 = new TH1D( "tResid2", "Time Residual of PMT Hits", 400, -100, 300);

  // If this is being done on data that does not require remote database connection
  // eg.: a simple simulation with default run number (0)
  // We can disable the remote connections:
  //
  // NOTE: Don't do this if you are using real data!!!
  RAT::DB::Get()->SetAirplaneModeStatus(true);

  RAT::DU::DSReader dsReader1( fileName1 );

  RAT::DU::TimeResidualCalculator timeResCalc1 = RAT::DU::Utility::Get()->GetTimeResidualCalculator(); // To calculate the light's path
  const RAT::DU::PMTInfo& pmtInfo1 = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry<dsReader1.GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS1 = dsReader1.GetEntry( iEntry );
      for( size_t iEV = 0; iEV < rDS1.GetEVCount(); iEV++ )
        {
	  if(iEV>0)// man don't want no retriggers
	    continue;
          const RAT::DS::EV& rEV1 = rDS1.GetEV( iEV );

	  const RAT::DS::CalPMTs& calibratedPMTs1 = rEV1.GetCalPMTs();
          for( size_t iPMT = 0; iPMT < calibratedPMTs1.GetCount(); iPMT++ )
            {
              const RAT::DS::PMTCal& pmtCal1 = calibratedPMTs1.GetPMT( iPMT );

	      TVector3 truePos1 = rDS1.GetMC().GetMCParticle(0).GetPosition();
	      double trueTime1 = 390 - rDS1.GetMCEV(0).GetGTTime();

	      double distInAV1 = 0.0;
              double distInWater1 = 0.0;
              double distInTarget1 = 0.0;

	      TVector3 pmtpos1 = pmtInfo1.GetPosition( pmtCal1.GetID() );

	      RAT::LP::LightPathStraightScint::GetPath(pmtpos1, truePos1, distInTarget1, distInWater1);

              float trueTransitTime1 = RAT::DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( distInTarget1, distInAV1, distInWater1 );
              float trueCorrectedTime1 = pmtCal1.GetTime() - trueTransitTime1 - trueTime1;

	      double TimeResidual1 = timeResCalc1.CalcTimeResidual(pmtCal1, truePos1, 390 - rDS1.GetMCEV(0).GetGTTime(), true);

              hTResid1->Fill(TimeResidual1);
            }
        }
    }

  RAT::DU::DSReader dsReader2( fileName2 );

  RAT::DU::TimeResidualCalculator timeResCalc2 = RAT::DU::Utility::Get()->GetTimeResidualCalculator(); // To calculate the light's path
  const RAT::DU::PMTInfo& pmtInfo2 = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry<dsReader2.GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS2 = dsReader2.GetEntry( iEntry );
      for( size_t iEV = 0; iEV < rDS2.GetEVCount(); iEV++ )
        {
	  if(iEV>0)// man don't want no retriggers
	    continue;
          const RAT::DS::EV& rEV2 = rDS2.GetEV( iEV );

	  const RAT::DS::CalPMTs& calibratedPMTs2 = rEV2.GetCalPMTs();
          for( size_t iPMT = 0; iPMT < calibratedPMTs2.GetCount(); iPMT++ )
            {
              const RAT::DS::PMTCal& pmtCal2 = calibratedPMTs2.GetPMT( iPMT );

	      TVector3 truePos2 = rDS2.GetMC().GetMCParticle(0).GetPosition();
	      double trueTime2 = 390 - rDS2.GetMCEV(0).GetGTTime();

	      double distInAV2 = 0.0;
              double distInWater2 = 0.0;
              double distInTarget2 = 0.0;

	      TVector3 pmtpos2 = pmtInfo2.GetPosition( pmtCal2.GetID() );

	      RAT::LP::LightPathStraightScint::GetPath(pmtpos2, truePos2, distInTarget2, distInWater2);

              float trueTransitTime2 = RAT::DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( distInTarget2, distInAV2, distInWater2 );
              float trueCorrectedTime2 = pmtCal2.GetTime() - trueTransitTime2 - trueTime2;

	      double TimeResidual2 = timeResCalc2.CalcTimeResidual(pmtCal2, truePos2, 390 - rDS2.GetMCEV(0).GetGTTime(), true);

              hTResid2->Fill(TimeResidual2);
            }
        }

    }
  
  TCanvas* c1 = new TCanvas();
  hTResid1->GetXaxis()->SetTitle( "Time Residual, ns" );
  hTResid1->GetYaxis()->SetTitle( "Events" );
  hTResid1->SetLineColor(kRed);
  hTResid1->Draw();

  hTResid2->GetXaxis()->SetTitle( "Time Residual, ns" );
  hTResid2->GetYaxis()->SetTitle( "Events" );
  hTResid2->SetLineColor(kBlue);
  hTResid2->Draw("same");

  TLegend* t1 = new TLegend( 0.45, 0.75, 0.87, 0.9 );
  t1->AddEntry( hTResid1, label1.c_str(), "l" );
  t1->AddEntry( hTResid2, label2.c_str(), "l" );

  TFile* file = new TFile("timeResidComp.root","RECREATE");
  hTResid1->Write();
  hTResid2->Write();

}

void* PlotAngles( const std::string& fileName)
{

  gStyle->SetOptStat(0);

  TH2D* hnewPDF = new TH2D( "newPDF", "True cos(#theta_{#gamma}) - Time Residual of PMT Hits", 400, -100, 300, 20, -1.0, 1.0 );

  // If this is being done on data that does not require remote database connection
  // eg.: a simple simulation with default run number (0)
  // We can disable the remote connections:
  //
  // NOTE: Don't do this if you are using real data!!!
  RAT::DB::Get()->SetAirplaneModeStatus(true);

  RAT::DU::DSReader dsReader( fileName );

  RAT::DU::TimeResidualCalculator timeResCalc = RAT::DU::Utility::Get()->GetTimeResidualCalculator(); // To calculate the light's path
  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry<dsReader.GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
      for( size_t iEV = 0; iEV < rDS.GetEVCount(); iEV++ )
        {
	  if(iEV>0)// man don't want no retriggers
	    continue;
          const RAT::DS::EV& rEV = rDS.GetEV( iEV );

	  const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
          for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
            {
              const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );

	      TVector3 truePos = rDS.GetMC().GetMCParticle(0).GetPosition();
	      double trueTime = 390 - rDS.GetMCEV(0).GetGTTime();
	      TVector3 trueDirection = rDS.GetMC().GetMCParticle(0).GetMomentum().Unit();

	      double distInAV = 0.0;
              double distInWater = 0.0;
              double distInTarget = 0.0;

	      TVector3 pmtpos = pmtInfo.GetPosition( pmtCal.GetID() );

	      RAT::LP::LightPathStraightScint::GetPath(pmtpos, truePos, distInTarget, distInWater);

              float trueTransitTime = RAT::DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( distInTarget, distInAV, distInWater );
              float trueCorrectedTime = pmtCal.GetTime() - trueTransitTime - trueTime;

              TVector3 truePhotonDir = (pmtpos - truePos).Unit();
              double trueCosAngle = truePhotonDir.Dot(trueDirection);

	      double TimeResidual = timeResCalc.CalcTimeResidual(pmtCal, truePos, 390 - rDS.GetMCEV(0).GetGTTime(), true);

              hnewPDF->Fill(TimeResidual, trueCosAngle);
            }
        }

    }

  //  gStyle->SetPalette(55);
  TCanvas* c1 = new TCanvas();
  hnewPDF->GetXaxis()->SetTitle( "Time Residual, ns" );
  hnewPDF->GetXaxis()->SetRangeUser(-50,200);
  // hnewPDF->GetZaxis()->SetRangeUser(0,16000);
  hnewPDF->GetYaxis()->SetTitle( "cos(#theta_{#gamma})" );
  hnewPDF->GetZaxis()->SetTitle( "Events" );
  hnewPDF->GetXaxis()->SetTitleOffset(1.8);
  hnewPDF->GetYaxis()->SetTitleOffset(1.8);
  hnewPDF->GetZaxis()->SetTitleOffset(1.4);
  hnewPDF->Draw("lego");

  TFile* file = new TFile("timeResidAngle2NoCerenk.root","RECREATE");
  hnewPDF->Write();

  // return hHitTimeResiduals;
}
