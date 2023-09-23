
#include <RAT/DU/DSReader.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/MC.hh>
#include <RAT/DS/EV.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/FitResult.hh>
#include <RAT/DS/PMTSet.hh>
#include <RAT/DU/TimeResidualCalculator.hh>

#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>

#include <string>
#include <math.h>

void plot_ITR(  )
{
  // TCanvas c1;
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  RAT::DB::Get()->SetAirplaneModeStatus(true);

  RAT::DU::TimeResidualCalculator fTRCalc = RAT::DU::Utility::Get()->GetTimeResidualCalculator();

  TString hname = "ITR";
  TString htitle = "ITR";
  TH2D* h = new TH2D(hname, htitle, 200, -6000, 14000, 100, 0, 1);
  h->GetXaxis()->SetTitle("Event Z, mm");
  h->GetYaxis()->SetTitle("ITR");
  h->GetYaxis()->SetTitleOffset(1.3);
  h->SetLineWidth(2);

  TH2D* h2 = new TH2D("FitvsTrueR", "FitvsTrueR", 200, 0, 14000, 200, 0, 14000);
  h2->GetXaxis()->SetTitle("True R, mm");
  h2->GetYaxis()->SetTitle("Fitted R, mm");
  h2->GetYaxis()->SetTitleOffset(1.3);
  h2->SetLineWidth(2);

  std::string fname = "/data/snoplus3/parkerw/ratSimulations/Apr7_3MeV_SF/skyshine_*.root";
  //std::string fname ="./test.root";

  std::cout << fname << std::endl;
  RAT::DU::DSReader dsReader( fname );
  
  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
    {

      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
      if(rDS.GetEVCount() < 1)
        continue;
      const RAT::DS::EV& rEV = rDS.GetEV( 0 );
      
      //std::vector<std::string> cnames = rEV.GetClassifierNames();
      //std::cout << "bool " << rEV.ClassifierResultExists("ITR:scintFitter") << std::endl
      //  for(int i=0; i< cnames.size(); i++)
      //std::cout << cnames.at(i) << std::endl;
      if(!rEV.ClassifierResultExists("ITR:scintFitter"))
	continue;

      double ITR = rEV.GetClassifierResult("ITR:scintFitter").GetClassification("ITR");
      const TVector3 eventPosition = rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed
      double eventz = eventPosition.Z();

      const RAT::DS::FitVertex& rVertex = rEV.GetFitResult("scintFitter").GetVertex(0);

      h->Fill(eventz, ITR );
    
      h2->Fill(eventPosition.Mag(), rVertex.GetPosition().Mag() );
    }
  TCanvas* c1 = new TCanvas("c1", "c1", 800,600);
  h2->Draw("colz");
  
}
