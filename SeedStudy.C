#include <TH2D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>


void SeedStudy() {

  gStyle->SetOptStat(0);
  
  std::string canvname = "seedstudy_";
  
  // Read in file
  RAT::DU::DSReader dsreader0("/data/snoplus/parkerw/ratSimulations/Jun3_2p5_seed0_0_0/scintFitter_*.root");
  std::cout << "read 0" << std::endl;
  RAT::DU::DSReader dsreader100("/data/snoplus/parkerw/ratSimulations/Jun3_2p5_seed100_0_0/inputPerf_*.root");
  std::cout << "read 100" << std::endl;
  RAT::DU::DSReader dsreader250("/data/snoplus/parkerw/ratSimulations/Jun3_2p5_seed250_0_0/inputPerf_*.root");
  std::cout << "read 250" << std::endl;
  RAT::DU::DSReader dsreader500("/data/snoplus/parkerw/ratSimulations/Jun3_2p5_seed500_0_0/inputPerf_*.root");
  std::cout << "read 500" << std::endl;
  RAT::DU::DSReader dsreader1000("/data/snoplus/parkerw/ratSimulations/Jun3_2p5_seed1000_0_0/inputPerf_*.root");
  std::cout << "read 1000" << std::endl;
  RAT::DU::DSReader dsreader2000("/data/snoplus/parkerw/ratSimulations/Jun3_2p5_seed2000_0_0/inputPerf_*.root");
  std::cout << "read 2000" << std::endl;
  RAT::DU::DSReader dsreader3000("/data/snoplus/parkerw/ratSimulations/Jun3_2p5_seed3000_0_0/inputPerf_*.root");
  std::cout << "read 3000" << std::endl;
  RAT::DU::DSReader dsreader4000("/data/snoplus/parkerw/ratSimulations/Jun3_2p5_seed4000_0_0/inputPerf_*.root");
  std::cout << "read 4000" << std::endl;
  RAT::DU::DSReader dsreader5000("/data/snoplus/parkerw/ratSimulations/Jun3_2p5_seed5000_0_0/inputPerf_*.root");
  std::cout << "read 5000" << std::endl;

  TH1D* hx0 = new TH1D("hx0", "hx0", 100, -300, 300);
  TH1D* hx100 = new TH1D("hx100", "hx100", 100, -300, 300);
  TH1D* hx250 = new TH1D("hx250", "hx250", 100, -300, 300);
  TH1D* hx500 = new TH1D("hx500", "hx500", 100, -300, 300);
  TH1D* hx1000 = new TH1D("hx1000", "hx1000", 100, -300, 300);
  TH1D* hx2000 = new TH1D("hx2000", "hx2000", 100, -300, 300);
  TH1D* hx3000 = new TH1D("hx3000", "hx3000", 100, -300, 300);
  TH1D* hx4000 = new TH1D("hx4000", "hx4000", 100, -300, 300);
  TH1D* hx5000 = new TH1D("hx5000", "hx5000", 100, -300, 300);
  
  TH1D* hy0 = new TH1D("hy0", "hy0", 100, -300, 300);
  TH1D* hy100 = new TH1D("hy100", "hy100", 100, -300, 300);
  TH1D* hy250 = new TH1D("hy250", "hy250", 100, -300, 300);
  TH1D* hy500 = new TH1D("hy500", "hy500", 100, -300, 300);
  TH1D* hy1000 = new TH1D("hy1000", "hy1000", 100, -300, 300);
  TH1D* hy2000 = new TH1D("hy2000", "hy2000", 100, -300, 300);
  TH1D* hy3000 = new TH1D("hy3000", "hy3000", 100, -300, 300);
  TH1D* hy4000 = new TH1D("hy4000", "hy4000", 100, -300, 300);
  TH1D* hy5000 = new TH1D("hy5000", "hy5000", 100, -300, 300);

  TH1D* hz0 = new TH1D("hz0", "hz0", 100, -300, 300);
  TH1D* hz100 = new TH1D("hz100", "hz100", 100, -300, 300);
  TH1D* hz250 = new TH1D("hz250", "hz250", 100, -300, 300);
  TH1D* hz500 = new TH1D("hz500", "hz500", 100, -300, 300);
  TH1D* hz1000 = new TH1D("hz1000", "hz1000", 100, -300, 300);
  TH1D* hz2000 = new TH1D("hz2000", "hz2000", 100, -300, 300);
  TH1D* hz3000 = new TH1D("hz3000", "hz3000", 100, -300, 300);
  TH1D* hz4000 = new TH1D("hz4000", "hz4000", 100, -300, 300);
  TH1D* hz5000 = new TH1D("hz5000", "hz5000", 100, -300, 300);

  TH1D* hr0 = new TH1D("hr0", "hr0", 100, 0, 500);
  TH1D* hr100 = new TH1D("hr100", "hr100", 100, 0, 500);
  TH1D* hr250 = new TH1D("hr250", "hr250", 100, 0, 500);
  TH1D* hr500 = new TH1D("hr500", "hr500", 100, 0, 500);
  TH1D* hr1000 = new TH1D("hr1000", "hr1000", 100, 0, 500);
  TH1D* hr2000 = new TH1D("hr2000", "hr2000", 100, 0, 500);
  TH1D* hr3000 = new TH1D("hr3000", "hr3000", 100, 0, 500);
  TH1D* hr4000 = new TH1D("hr4000", "hr4000", 100, 0, 500);
  TH1D* hr5000 = new TH1D("hr5000", "hr5000", 100, 0, 500);

  
  for(int i=0; i<dsreader0.GetEntryCount();i++){
 
    const RAT::DS::Entry& rds0 = dsreader0.GetEntry(i);
    const RAT::DS::Entry& rds100 = dsreader100.GetEntry(i);
    const RAT::DS::Entry& rds250 = dsreader250.GetEntry(i);
    const RAT::DS::Entry& rds500 = dsreader500.GetEntry(i);
    const RAT::DS::Entry& rds1000 = dsreader1000.GetEntry(i);
    const RAT::DS::Entry& rds2000 = dsreader2000.GetEntry(i);
    const RAT::DS::Entry& rds3000 = dsreader3000.GetEntry(i);
    const RAT::DS::Entry& rds4000 = dsreader4000.GetEntry(i);
    const RAT::DS::Entry& rds5000 = dsreader5000.GetEntry(i);

    int nevC = rds0.GetEVCount();

    for(int iev=0;iev<1; iev++){

      const RAT::DS::EV& rev0 = rds0.GetEV(iev);
      const RAT::DS::EV& rev100 = rds100.GetEV(iev);
      const RAT::DS::EV& rev250 = rds250.GetEV(iev);
      const RAT::DS::EV& rev500 = rds500.GetEV(iev);
      const RAT::DS::EV& rev1000 = rds1000.GetEV(iev);
      const RAT::DS::EV& rev2000 = rds2000.GetEV(iev);
      const RAT::DS::EV& rev3000 = rds3000.GetEV(iev);
      const RAT::DS::EV& rev4000 = rds4000.GetEV(iev);
      const RAT::DS::EV& rev5000 = rds5000.GetEV(iev);

      RAT::DS::FitVertex fitVertex0;
      RAT::DS::FitVertex fitVertex100;
      RAT::DS::FitVertex fitVertex250;
      RAT::DS::FitVertex fitVertex500;
      RAT::DS::FitVertex fitVertex1000;
      RAT::DS::FitVertex fitVertex2000;
      RAT::DS::FitVertex fitVertex3000;
      RAT::DS::FitVertex fitVertex4000;
      RAT::DS::FitVertex fitVertex5000;
      
      try{
	fitVertex0 = rev0.GetFitResult( "scintFitter" ).GetVertex(0);
	fitVertex100 = rev100.GetFitResult( "scintFitter" ).GetVertex(0);
	fitVertex250 = rev250.GetFitResult( "scintFitter" ).GetVertex(0);
	fitVertex500 = rev500.GetFitResult( "scintFitter" ).GetVertex(0);
	fitVertex1000 = rev1000.GetFitResult( "scintFitter" ).GetVertex(0);
	fitVertex2000 = rev2000.GetFitResult( "scintFitter" ).GetVertex(0);
	fitVertex3000 = rev3000.GetFitResult( "scintFitter" ).GetVertex(0);
	fitVertex4000 = rev4000.GetFitResult( "scintFitter" ).GetVertex(0);
	fitVertex5000 = rev5000.GetFitResult( "scintFitter" ).GetVertex(0);

	if(!(fitVertex0.ValidPosition() && fitVertex0.ValidTime()))
                  continue;
	if(!(fitVertex100.ValidPosition() && fitVertex100.ValidTime()))
                  continue;
	if(!(fitVertex250.ValidPosition() && fitVertex250.ValidTime()))
                  continue;
	if(!(fitVertex500.ValidPosition() && fitVertex500.ValidTime()))
                  continue;
	if(!(fitVertex1000.ValidPosition() && fitVertex1000.ValidTime()))
                  continue;
	if(!(fitVertex2000.ValidPosition() && fitVertex2000.ValidTime()))
                  continue;
	if(!(fitVertex3000.ValidPosition() && fitVertex3000.ValidTime()))
                  continue;
	if(!(fitVertex4000.ValidPosition() && fitVertex4000.ValidTime()))
                  continue;
	if(!(fitVertex5000.ValidPosition() && fitVertex5000.ValidTime()))
                  continue;
      }
      catch(const RAT::DS::FitCollection::NoResultError&){
	continue;
      }


      hx0->Fill(fitVertex0.GetPosition().X());
      hx100->Fill(fitVertex100.GetPosition().X());
      hx250->Fill(fitVertex250.GetPosition().X());
      hx500->Fill(fitVertex500.GetPosition().X());
      hx1000->Fill(fitVertex1000.GetPosition().X());
      hx2000->Fill(fitVertex2000.GetPosition().X());
      hx3000->Fill(fitVertex3000.GetPosition().X());
      hx4000->Fill(fitVertex4000.GetPosition().X());
      hx5000->Fill(fitVertex5000.GetPosition().X());

      hy0->Fill(fitVertex0.GetPosition().Y());
      hy100->Fill(fitVertex100.GetPosition().Y());
      hy250->Fill(fitVertex250.GetPosition().Y());
      hy500->Fill(fitVertex500.GetPosition().Y());
      hy1000->Fill(fitVertex1000.GetPosition().Y());
      hy2000->Fill(fitVertex2000.GetPosition().Y());
      hy3000->Fill(fitVertex3000.GetPosition().Y());
      hy4000->Fill(fitVertex4000.GetPosition().Y());
      hy5000->Fill(fitVertex5000.GetPosition().Y());

      hz0->Fill(fitVertex0.GetPosition().Z());
      hz100->Fill(fitVertex100.GetPosition().Z());
      hz250->Fill(fitVertex250.GetPosition().Z());
      hz500->Fill(fitVertex500.GetPosition().Z());
      hz1000->Fill(fitVertex1000.GetPosition().Z());
      hz2000->Fill(fitVertex2000.GetPosition().Z());
      hz3000->Fill(fitVertex3000.GetPosition().Z());
      hz4000->Fill(fitVertex4000.GetPosition().Z());
      hz5000->Fill(fitVertex5000.GetPosition().Z());

      hr0->Fill(fitVertex0.GetPosition().Mag());
      hr100->Fill(fitVertex100.GetPosition().Mag());
      hr250->Fill(fitVertex250.GetPosition().Mag());
      hr500->Fill(fitVertex500.GetPosition().Mag());
      hr1000->Fill(fitVertex1000.GetPosition().Mag());
      hr2000->Fill(fitVertex2000.GetPosition().Mag());
      hr3000->Fill(fitVertex3000.GetPosition().Mag());
      hr4000->Fill(fitVertex4000.GetPosition().Mag());
      hr5000->Fill(fitVertex5000.GetPosition().Mag());

    }
  }

  TCanvas* cx = new TCanvas("cx", "cx", 1500, 800);

  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  hx0->SetTitle("");
  hx0->GetXaxis()->SetTitle("Fitted X, mm");
  hx0->GetYaxis()->SetTitle("Events");
  hx0->SetMaximum( 1.2*hx0->GetMaximum() );
  
  hx0->SetLineColor(kBlack);
  hx100->SetLineColor(kBlue+2);
  hx250->SetLineColor(kBlue);
  hx500->SetLineColor(kGreen);
  hx1000->SetLineColor(kYellow);
  hx2000->SetLineColor(kYellow+2);
  hx3000->SetLineColor(kPink);
  hx4000->SetLineColor(kRed);
  hx5000->SetLineColor(kRed+2);

  hx0->SetLineWidth(2);
  hx100->SetLineWidth(2);
  hx250->SetLineWidth(2);
  hx500->SetLineWidth(2);
  hx1000->SetLineWidth(2);
  hx2000->SetLineWidth(2);
  hx3000->SetLineWidth(2);
  hx4000->SetLineWidth(2);
  hx5000->SetLineWidth(2);

  hx0->SetLineStyle(1);
  hx100->SetLineStyle(2);
  hx250->SetLineStyle(3);
  hx500->SetLineStyle(4);
  hx1000->SetLineStyle(5);
  hx2000->SetLineStyle(6);
  hx3000->SetLineStyle(7);
  hx4000->SetLineStyle(8);
  hx5000->SetLineStyle(9);
  
  hx0->Draw();
  hx100->Draw("same");
  hx250->Draw("same");
  hx500->Draw("same");
  hx1000->Draw("same");
  hx2000->Draw("same");
  hx3000->Draw("same");
  hx4000->Draw("same");
  hx5000->Draw("same");

  TLegend* tx = new TLegend( 0.15, 0.55, 0.38, 0.85 );
  tx->SetLineWidth(2);
  tx->AddEntry( hx0, "Seed (0, 0, 0) mm", "l" );
  tx->AddEntry( hx100, "Seed (100, 0, 0) mm", "l" );
  tx->AddEntry( hx250, "Seed (250, 0, 0) mm", "l" );
  tx->AddEntry( hx500, "Seed (500, 0, 0) mm", "l" );
  tx->AddEntry( hx1000, "Seed (1000, 0, 0) mm", "l" );
  tx->AddEntry( hx2000, "Seed (2000, 0, 0) mm", "l" );
  tx->AddEntry( hx3000, "Seed (3000, 0, 0) mm", "l" );
  tx->AddEntry( hx4000, "Seed (4000, 0, 0) mm", "l" );
  tx->AddEntry( hx5000, "Seed (5000, 0, 0) mm", "l" );
  tx->Draw("same");

  cx->SaveAs( (canvname+"x.pdf").c_str() );

  
  TCanvas* cy = new TCanvas("cy", "cy", 1500, 800);

  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  hy0->SetTitle("");
  hy0->GetXaxis()->SetTitle("Fitted Y, mm");
  hy0->GetYaxis()->SetTitle("Events");
  hy0->SetMaximum( 1.2*hy0->GetMaximum() );

  hy0->SetLineColor(kBlack);
  hy100->SetLineColor(kBlue+2);
  hy250->SetLineColor(kBlue);
  hy500->SetLineColor(kGreen);
  hy1000->SetLineColor(kYellow);
  hy2000->SetLineColor(kYellow+2);
  hy3000->SetLineColor(kPink);
  hy4000->SetLineColor(kRed);
  hy5000->SetLineColor(kRed+2);

  hy0->SetLineWidth(2);
  hy100->SetLineWidth(2);
  hy250->SetLineWidth(2);
  hy500->SetLineWidth(2);
  hy1000->SetLineWidth(2);
  hy2000->SetLineWidth(2);
  hy3000->SetLineWidth(2);
  hy4000->SetLineWidth(2);
  hy5000->SetLineWidth(2);

  hy0->SetLineStyle(1);
  hy100->SetLineStyle(2);
  hy250->SetLineStyle(3);
  hy500->SetLineStyle(4);
  hy1000->SetLineStyle(5);
  hy2000->SetLineStyle(6);
  hy3000->SetLineStyle(7);
  hy4000->SetLineStyle(8);
  hy5000->SetLineStyle(9);

  hy0->Draw();
  hy100->Draw("same");
  hy250->Draw("same");
  hy500->Draw("same");
  hy1000->Draw("same");
  hy2000->Draw("same");
  hy3000->Draw("same");
  hy4000->Draw("same");
  hy5000->Draw("same");

  TLegend* ty = new TLegend( 0.15, 0.55, 0.38, 0.85 );
  ty->SetLineWidth(2);
  ty->AddEntry( hy0, "Seed (0, 0, 0) mm", "l" );
  ty->AddEntry( hy100, "Seed (100, 0, 0) mm", "l" );
  ty->AddEntry( hy250, "Seed (250, 0, 0) mm", "l" );
  ty->AddEntry( hy500, "Seed (500, 0, 0) mm", "l" );
  ty->AddEntry( hy1000, "Seed (1000, 0, 0) mm", "l" );
  ty->AddEntry( hy2000, "Seed (2000, 0, 0) mm", "l" );
  ty->AddEntry( hy3000, "Seed (3000, 0, 0) mm", "l" );
  ty->AddEntry( hy4000, "Seed (4000, 0, 0) mm", "l" );
  ty->AddEntry( hy5000, "Seed (5000, 0, 0) mm", "l" );
  ty->Draw("same");

  cy->SaveAs( (canvname+"y.pdf").c_str() );

  
  TCanvas* cz = new TCanvas("cz", "cz", 1500, 800);

  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  hz0->SetTitle("");
  hz0->GetXaxis()->SetTitle("Fitted Z, mm");
  hz0->GetYaxis()->SetTitle("Events");
  hz0->SetMaximum( 1.2*hz0->GetMaximum() );

  hz0->SetLineColor(kBlack);
  hz100->SetLineColor(kBlue+2);
  hz250->SetLineColor(kBlue);
  hz500->SetLineColor(kGreen);
  hz1000->SetLineColor(kYellow);
  hz2000->SetLineColor(kYellow+2);
  hz3000->SetLineColor(kPink);
  hz4000->SetLineColor(kRed);
  hz5000->SetLineColor(kRed+2);

  hz0->SetLineWidth(2);
  hz100->SetLineWidth(2);
  hz250->SetLineWidth(2);
  hz500->SetLineWidth(2);
  hz1000->SetLineWidth(2);
  hz2000->SetLineWidth(2);
  hz3000->SetLineWidth(2);
  hz4000->SetLineWidth(2);
  hz5000->SetLineWidth(2);

  hz0->SetLineStyle(1);
  hz100->SetLineStyle(2);
  hz250->SetLineStyle(3);
  hz500->SetLineStyle(4);
  hz1000->SetLineStyle(5);
  hz2000->SetLineStyle(6);
  hz3000->SetLineStyle(7);
  hz4000->SetLineStyle(8);
  hz5000->SetLineStyle(9);

  hz0->Draw();
  hz100->Draw("same");
  hz250->Draw("same");
  hz500->Draw("same");
  hz1000->Draw("same");
  hz2000->Draw("same");
  hz3000->Draw("same");
  hz4000->Draw("same");
  hz5000->Draw("same");

  TLegend* tz = new TLegend( 0.15, 0.55, 0.38, 0.85 );
  tz->SetLineWidth(2);
  tz->AddEntry( hz0, "Seed (0, 0, 0) mm", "l" );
  tz->AddEntry( hz100, "Seed (100, 0, 0) mm", "l" );
  tz->AddEntry( hz250, "Seed (250, 0, 0) mm", "l" );
  tz->AddEntry( hz500, "Seed (500, 0, 0) mm", "l" );
  tz->AddEntry( hz1000, "Seed (1000, 0, 0) mm", "l" );
  tz->AddEntry( hz2000, "Seed (2000, 0, 0) mm", "l" );
  tz->AddEntry( hz3000, "Seed (3000, 0, 0) mm", "l" );
  tz->AddEntry( hz4000, "Seed (4000, 0, 0) mm", "l" );
  tz->AddEntry( hz5000, "Seed (5000, 0, 0) mm", "l" );
  tz->Draw("same");

  cz->SaveAs( (canvname+"z.pdf").c_str() );


  TCanvas* cr = new TCanvas("cr", "cr", 1500, 800);

  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  hr0->SetTitle("");
  hr0->GetXaxis()->SetTitle("Fitted R, mm");
  hr0->GetYaxis()->SetTitle("Events");
  hr0->SetMaximum( 1.2*hr0->GetMaximum() );

  hr0->SetLineColor(kBlack);
  hr100->SetLineColor(kBlue+2);
  hr250->SetLineColor(kBlue);
  hr500->SetLineColor(kGreen);
  hr1000->SetLineColor(kYellow);
  hr2000->SetLineColor(kYellow+2);
  hr3000->SetLineColor(kPink);
  hr4000->SetLineColor(kRed);
  hr5000->SetLineColor(kRed+2);

  hr0->SetLineWidth(2);
  hr100->SetLineWidth(2);
  hr250->SetLineWidth(2);
  hr500->SetLineWidth(2);
  hr1000->SetLineWidth(2);
  hr2000->SetLineWidth(2);
  hr3000->SetLineWidth(2);
  hr4000->SetLineWidth(2);
  hr5000->SetLineWidth(2);

  hr0->SetLineStyle(1);
  hr100->SetLineStyle(2);
  hr250->SetLineStyle(3);
  hr500->SetLineStyle(4);
  hr1000->SetLineStyle(5);
  hr2000->SetLineStyle(6);
  hr3000->SetLineStyle(7);
  hr4000->SetLineStyle(8);
  hr5000->SetLineStyle(9);

  hr0->Draw();
  hr100->Draw("same");
  hr250->Draw("same");
  hr500->Draw("same");
  hr1000->Draw("same");
  hr2000->Draw("same");
  hr3000->Draw("same");
  hr4000->Draw("same");
  hr5000->Draw("same");

  TLegend* tr = new TLegend( 0.55, 0.55, 0.78, 0.85 );
  tr->SetLineWidth(2);
  tr->AddEntry( hx0, "Seed (0, 0, 0) mm", "l" );
  tr->AddEntry( hx100, "Seed (100, 0, 0) mm", "l" );
  tr->AddEntry( hx250, "Seed (250, 0, 0) mm", "l" );
  tr->AddEntry( hx500, "Seed (500, 0, 0) mm", "l" );
  tr->AddEntry( hx1000, "Seed (1000, 0, 0) mm", "l" );
  tr->AddEntry( hx2000, "Seed (2000, 0, 0) mm", "l" );
  tr->AddEntry( hx3000, "Seed (3000, 0, 0) mm", "l" );
  tr->AddEntry( hx4000, "Seed (4000, 0, 0) mm", "l" );
  tr->AddEntry( hx5000, "Seed (5000, 0, 0) mm", "l" );
  tr->Draw("same");

  cr->SaveAs( (canvname+"r.pdf").c_str() );
  
}
