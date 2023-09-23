
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

void roccurve_nearav(  )
{

  std::vector<std::pair<double, double> > r_ratio_vec;

  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  RAT::DB::Get()->SetAirplaneModeStatus(true);

  double dip_low = 40;
  double dip_high = 90;
 
  TLegend *leg = new TLegend(0.57,0.77,0.87,0.87);
  leg->SetLineWidth(2);

  TString gname = Form("ROC3MeV");
  TString gtitle = Form("ROC 3MeV");
  TGraph* g = new TGraph( );
  g->GetYaxis()->SetTitle("Tagging Efficiency of Events with R > 5350 mm");
  g->GetXaxis()->SetTitle("Signal Sacrifice off Events with R < 5350 mm");
  g->GetYaxis()->SetTitleOffset(1.2);
  g->SetMarkerColor(1);
  g->SetLineColor(4);
  g->SetLineWidth(2);
  g->SetMarkerStyle(2);
  g->SetMarkerSize(4);

  TString fname = Form("/data/snoplus3/parkerw/ratSimulations/Mar31_nearAV_3.0MeV/nearav_*.root");
  std::cout << fname << std::endl;
  RAT::DU::DSReader dsReader( fname.Data() );

  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
    {

      double dip_count = 0;
      double total_count = 0;

	    const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
	    const TVector3 eventPosition = rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed
      if(rDS.GetEVCount() < 1)
        continue;
	    const RAT::DS::EV& rEV = rDS.GetEV( 0 );
 	    const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
	    for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
	      {
	        const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );
	        TVector3 pmtPosition = pmtInfo.GetPosition( pmtCal.GetID() );
	        double angle = acos((eventPosition.Dot(pmtPosition)) / (eventPosition.Mag() * pmtPosition.Mag())) * (180 / M_PI);
            
          if ((angle >= dip_low) && (angle <= dip_high))
            dip_count += 1.0;
          total_count += 1.0;
        }
        double ratio = dip_count / total_count;

        std::pair<double,double> p = std::make_pair(eventPosition.Mag(), ratio );
        r_ratio_vec.push_back(p);
        //std::cout << eventPosition.Mag() << " " << ratio << std::endl;

    }
  std::cout << "got evs " << std::endl;
  int point = 0;
  for(double r_threshold = 0.0; r_threshold <= 1.0; r_threshold += 0.01) 
    {
      //std::cout << "doing " << r_threshold << std::endl;
      double nearav_count = 0;
      double tagged_nearav_count = 0;
      double non_nearav_count = 0;
      double tagged_non_nearav_count = 0;

      for(size_t eve = 0; eve < r_ratio_vec.size(); eve++)
      {

        if(r_ratio_vec.at(eve).first > 6000)
          continue;
        // if should be treated as nearav
        if(r_ratio_vec.at(eve).second < r_threshold)
        {
          if(r_ratio_vec.at(eve).first > 5350)
            tagged_nearav_count++;
          else
            tagged_non_nearav_count++;
        }
        if(r_ratio_vec.at(eve).first > 5350)
          nearav_count++;
        else
          non_nearav_count++;
      }

      double efficiency = tagged_nearav_count / nearav_count;
      double sacrifice = tagged_non_nearav_count / non_nearav_count;

      g->SetPoint(point, sacrifice, efficiency);
      std::cout << r_threshold << "\t" << sacrifice << "\t" << efficiency << std::endl;
      point++;
    }

   g->Draw("AL");

}

