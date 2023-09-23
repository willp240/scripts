
#include <RAT/DU/DSReader.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
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
#include <math.h>

void plot_highzratio(  )
{
  // TCanvas c1;
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  RAT::DB::Get()->SetAirplaneModeStatus(true);

  TString hname = "HighZAngularRatio";
  TString htitle = "HighZ Angular Ratio";
  TH2D* h = new TH2D(hname, htitle, 200, -6000, 14000, 100, 0, 1);
  h->GetXaxis()->SetTitle("Event Z, mm");
  h->GetYaxis()->SetTitle("Ratio");
  h->GetYaxis()->SetTitleOffset(1.3);
  h->SetLineWidth(2);

  std::string fname = "/data/snoplus3/parkerw/ratSimulations/Mar31_nearAV_3.0MeV/nearav_*.root";

  std::cout << fname << std::endl;
  RAT::DU::DSReader dsReader( fname );

  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
    {

      int nSmallAngle = 0;
      int nTotal = 0;
      TVector3 eventAxis (0.0, 0.0, 0.0);

	    const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
	    const TVector3 eventPosition = rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed
      double eventz = eventPosition.Z();
      if(rDS.GetEVCount() < 1)
        continue;
	    const RAT::DS::EV& rEV = rDS.GetEV( 0 );
 	    const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();

      // First, get the 10th percerntile hit PMT    
      std::vector<double> pmtTimes(calibratedPMTs.GetCount(), 0);
      for (size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++){
          const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );
          pmtTimes[iPMT] = pmtCal.GetTime();
        }

      int pmtStart = static_cast<int>(0.1 * calibratedPMTs.GetCount());
      nth_element(pmtTimes.begin(), pmtTimes.begin() + pmtStart, pmtTimes.end());
      double windowStart = pmtTimes[pmtStart] - 10; // ns
      double windowEnd = pmtTimes[pmtStart] + 10; // ns

      for (size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++)
        {
          const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );
          double pmtTime = pmtCal.GetTime();
          if(pmtTime >= windowStart && pmtTime <= windowEnd) {
              // Only include PMTs within the time window
              TVector3 pmtPos = pmtInfo.GetPosition(pmtCal.GetID()).Unit();
              eventAxis += pmtPos;
            }
        }
        
      eventAxis = eventAxis.Unit(); // normalise

	    for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
	      {
	        const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );
	        TVector3 pmtPosition = pmtInfo.GetPosition( pmtCal.GetID() );

	        double angle = acos((eventAxis.Dot(pmtPosition)) / (eventAxis.Mag() * pmtPosition.Mag())) * (180 / M_PI);
            
          if(angle > 50 && angle < 90)
            nSmallAngle++;
          nTotal++;
        }

      double ratio = (double)nSmallAngle / (double)nTotal;
      //std::cout << "Filling " << eventz << " " << ratio << std::endl;
      h->Fill(eventz, ratio);
    }

  h->Draw("colz");

}
