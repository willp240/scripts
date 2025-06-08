
void ReadFile(std::string filename, TH1D* h)
{
  
  std::ifstream infile(filename);
  if (!infile.is_open()) {
    std::cerr << "Error opening file: " << filename << std::endl;
    return;
  }

  std::string line;
  const std::string marker = "number of powell iterations:";
  while (std::getline(infile, line)) {
    std::size_t pos = line.find(marker);
    if (pos != std::string::npos) {
      std::istringstream iss(line.substr(pos + marker.length()));
      int iter;
      if (iss >> iter) {
	h->Fill(iter);
      }
    }
  }

  infile.close();

}


void PowellIterations() {

  gStyle->SetOptStat(0);
  
  std::string filename0 = "/data/snoplus/parkerw/ratSimulations/Jun3_2p5_seed0_0_0/output/output.txt";
  std::string filename100 = "/data/snoplus/parkerw/ratSimulations/Jun3_2p5_seed100_0_0/output/output.txt";
  std::string filename250 = "/data/snoplus/parkerw/ratSimulations/Jun3_2p5_seed250_0_0/output/output.txt";
  std::string filename500 = "/data/snoplus/parkerw/ratSimulations/Jun3_2p5_seed500_0_0/output/output.txt";
  std::string filename1000 = "/data/snoplus/parkerw/ratSimulations/Jun3_2p5_seed1000_0_0/output/output.txt";
  std::string filename2000 = "/data/snoplus/parkerw/ratSimulations/Jun3_2p5_seed2000_0_0/output/output.txt";
  std::string filename3000 = "/data/snoplus/parkerw/ratSimulations/Jun3_2p5_seed3000_0_0/output/output.txt";
  std::string filename4000 = "/data/snoplus/parkerw/ratSimulations/Jun3_2p5_seed4000_0_0/output/output.txt";
  std::string filename5000 = "/data/snoplus/parkerw/ratSimulations/Jun3_2p5_seed5000_0_0/output/output.txt";

  TH1D* hIterations0 = new TH1D("hIterations0", "Powell Iteration Count;Iterations;Frequency", 25, 0, 25);
  TH1D* hIterations100 = new TH1D("hIterations100", "Powell Iteration Count;Iterations;Frequency", 25, 0, 25);
  TH1D* hIterations250 = new TH1D("hIterations250", "Powell Iteration Count;Iterations;Frequency", 25, 0, 25);
  TH1D* hIterations500 = new TH1D("hIterations500", "Powell Iteration Count;Iterations;Frequency", 25, 0, 25);
  TH1D* hIterations1000 = new TH1D("hIterations1000", "Powell Iteration Count;Iterations;Frequency", 25, 0, 25);
  TH1D* hIterations2000 = new TH1D("hIterations2000", "Powell Iteration Count;Iterations;Frequency", 25, 0, 25);
  TH1D* hIterations3000 = new TH1D("hIterations3000", "Powell Iteration Count;Iterations;Frequency", 25, 0, 25);
  TH1D* hIterations4000 = new TH1D("hIterations4000", "Powell Iteration Count;Iterations;Frequency", 25, 0, 25);
  TH1D* hIterations5000 = new TH1D("hIterations5000", "Powell Iteration Count;Iterations;Frequency", 25, 0, 25);
  

  ReadFile( filename0, hIterations0);
  ReadFile( filename100, hIterations100);
  ReadFile( filename250, hIterations250);
  ReadFile( filename500, hIterations500);
  ReadFile( filename1000, hIterations1000);
  ReadFile( filename2000, hIterations2000);
  ReadFile( filename3000, hIterations3000);
  ReadFile( filename4000, hIterations4000);
  ReadFile( filename5000, hIterations5000);

  hIterations0->SetLineWidth(2);
  hIterations100->SetLineWidth(2);
  hIterations250->SetLineWidth(2);
  hIterations500->SetLineWidth(2);
  hIterations1000->SetLineWidth(2);
  hIterations2000->SetLineWidth(2);
  hIterations3000->SetLineWidth(2);
  hIterations4000->SetLineWidth(2);
  hIterations5000->SetLineWidth(2);

  hIterations0->SetLineColor(kBlack);
  hIterations100->SetLineColor(kBlue+2);
  hIterations250->SetLineColor(kBlue);
  hIterations500->SetLineColor(kGreen);
  hIterations1000->SetLineColor(kYellow);
  hIterations2000->SetLineColor(kYellow+2);
  hIterations3000->SetLineColor(kPink);
  hIterations4000->SetLineColor(kRed);
  hIterations5000->SetLineColor(kRed+2);

  // Draw and save the histogram
  TCanvas* c1 = new TCanvas("c1", "Powell Iteration Histogram", 800, 600);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  hIterations0->GetYaxis()->SetTitleOffset(1.2);
  hIterations0->Draw();
  hIterations100->Draw("same");
  hIterations250->Draw("same");
  hIterations500->Draw("same");
  hIterations1000->Draw("same");
  hIterations2000->Draw("same");
  hIterations3000->Draw("same");
  hIterations4000->Draw("same");
  hIterations5000->Draw("same");

  TLegend* ti = new TLegend( 0.55, 0.55, 0.78, 0.85 );
  ti->SetLineWidth(2);
  ti->AddEntry( hIterations0, "Seed (0, 0, 0) mm", "l" );
  ti->AddEntry( hIterations100, "Seed (100, 0, 0) mm", "l" );
  ti->AddEntry( hIterations250, "Seed (250, 0, 0) mm", "l" );
  ti->AddEntry( hIterations500, "Seed (500, 0, 0) mm", "l" );
  ti->AddEntry( hIterations1000, "Seed (1000, 0, 0) mm", "l" );
  ti->AddEntry( hIterations2000, "Seed (2000, 0, 0) mm", "l" );
  ti->AddEntry( hIterations3000, "Seed (3000, 0, 0) mm", "l" );
  ti->AddEntry( hIterations4000, "Seed (4000, 0, 0) mm", "l" );
  ti->AddEntry( hIterations5000, "Seed (5000, 0, 0) mm", "l" );
  ti->Draw("same");

  
  c1->SaveAs("powell_iterations_hist.pdf");

}
