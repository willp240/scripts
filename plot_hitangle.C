
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

void plot_hitangle(  )
{
 // TCanvas c1;
  int col[10] = {2,6,5,3,8,7,9,4,12,1};
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  RAT::DB::Get()->SetAirplaneModeStatus(true);

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
  c1->SetRightMargin(0.13);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  
  std::vector<TH1D*> hist_vec;
  TLegend *leg = new TLegend(0.57,0.27,0.87,0.87);
  leg->SetLineWidth(2);

  // Make ten histograms
  for(int hist_num = 9; hist_num >= 0; hist_num--){

    int minr = 5000 + hist_num*100;
    int maxr = 5000 + (hist_num+1)*100;

    TString hname = Form("Radius%dto%d", minr, maxr);
    TString htitle = Form("%dmm - %dmm", minr, maxr);
    TH1D* h = new TH1D(hname, "", 180, 0, 180);
    h->GetXaxis()->SetTitle("Angle (degrees)");
    h->GetYaxis()->SetTitle("Number of PMT Hits");
    h->GetYaxis()->SetTitleOffset(1.3);
    h->SetLineColor(col[hist_num]);
    h->SetLineWidth(2);
    hist_vec.push_back(h);

    TString fname = Form("/data/snoplus3/parkerw/ratSimulations/Nov29_NearAVShells/nearav_%d_%d.root", minr, maxr);
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
    h->Draw("same");
    leg->AddEntry(h, htitle, "l");
    h->GetYaxis()->SetTitleOffset(1.3);
  }
  leg->Draw("same");

  c1->SaveAs("nearAVangles.pdf");
  
}

