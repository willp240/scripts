////////////////////////////////////////////////////////////////////
/// \file PlotPMTAngDist.cc
///
/// \brief Functions to hit pmmt angles.
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
#include <RAT/LightPathStraightScint.hh>

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
void* PlotHitPMTAngDist( const std::string& fileName)
{

  gStyle->SetOptStat(0);

  TH2D* hPMTTrue = new TH2D( "hPMT_True", "True cos(#theta_{#gamma}) - Time Residual of PMT Hits", 400, -100, 300, 20, -1.0, 1.0 );
  TH2D* hPMTFit = new TH2D( "hPMT_Fit", "Fitted cos(#theta_{#gamma}) - Time Residual of PMT Hits", 400, -100, 300, 20, -1.0, 1.0 );

  // If this is being done on data that does not require remote database connection
  // eg.: a simple simulation with default run number (0)
  // We can disable the remote connections:
  //
  // NOTE: Don't do this if you are using real data!!!
  RAT::DB::Get()->SetAirplaneModeStatus(true);

  RAT::DU::DSReader dsReader( fileName );

  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry < 1; iEntry++)//dsReader.GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
      for( size_t iEV = 0; iEV < rDS.GetEVCount(); iEV++ )
        {
          const RAT::DS::EV& rEV = rDS.GetEV( iEV );

	  const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
          for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
            {
              const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );

	      //Get fit position
	      const RAT::DS::FitVertex& rVertex = rEV.GetFitResult("diPoFit").GetVertex(0);

              TVector3 fitPos = rVertex.GetPosition();
              double fitTime = rVertex.GetTime();
	      TVector3 fitDirection = rVertex.GetDirection();

	      TVector3 truePos(0,0,0);
	      double trueTime = 390 - rDS.GetMCEV(0).GetGTTime();
	      TVector3 trueDirection(1,0,0);

	      TVector3 pmtpos = pmtInfo.GetPosition( pmtCal.GetID() );

	      double distInAV = 0.0;
	      double distInWater = 0.0;
	      double distInTarget = 0.0;

	      RAT::LP::LightPathStraightScint::GetPath(pmtpos, fitPos, distInTarget, distInWater);

	      float fitTransitTime = RAT::DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( distInTarget, distInAV, distInWater );
	      float fitCorrectedTime = pmtCal.GetTime() - fitTransitTime - fitTime;

	      TVector3 fitPhotonDir = (pmtpos - fitPos).Unit();
	      double fitCosAngle = fitPhotonDir.Dot(fitDirection);

	      RAT::LP::LightPathStraightScint::GetPath(pmtpos, truePos, distInTarget, distInWater);

              float trueTransitTime = RAT::DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( distInTarget, distInAV, distInWater );
              float trueCorrectedTime = pmtCal.GetTime() - trueTransitTime - trueTime;

              TVector3 truePhotonDir = (pmtpos - truePos).Unit();
              double trueCosAngle = truePhotonDir.Dot(trueDirection);

	      if (trueCorrectedTime < -100)
		trueCorrectedTime = -100;
	      if (trueCorrectedTime > 300)
		trueCorrectedTime = 300;
	      if (fitCorrectedTime < -100)
                fitCorrectedTime = -100;
              if (fitCorrectedTime > 300)
		fitCorrectedTime = 300;

              hPMTTrue->Fill(trueCorrectedTime,trueCosAngle);
	      hPMTFit->Fill(fitCorrectedTime,fitCosAngle);
            }
        }
    }

  gStyle->SetPalette(55);
  TCanvas* c1 = new TCanvas();
  hPMTTrue->GetXaxis()->SetTitle( "Time Residual, ns" );
  hPMTTrue->GetYaxis()->SetTitle( "cos(#theta_{#gamma})" );
  //  hPMTTrue->Smooth();
  hPMTTrue->Draw("colz");

  TCanvas* c2 = new TCanvas();
  hPMTFit->GetXaxis()->SetTitle( "Time Residual, ns" );
  hPMTFit->GetYaxis()->SetTitle( "cos(#theta_{#gamma})" );
  //hPMTFit->Smooth();
  hPMTFit->Draw("colz");

  // return hHitTimeResiduals;
}

