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

#include <string>
#include <RAT/DU/DSReader.hh>
#include <string>


void TestLightPath( const std::string& fileName )
{
  RAT::DB::Get()->SetAirplaneModeStatus(false);

  RAT::DU::DSReader dsReader( fileName );

  RAT::DU::TimeResidualCalculator timeResCalc = RAT::DU::Utility::Get()->GetTimeResidualCalculator();

  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
    {
      std::cout << "eve nunm " << iEntry << std::endl;
      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
      if(rDS.GetEVCount()<1) continue;
      const RAT::DS::EV& rEV = rDS.GetEV(0);

      TVector3 eventPosition = rDS.GetMC().GetMCParticle(0).GetPosition();
      double eventTime = rDS.GetMC().GetMCParticle(0).GetTime();

      const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();

      for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
        {

          const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );

          // Time residuals estimate the photon emission time relative to the event start so subtract off the transit time and eventTime    
          double TimeResidual = timeResCalc.CalcTimeResidual(pmtCal, eventPosition, 390 - rDS.GetMCEV(0).GetGTTime(), true);
	  //          cout<<"Calc TRes complete"<<endl;
        }
    }

}
