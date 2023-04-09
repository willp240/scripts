
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

void plot_ratio(  )
{
 // TCanvas c1;
  int col[10] = {1,2,6,5,3,8,7,9,4,12};
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  RAT::DB::Get()->SetAirplaneModeStatus(true);

  double dip_low = 40;
  double dip_high = 90;

  TLegend *leg = new TLegend(0.57,0.77,0.87,0.87);
  leg->SetLineWidth(2);

  double es[4] = {0.5, 3.0, 5.0, 10.0};

  // Make four graphs
  for(int graph_num = 3; graph_num < 4; graph_num++){

    double energy = es[graph_num];

    TString gname = Form("Energy%.1fMeV", energy);
    TString gtitle = Form("%.1fMeV", energy);
    TGraph* g = new TGraph( );
    g->GetYaxis()->SetTitle("Ratio");
    g->GetXaxis()->SetTitle("True Radius, mm");
    g->GetYaxis()->SetTitleOffset(1.2);
    g->SetMarkerColor(graph_num+1);
    g->SetLineColor(graph_num+1);
    //g->SetMarkerStyle(4);
    //g->SetMarkerSize(2);

    TString fname = Form("/data/snoplus3/parkerw/ratSimulations/Mar31_nearAV_%.1fMeV/nearav_*.root", energy);
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
	//        std::cout << graph_num << " Setting point " << iEntry << " " << eventPosition.Mag() << " " << ratio << std::endl;
	      g->SetPoint(iEntry, eventPosition.Mag(), ratio);
      }
    if(graph_num == 3){
      g->GetYaxis()->SetRangeUser(0,1);
      g->Draw("AP");
    }
    else
      g->Draw("sameP");
    leg->AddEntry(g, gtitle, "l");
  }
  leg->Draw("same");
}

