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
void* Make2dPDF( const std::string& fileName)
{

  gStyle->SetOptStat(0);

  TH2D* hnewPDF = new TH2D( "newPDF", "True cos(#theta_{#gamma}) - Time Residual of PMT Hits", 400, -100, 300, 10, -1.0, 1.0 );

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

	      //Get fit position
	      //	      const RAT::DS::FitVertex& rVertex = rEV.GetFitResult("multiPDFFit").GetVertex(0);

	      TVector3 truePos = rDS.GetMC().GetMCParticle(0).GetPosition();
	      double trueTime = 390 - rDS.GetMCEV(0).GetGTTime();
	      TVector3 trueDirection = rDS.GetMC().GetMCParticle(0).GetMomentum().Unit();

	      //TVector3 fitPos = rVertex.GetPosition();
              //double fitTime = rVertex.GetTime();

	      double distInAV = 0.0;
              double distInWater = 0.0;
              double distInTarget = 0.0;

	      TVector3 pmtpos = pmtInfo.GetPosition( pmtCal.GetID() );

	      //RAT::LP::LightPathStraightScint::GetPath(pmtpos, fitPos, distInTarget, distInWater);

              //float fitTransitTime = RAT::DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( distInTarget, distInAV, distInWater );
              //float fitCorrectedTime = pmtCal.GetTime() - fitTransitTime - fitTime;

	      RAT::LP::LightPathStraightScint::GetPath(pmtpos, truePos, distInTarget, distInWater);

              float trueTransitTime = RAT::DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( distInTarget, distInAV, distInWater );
              float trueCorrectedTime = pmtCal.GetTime() - trueTransitTime - trueTime;

              TVector3 truePhotonDir = (pmtpos - truePos).Unit();
              double trueCosAngle = truePhotonDir.Dot(trueDirection);

	      double TimeResidual = timeResCalc.CalcTimeResidual(pmtCal, truePos, 390 - rDS.GetMCEV(0).GetGTTime(), true);

	      //	      std::cout << pmtCal.GetTime() << " " << trueTransitTime << " " << trueTime << " " << TimeResidual << std::endl;

	      /*	      if (trueCorrectedTime < -100)
		trueCorrectedTime = -100;
	      if (trueCorrectedTime > 300)
		trueCorrectedTime = 300;
	      if (fitCorrectedTime < -100)
                fitCorrectedTime = -100;
              if (fitCorrectedTime > 300)
                fitCorrectedTime = 300;
	      */
              hnewPDF->Fill(TimeResidual,trueCosAngle);
            }
        }

    }

  for(int i=1; i<=hnewPDF->GetXaxis()->GetNbins(); i++){
    for(int j=1; j<=hnewPDF->GetYaxis()->GetNbins(); j++){
      if(i>330)
	hnewPDF->SetBinContent(i,j, hnewPDF->GetBinContent(330,j));
      if(i<80)
	hnewPDF->SetBinContent(i,j, hnewPDF->GetBinContent(80,j));
    }
  }

  gStyle->SetPalette(55);
  TCanvas* c1 = new TCanvas();
  hnewPDF->GetXaxis()->SetTitle( "Time Residual, ns" );
  hnewPDF->GetYaxis()->SetTitle( "cos(#theta_{#gamma})" );
  //  hnewPDF->Smooth();
  hnewPDF->Draw("colz");

  TFile* file = new TFile("newPDF_centre_fitPos_10dirbins_tresidcalc.root","RECREATE");
  hnewPDF->Write();

  // return hHitTimeResiduals;
}

