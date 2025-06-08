#include <TH2D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>


void SeedStudy500() {

  gStyle->SetOptStat(0);
  
  std::string canvname = "seedstudy_";
  
  // Read in file
  RAT::DU::DSReader dsreader5500("/data/snoplus/parkerw/ratSimulations/Jun3_2p5_5000seed5500_0_0/inputPerf_*.root");
  std::cout << "read 5500" << std::endl;
  RAT::DU::DSReader dsreader5000("/data/snoplus/parkerw/ratSimulations/Jun3_2p5_5000seed5000_0_0/scintFitter_*.root");
  std::cout << "read 5000" << std::endl;
  RAT::DU::DSReader dsreader4500("/data/snoplus/parkerw/ratSimulations/Jun3_2p5_5000seed4500_0_0/inputPerf_*.root");
  std::cout << "read 4500" << std::endl;
  RAT::DU::DSReader dsreader4000("/data/snoplus/parkerw/ratSimulations/Jun3_2p5_5000seed4000_0_0/inputPerf_*.root");
  std::cout << "read 4000" << std::endl;
  RAT::DU::DSReader dsreader3500("/data/snoplus/parkerw/ratSimulations/Jun3_2p5_5000seed3500_0_0/inputPerf_*.root");
  std::cout << "read 3500" << std::endl;
  RAT::DU::DSReader dsreader3000("/data/snoplus/parkerw/ratSimulations/Jun3_2p5_5000seed3000_0_0/inputPerf_*.root");
  std::cout << "read 3000" << std::endl;
  RAT::DU::DSReader dsreader2000("/data/snoplus/parkerw/ratSimulations/Jun3_2p5_5000seed2000_0_0/inputPerf_*.root");
  std::cout << "read 2000" << std::endl;
  RAT::DU::DSReader dsreader1000("/data/snoplus/parkerw/ratSimulations/Jun3_2p5_5000seed1000_0_0/inputPerf_*.root");
  std::cout << "read 1000" << std::endl;
  RAT::DU::DSReader dsreader0("/data/snoplus/parkerw/ratSimulations/Jun3_2p5_5000seed0_0_0/inputPerf_*.root");
  std::cout << "read 0" << std::endl;


  TH1D* hx5500range = new TH1D("hx5500range", "hx5500range", 1000, 0, 20000);
  TH1D* hx5500 = new TH1D("hx5500", "hx5500", 100, 4500, 5500);
  TH1D* hx5000 = new TH1D("hx5000", "hx5000", 100, 4500, 5500);
  TH1D* hx4500 = new TH1D("hx4500", "hx4500", 100, 4500, 5500);
  TH1D* hx4000 = new TH1D("hx4000", "hx4000", 100, 4500, 5500);
  TH1D* hx3500 = new TH1D("hx3500", "hx3500", 100, 4500, 5500);
  TH1D* hx3000 = new TH1D("hx3000", "hx3000", 100, 4500, 5500);
  TH1D* hx2000 = new TH1D("hx2000", "hx2000", 100, 4500, 5500);
  TH1D* hx1000 = new TH1D("hx1000", "hx1000", 100, 4500, 5500);
  TH1D* hx0 = new TH1D("hx0", "hx0", 100, 4500, 5500);
  
  TH1D* hy5500 = new TH1D("hy5500", "hy5500", 100, -300, 300);
  TH1D* hy5000 = new TH1D("hy5000", "hy5000", 100, -300, 300);
  TH1D* hy4500 = new TH1D("hy4500", "hy4500", 100, -300, 300);
  TH1D* hy4000 = new TH1D("hy4000", "hy4000", 100, -300, 300);
  TH1D* hy3500 = new TH1D("hy3500", "hy3500", 100, -300, 300);
  TH1D* hy3000 = new TH1D("hy3000", "hy3000", 100, -300, 300);
  TH1D* hy2000 = new TH1D("hy2000", "hy2000", 100, -300, 300);
  TH1D* hy1000 = new TH1D("hy1000", "hy1000", 100, -300, 300);
  TH1D* hy0 = new TH1D("hy0", "hy0", 100, -300, 300);

  TH1D* hz5500 = new TH1D("hz5500", "hz5500", 100, -300, 300);
  TH1D* hz5000 = new TH1D("hz5000", "hz5000", 100, -300, 300);
  TH1D* hz4500 = new TH1D("hz4500", "hz4500", 100, -300, 300);
  TH1D* hz4000 = new TH1D("hz4000", "hz4000", 100, -300, 300);
  TH1D* hz3500 = new TH1D("hz3500", "hz3500", 100, -300, 300);
  TH1D* hz3000 = new TH1D("hz3000", "hz3000", 100, -300, 300);
  TH1D* hz2000 = new TH1D("hz2000", "hz2000", 100, -300, 300);
  TH1D* hz1000 = new TH1D("hz1000", "hz1000", 100, -300, 300);
  TH1D* hz0 = new TH1D("hz0", "hz0", 100, -300, 300);

  TH1D* hr5500 = new TH1D("hr5500", "hr5500", 100, 4500, 5500);
  TH1D* hr5000 = new TH1D("hr5000", "hr5000", 100, 4500, 5500);
  TH1D* hr4500 = new TH1D("hr4500", "hr4500", 100, 4500, 5500);
  TH1D* hr4000 = new TH1D("hr4000", "hr4000", 100, 4500, 5500);
  TH1D* hr3500 = new TH1D("hr3500", "hr3500", 100, 4500, 5500);
  TH1D* hr3000 = new TH1D("hr3000", "hr3000", 100, 4500, 5500);
  TH1D* hr2000 = new TH1D("hr2000", "hr2000", 100, 4500, 5500);
  TH1D* hr1000 = new TH1D("hr1000", "hr1000", 100, 4500, 5500);
  TH1D* hr0 = new TH1D("hr0", "hr0", 100, 4500, 5500);

  
  for(int i=0; i<dsreader0.GetEntryCount();i++){
 
    const RAT::DS::Entry& rds5500 = dsreader5500.GetEntry(i);
    const RAT::DS::Entry& rds5000 = dsreader5000.GetEntry(i);
    const RAT::DS::Entry& rds4500 = dsreader4500.GetEntry(i);
    const RAT::DS::Entry& rds4000 = dsreader4000.GetEntry(i);
    const RAT::DS::Entry& rds3500 = dsreader3500.GetEntry(i);
    const RAT::DS::Entry& rds3000 = dsreader3000.GetEntry(i);
    const RAT::DS::Entry& rds2000 = dsreader2000.GetEntry(i);
    const RAT::DS::Entry& rds1000 = dsreader1000.GetEntry(i);
    const RAT::DS::Entry& rds0 = dsreader0.GetEntry(i);

    int nevC = rds0.GetEVCount();

    for(int iev=0;iev<1; iev++){

      const RAT::DS::EV& rev5500 = rds5500.GetEV(iev);
      const RAT::DS::EV& rev5000 = rds5000.GetEV(iev);
      const RAT::DS::EV& rev4500 = rds4500.GetEV(iev);
      const RAT::DS::EV& rev4000 = rds4000.GetEV(iev);
      const RAT::DS::EV& rev3500 = rds3500.GetEV(iev);
      const RAT::DS::EV& rev3000 = rds3000.GetEV(iev);
      const RAT::DS::EV& rev2000 = rds2000.GetEV(iev);
      const RAT::DS::EV& rev1000 = rds1000.GetEV(iev);
      const RAT::DS::EV& rev0 = rds0.GetEV(iev);

      RAT::DS::FitVertex fitVertex5500;
      RAT::DS::FitVertex fitVertex5000;
      RAT::DS::FitVertex fitVertex4500;
      RAT::DS::FitVertex fitVertex4000;
      RAT::DS::FitVertex fitVertex3500;
      RAT::DS::FitVertex fitVertex3000;
      RAT::DS::FitVertex fitVertex2000;
      RAT::DS::FitVertex fitVertex1000;
      RAT::DS::FitVertex fitVertex0;
      
      try{
	fitVertex5500 = rev5500.GetFitResult( "scintFitter" ).GetVertex(0);
	fitVertex5000 = rev5000.GetFitResult( "scintFitter" ).GetVertex(0);
	fitVertex4500 = rev4500.GetFitResult( "scintFitter" ).GetVertex(0);
	fitVertex4000 = rev4000.GetFitResult( "scintFitter" ).GetVertex(0);
	fitVertex3500 = rev3500.GetFitResult( "scintFitter" ).GetVertex(0);
	fitVertex3000 = rev3000.GetFitResult( "scintFitter" ).GetVertex(0);
	fitVertex2000 = rev2000.GetFitResult( "scintFitter" ).GetVertex(0);
	fitVertex1000 = rev1000.GetFitResult( "scintFitter" ).GetVertex(0);
	fitVertex0 = rev0.GetFitResult( "scintFitter" ).GetVertex(0);

	if(!(fitVertex5500.ValidPosition() && fitVertex5500.ValidTime()))
                  continue;
	if(!(fitVertex5000.ValidPosition() && fitVertex5000.ValidTime()))
                  continue;
	if(!(fitVertex4500.ValidPosition() && fitVertex4500.ValidTime()))
                  continue;
	if(!(fitVertex4000.ValidPosition() && fitVertex4000.ValidTime()))
                  continue;
	if(!(fitVertex3500.ValidPosition() && fitVertex3500.ValidTime()))
                  continue;
	if(!(fitVertex3000.ValidPosition() && fitVertex3000.ValidTime()))
                  continue;
	if(!(fitVertex2000.ValidPosition() && fitVertex2000.ValidTime()))
                  continue;
	if(!(fitVertex1000.ValidPosition() && fitVertex1000.ValidTime()))
                  continue;
	if(!(fitVertex0.ValidPosition() && fitVertex0.ValidTime()))
                  continue;
      }
      catch(const RAT::DS::FitCollection::NoResultError&){
	continue;
      }

      hx5500range->Fill(fitVertex5500.GetPosition().X());
      
      hx5500->Fill(fitVertex5500.GetPosition().X());
      hx5000->Fill(fitVertex5000.GetPosition().X());
      hx4500->Fill(fitVertex4500.GetPosition().X());
      hx4000->Fill(fitVertex4000.GetPosition().X());
      hx3500->Fill(fitVertex3500.GetPosition().X());
      hx3000->Fill(fitVertex3000.GetPosition().X());
      hx2000->Fill(fitVertex2000.GetPosition().X());
      hx1000->Fill(fitVertex1000.GetPosition().X());
      hx0->Fill(fitVertex0.GetPosition().X());

      hy5500->Fill(fitVertex5500.GetPosition().Y());
      hy5000->Fill(fitVertex5000.GetPosition().Y());
      hy4500->Fill(fitVertex4500.GetPosition().Y());
      hy4000->Fill(fitVertex4000.GetPosition().Y());
      hy3500->Fill(fitVertex3500.GetPosition().Y());
      hy3000->Fill(fitVertex3000.GetPosition().Y());
      hy2000->Fill(fitVertex2000.GetPosition().Y());
      hy1000->Fill(fitVertex1000.GetPosition().Y());
      hy0->Fill(fitVertex0.GetPosition().Y());

      hz5500->Fill(fitVertex5500.GetPosition().Z());
      hz5000->Fill(fitVertex5000.GetPosition().Z());
      hz4500->Fill(fitVertex4500.GetPosition().Z());
      hz4000->Fill(fitVertex4000.GetPosition().Z());
      hz3500->Fill(fitVertex3500.GetPosition().Z());
      hz3000->Fill(fitVertex3000.GetPosition().Z());
      hz2000->Fill(fitVertex2000.GetPosition().Z());
      hz1000->Fill(fitVertex1000.GetPosition().Z());
      hz0->Fill(fitVertex0.GetPosition().Z());

      hr5500->Fill(fitVertex5500.GetPosition().Mag());
      hr5000->Fill(fitVertex5000.GetPosition().Mag());
      hr4500->Fill(fitVertex4500.GetPosition().Mag());
      hr4000->Fill(fitVertex4000.GetPosition().Mag());
      hr3500->Fill(fitVertex3500.GetPosition().Mag());
      hr3000->Fill(fitVertex3000.GetPosition().Mag());
      hr2000->Fill(fitVertex2000.GetPosition().Mag());
      hr1000->Fill(fitVertex1000.GetPosition().Mag());
      hr0->Fill(fitVertex0.GetPosition().Mag());

    }
  }

  TCanvas* cxrange = new TCanvas("cxrange", "cxrange", 1500, 800);

  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);
  gPad->SetLogy();
  
  hx5500range->SetTitle("");
  hx5500range->GetXaxis()->SetTitle("Fitted X, mm");
  hx5500range->GetYaxis()->SetTitle("Events");
  hx5500range->SetLineColor(kBlack);
  hx5500range->SetLineWidth(2);
  hx5500range->Draw();

  cxrange->SaveAs( (canvname+"xrange5000.pdf").c_str() );
  
  TCanvas* cx = new TCanvas("cx", "cx", 1500, 800);

  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  hx5500->SetTitle("");
  hx5500->GetXaxis()->SetTitle("Fitted X, mm");
  hx5500->GetYaxis()->SetTitle("Events");
  hx5500->SetMaximum( 1.2*hx5000->GetMaximum() );
  
  hx5500->SetLineColor(kBlack);
  hx5000->SetLineColor(kBlue+2);
  hx4500->SetLineColor(kBlue);
  hx4000->SetLineColor(kGreen);
  hx3500->SetLineColor(kYellow);
  hx3000->SetLineColor(kYellow+2);
  hx2000->SetLineColor(kPink);
  hx1000->SetLineColor(kRed);
  hx0->SetLineColor(kRed+2);

  hx5500->SetLineWidth(2);
  hx5000->SetLineWidth(2);
  hx4500->SetLineWidth(2);
  hx4000->SetLineWidth(2);
  hx3500->SetLineWidth(2);
  hx3000->SetLineWidth(2);
  hx2000->SetLineWidth(2);
  hx1000->SetLineWidth(2);
  hx0->SetLineWidth(2);

  hx5500->SetLineStyle(1);
  hx5000->SetLineStyle(2);
  hx4500->SetLineStyle(3);
  hx4000->SetLineStyle(4);
  hx3500->SetLineStyle(5);
  hx3000->SetLineStyle(6);
  hx2000->SetLineStyle(7);
  hx1000->SetLineStyle(8);
  hx0->SetLineStyle(9);
  
  hx5500->Draw();
  hx5000->Draw("same");
  hx4500->Draw("same");
  hx4000->Draw("same");
  hx3500->Draw("same");
  hx3000->Draw("same");
  hx2000->Draw("same");
  hx1000->Draw("same");
  hx0->Draw("same");

  TLegend* tx = new TLegend( 0.15, 0.55, 0.38, 0.85 );
  tx->SetLineWidth(2);
  tx->AddEntry( hx5500, "Seed (5500, 0, 0) mm", "l" );
  tx->AddEntry( hx5000, "Seed (5000, 0, 0) mm", "l" );
  tx->AddEntry( hx4500, "Seed (4500, 0, 0) mm", "l" );
  tx->AddEntry( hx4000, "Seed (4000, 0, 0) mm", "l" );
  tx->AddEntry( hx3500, "Seed (3500, 0, 0) mm", "l" );
  tx->AddEntry( hx3000, "Seed (3000, 0, 0) mm", "l" );
  tx->AddEntry( hx2000, "Seed (2000, 0, 0) mm", "l" );
  tx->AddEntry( hx1000, "Seed (1000, 0, 0) mm", "l" );
  tx->AddEntry( hx0, "Seed (0, 0, 0) mm", "l" );
  tx->Draw("same");

  cx->SaveAs( (canvname+"x5000.pdf").c_str() );

  
  TCanvas* cy = new TCanvas("cy", "cy", 1500, 800);

  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  hy5500->SetTitle("");
  hy5500->GetXaxis()->SetTitle("Fitted Y, mm");
  hy5500->GetYaxis()->SetTitle("Events");
  hy5500->SetMaximum( 1.2*hy5000->GetMaximum() );

  hy5500->SetLineColor(kBlack);
  hy5000->SetLineColor(kBlue+2);
  hy4500->SetLineColor(kBlue);
  hy4000->SetLineColor(kGreen);
  hy3500->SetLineColor(kYellow);
  hy3000->SetLineColor(kYellow+2);
  hy2000->SetLineColor(kPink);
  hy1000->SetLineColor(kRed);
  hy0->SetLineColor(kRed+2);

  hy5500->SetLineWidth(2);
  hy5000->SetLineWidth(2);
  hy4500->SetLineWidth(2);
  hy4000->SetLineWidth(2);
  hy3500->SetLineWidth(2);
  hy3000->SetLineWidth(2);
  hy2000->SetLineWidth(2);
  hy1000->SetLineWidth(2);
  hy0->SetLineWidth(2);

  hy5500->SetLineStyle(1);
  hy5000->SetLineStyle(2);
  hy4500->SetLineStyle(3);
  hy4000->SetLineStyle(4);
  hy3500->SetLineStyle(5);
  hy3000->SetLineStyle(6);
  hy2000->SetLineStyle(7);
  hy1000->SetLineStyle(8);
  hy0->SetLineStyle(9);

  hy5500->Draw();
  hy5000->Draw("same");
  hy4500->Draw("same");
  hy4000->Draw("same");
  hy3500->Draw("same");
  hy3000->Draw("same");
  hy2000->Draw("same");
  hy1000->Draw("same");
  hy0->Draw("same");

  std::cout << "5500: " << hx5500->Integral() << std::endl;
  std::cout << "5000: "	<< hx5000->Integral() << std::endl;
  std::cout << "4500: "	<< hx4500->Integral() << std::endl;
  std::cout << "4000: "	<< hx4000->Integral() << std::endl;
  std::cout << "3500: "	<< hx3500->Integral() << std::endl;
  std::cout << "3000: "	<< hx3000->Integral() << std::endl;
  std::cout << "2000: "	<< hx2000->Integral() << std::endl;
  std::cout << "1000: "	<< hx1000->Integral() << std::endl;
  std::cout << "   0: "	<< hx0->Integral() << std::endl;

  TLegend* ty = new TLegend( 0.15, 0.55, 0.38, 0.85 );
  ty->SetLineWidth(2);
  ty->AddEntry( hy5500, "Seed (5500, 0, 0) mm", "l" );
  ty->AddEntry( hy5000, "Seed (5000, 0, 0) mm", "l" );
  ty->AddEntry( hy4500, "Seed (4500, 0, 0) mm", "l" );
  ty->AddEntry( hy4000, "Seed (4000, 0, 0) mm", "l" );
  ty->AddEntry( hy3500, "Seed (3500, 0, 0) mm", "l" );
  ty->AddEntry( hy3000, "Seed (3000, 0, 0) mm", "l" );
  ty->AddEntry( hy2000, "Seed (2000, 0, 0) mm", "l" );
  ty->AddEntry( hy1000, "Seed (1000, 0, 0) mm", "l" );
  ty->AddEntry( hy0, "Seed (0, 0, 0) mm", "l" );
  ty->Draw("same");

  cy->SaveAs( (canvname+"y5000.pdf").c_str() );

  
  TCanvas* cz = new TCanvas("cz", "cz", 1500, 800);

  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  hz5500->SetTitle("");
  hz5500->GetXaxis()->SetTitle("Fitted Z, mm");
  hz5500->GetYaxis()->SetTitle("Events");
  hz5500->SetMaximum( 1.2*hz5000->GetMaximum() );

  hz5500->SetLineColor(kBlack);
  hz5000->SetLineColor(kBlue+2);
  hz4500->SetLineColor(kBlue);
  hz4000->SetLineColor(kGreen);
  hz3500->SetLineColor(kYellow);
  hz3000->SetLineColor(kYellow+2);
  hz2000->SetLineColor(kPink);
  hz1000->SetLineColor(kRed);
  hz0->SetLineColor(kRed+2);

  hz5500->SetLineWidth(2);
  hz5000->SetLineWidth(2);
  hz4500->SetLineWidth(2);
  hz4000->SetLineWidth(2);
  hz3500->SetLineWidth(2);
  hz3000->SetLineWidth(2);
  hz2000->SetLineWidth(2);
  hz1000->SetLineWidth(2);
  hz0->SetLineWidth(2);

  hz5500->SetLineStyle(1);
  hz5000->SetLineStyle(2);
  hz4500->SetLineStyle(3);
  hz4000->SetLineStyle(4);
  hz3500->SetLineStyle(5);
  hz3000->SetLineStyle(6);
  hz2000->SetLineStyle(7);
  hz1000->SetLineStyle(8);
  hz0->SetLineStyle(9);

  hz5500->Draw();
  hz5000->Draw("same");
  hz4500->Draw("same");
  hz4000->Draw("same");
  hz3500->Draw("same");
  hz3000->Draw("same");
  hz2000->Draw("same");
  hz1000->Draw("same");
  hz0->Draw("same");

  TLegend* tz = new TLegend( 0.15, 0.55, 0.38, 0.85 );
  tz->SetLineWidth(2);
  tz->AddEntry( hz5500, "Seed (5500, 0, 0) mm", "l" );
  tz->AddEntry( hz5000, "Seed (5000, 0, 0) mm", "l" );
  tz->AddEntry( hz4500, "Seed (4500, 0, 0) mm", "l" );
  tz->AddEntry( hz4000, "Seed (4000, 0, 0) mm", "l" );
  tz->AddEntry( hz3500, "Seed (3500, 0, 0) mm", "l" );
  tz->AddEntry( hz3000, "Seed (3000, 0, 0) mm", "l" );
  tz->AddEntry( hz2000, "Seed (2000, 0, 0) mm", "l" );
  tz->AddEntry( hz1000, "Seed (1000, 0, 0) mm", "l" );
  tz->AddEntry( hz0, "Seed (0, 0, 0) mm", "l" );
  tz->Draw("same");

  cz->SaveAs( (canvname+"z5000.pdf").c_str() );


  TCanvas* cr = new TCanvas("cr", "cr", 1500, 800);

  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  hr5500->SetTitle("");
  hr5500->GetXaxis()->SetTitle("Fitted R, mm");
  hr5500->GetYaxis()->SetTitle("Events");
  hr5500->SetMaximum( 1.2*hr5000->GetMaximum() );

  hr5500->SetLineColor(kBlack);
  hr5000->SetLineColor(kBlue+2);
  hr4500->SetLineColor(kBlue);
  hr4000->SetLineColor(kGreen);
  hr3500->SetLineColor(kYellow);
  hr3000->SetLineColor(kYellow+2);
  hr2000->SetLineColor(kPink);
  hr1000->SetLineColor(kRed);
  hr0->SetLineColor(kRed+2);

  hr5500->SetLineWidth(2);
  hr5000->SetLineWidth(2);
  hr4500->SetLineWidth(2);
  hr4000->SetLineWidth(2);
  hr3500->SetLineWidth(2);
  hr3000->SetLineWidth(2);
  hr2000->SetLineWidth(2);
  hr1000->SetLineWidth(2);
  hr0->SetLineWidth(2);

  hr5500->SetLineStyle(1);
  hr5000->SetLineStyle(2);
  hr4500->SetLineStyle(3);
  hr4000->SetLineStyle(4);
  hr3500->SetLineStyle(5);
  hr3000->SetLineStyle(6);
  hr2000->SetLineStyle(7);
  hr1000->SetLineStyle(8);
  hr0->SetLineStyle(9);

  hr5500->Draw();
  hr5000->Draw("same");
  hr4500->Draw("same");
  hr4000->Draw("same");
  hr3500->Draw("same");
  hr3000->Draw("same");
  hr2000->Draw("same");
  hr1000->Draw("same");
  hr0->Draw("same");

  TLegend* tr = new TLegend( 0.58, 0.55, 0.81, 0.85 );
  tr->SetLineWidth(2);
  tr->AddEntry( hx5500, "Seed (5500, 0, 0) mm", "l" );
  tr->AddEntry( hx5000, "Seed (5000, 0, 0) mm", "l" );
  tr->AddEntry( hx4500, "Seed (4500, 0, 0) mm", "l" );
  tr->AddEntry( hx4000, "Seed (4000, 0, 0) mm", "l" );
  tr->AddEntry( hx3500, "Seed (3500, 0, 0) mm", "l" );
  tr->AddEntry( hx3000, "Seed (3000, 0, 0) mm", "l" );
  tr->AddEntry( hx2000, "Seed (2000, 0, 0) mm", "l" );
  tr->AddEntry( hx1000, "Seed (1000, 0, 0) mm", "l" );
  tr->AddEntry( hx0, "Seed (0, 0, 0) mm", "l" );
  tr->Draw("same");

  cr->SaveAs( (canvname+"r5000.pdf").c_str() );
  
}
