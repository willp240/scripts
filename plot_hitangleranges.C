
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

void plot_hitangleranges(  )
{
 // TCanvas c1;
  int col[14] = {2,46,49,48,45,44,3,8,29,4,9,38,39,1};//   6,5,41,32,8,3,29,7,9,4,14,1};
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  RAT::DB::Get()->SetAirplaneModeStatus(true);

  std::vector<TH1D*> hist_vec;
  TLegend *leg = new TLegend(0.57,0.57,0.87,0.87);
  leg->SetLineWidth(2);

  // Make ten histograms
  for(int hist_num = 13; hist_num >=0; hist_num--){

    TString hname;
    TString htitle;
    TString fname;

    int i = hist_num;

    if(i<8) {
      int minr = i;
      int maxr = i+1;
      hname = Form("Radius%dto%d", minr, maxr);
      htitle = Form("%dmm - %dmm", minr, maxr);
      fname = Form("/data/snoplus3/parkerw/ratSimulations/Apr11_3MeV_%dto%dm/*.root", minr, maxr);
    }
    else if (i<9){
      int minr = i;
      double maxr = i+0.5;
      hname = Form("Radius%dto%.1f", minr, maxr);
      htitle = Form("%dmm - %.1fmm", minr, maxr);
      fname = Form("/data/snoplus3/parkerw/ratSimulations/Apr11_3MeV_%dto%.1fm/*.root", minr, maxr);
    }
    else if (i<10){
      double minr = i-0.5;
      int maxr = i;
      hname = Form("Radius%.1fto%d", minr, maxr);
      htitle = Form("%.1fmm - %dmm", minr, maxr);
      fname = Form("/data/snoplus3/parkerw/ratSimulations/Apr11_3MeV_%.1fto%dm/*.root", minr, maxr);
    }
    else {
      int minr = i-1;
      int maxr = i;
      hname = Form("Radius%dto%d", minr, maxr);
      htitle = Form("%dmm - %dmm", minr, maxr);
      fname = Form("/data/snoplus3/parkerw/ratSimulations/Apr11_3MeV_%dto%dm/*.root", minr, maxr);
    }

    TH1D* h = new TH1D(hname, htitle, 90, 0, 180);
    h->GetXaxis()->SetTitle("Angle (degrees)");
    h->GetYaxis()->SetTitle("Normalised PMT Hits");
    h->GetYaxis()->SetTitleOffset(1.3);
    h->SetLineColor(col[hist_num]);
    h->SetLineWidth(2);
    hist_vec.push_back(h);

    std::cout << fname << std::endl;
    RAT::DU::DSReader dsReader( fname.Data() );

    TVector3 eventAxis (0.0, 0.0, 0.0);
    const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
    for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
      {
	      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
	      const TVector3 eventPosition = rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed
        if(rDS.GetEVCount() < 1)
          continue;
	      const RAT::DS::EV& rEV = rDS.GetEV( 0 );
 	      const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();

        // First, get the 10th percerntile hit PMT    
        std::vector<double> pmtTimes(calibratedPMTs.GetCount(), 0);
        for (size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++){
            const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );
            pmtTimes[iPMT] = pmtCal.GetTime();
            //std::cout << pmtCal.GetTime() << std::endl;
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
            
	          h->Fill(angle);
          }
      }
    hist_vec.push_back(h);
    h->Scale(1/h->Integral() );
    h->GetYaxis()->SetRangeUser(0,0.05);
    if(i == 13)
      h->Draw("hist");
    else
      h->Draw("hist same");
    leg->AddEntry(h, htitle, "l");
  }
  leg->Draw("same");
}

