
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

void plot_pmtzntuple(  )
{
  // TCanvas c1;
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  RAT::DB::Get()->SetAirplaneModeStatus(true);

  std::vector<TH1D*> hist_vec;

  std::string hist_names[6] = {"Ztoneg5m", "Zneg5mto0m", "Z0mto5m", "Z5mto6m", "Z6mto8p5m", "Z8p5mto40m"};
  std::string hist_titles[6] = {"Z: <-5m", "Z: -5m \- 0m", "Z: 0m \- 5m", "Z: 5 \- 6m", "Z: 6 \- 8.5m", "Z: >8.5m"}; 

  TLegend *leg = new TLegend(0.25,0.5,0.5,0.85);
  leg->SetLineWidth(2);

  // Make ten histograms
  for(int hist_num = 0; hist_num < 1; hist_num++){

    TString hname = hist_names[hist_num];
    TString htitle = hist_titles[hist_num];
    TH1D* h = new TH1D(hname, htitle, 34, -8500, 8500);
    h->GetXaxis()->SetTitle("Hit PMT Z, mm");
    h->GetYaxis()->SetTitle("Normalised Number of PMTs");
    h->GetYaxis()->SetTitleOffset(1.3);
    h->SetLineColor(hist_num+1);
    h->SetLineWidth(2);
    hist_vec.push_back(h);
    leg->AddEntry(h, htitle, "l");
  }

  std::string fname = "/data/snoplus3/parkerw/ratSimulations/Apr3_SkyShine_2.5MeV/skyshine_*.root";
  std::cout << fname << std::endl;
  RAT::DU::DSReader dsReader( fname );
  
  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
    {
	    const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
	    const TVector3 eventPosition = rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed
      double eventz = eventPosition.Z();
      if(rDS.GetEVCount() < 1)
        continue;
	    const RAT::DS::EV& rEV = rDS.GetEV( 0 );
 	    const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
	    for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
	      {
	        const RAT::DS::PMTCal& pmtCal = calibratedPMTs.GetPMT( iPMT );
	        TVector3 pmtPosition = pmtInfo.GetPosition( pmtCal.GetID() );
          double pmtz = pmtPosition.Z();

          //if( eventz <= -5000 )
	        hist_vec.at(0)->Fill(pmtz);
          /*else if( eventz <= 0 )
            hist_vec.at(1)->Fill(pmtz);
          else if( eventz <= 5000 )
            hist_vec.at(2)->Fill(pmtz);
          else if( eventz <= 6000 )
            hist_vec.at(3)->Fill(pmtz);
          else if( eventz <= 8500 )
            hist_vec.at(4)->Fill(pmtz);
          else if( eventz >= 8500 ){
            hist_vec.at(5)->Fill(pmtz);
            std::cout << "z " << eventz << " " << " nhits " << calibratedPMTs.GetCount() << std::endl;
          }*/ 
        }
    }

  for(int hist_num = 0; hist_num < 1; hist_num++)
  {
    hist_vec.at(hist_num)->Scale(1/hist_vec.at(hist_num)->Integral());
    if(hist_num==0){
      hist_vec.at(hist_num)->Draw("hist");
      //hist_vec.at(hist_num)->GetYaxis()->SetRangeUser(0,0.18);
    }
    else
      hist_vec.at(hist_num)->Draw("hist same");
  } 
  //leg->Draw("same");
}

