
#include <RAT/DU/DSReader.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/DU/LightPathCalculator.hh>
#include <RAT/LightPathStraightScint.hh>
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

void* CompareLightPaths( const string& fileName )
{

  RAT::DB::Get()->SetAirplaneModeStatus(true);

  RAT::DU::DSReader dsReader( fileName );

  RAT::LP::LightPathStraightScint::BeginOfRun();

  //RAT::DU::Utility::Get()->GetLightPathCalculator() must be called *after* the RAT::DU::DSReader constructor.
  RAT::DU::LightPathCalculator lightPath = RAT::DU::Utility::Get()->GetLightPathCalculator(); // To calculate the light's path
  const RAT::DU::GroupVelocity& groupVelocity = RAT::DU::Utility::Get()->GetGroupVelocity(); // To get the group velocity
  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry < 1; iEntry++ )//dsReader.GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
      const TVector3 startPos = rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed
      double eventTime = rDS.GetMC().GetMCParticle(0).GetTime();
      for( size_t iEV = 0; iEV < 1; iEV++)//rDS.GetEVCount(); iEV++ )
        {
          const RAT::DS::EV& rEV = rDS.GetEV( iEV );
          const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
          for( size_t iPMT = 0; iPMT < 10; iPMT++ )//calibratedPMTs.GetCount(); iPMT++ )
            {
              const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );

	      const TVector3 pmtPos = pmtInfo.GetPosition( pmtCal.GetID() );

	      double distInAV = 0.0;
	      double distInWater = 0.0;
	      double distInTarget = 0.0;

	      RAT::LP::LightPathStraightScint::GetPath(pmtPos, startPos, distInTarget, distInWater);

	      float transitTime = RAT::DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( distInTarget, distInAV, distInWater );
	      float correctedTime = pmtCal.GetTime() - transitTime - eventTime;

	      std::cout << distInTarget << " " << distInWater << " " << transitTime << " " << correctedTime << std::endl;

            }
        }
    }
}
