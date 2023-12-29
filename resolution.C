
void resolution()
{

  // Setup histograms
  TH1D *h_x = new TH1D("h_x", "Event Distribution", 100, -1000, 1000);
  TH1D *h_y = new TH1D("h_y", "Event Distribution", 100, -1000, 1000);
  TH1D *h_z = new TH1D("h_z", "Event Distribution", 100, -1000, 1000);

  TH1D *h_x2 = new TH1D("h_x2", "Event Distribution", 100, -1000, 1000);
  TH1D *h_y2 = new TH1D("h_y2", "Event Distribution", 100, -1000, 1000);
  TH1D *h_z2 = new TH1D("h_z2", "Event Distribution", 100, -1000, 1000);
  
  TH1D *h_distance = new TH1D("h_distance", "Distance between Two Events", 200, 0, 1000);

  // Number of events we'll generate
  int num_events = pow(10,6);
  // Number of pairs of events we'll calculate the distance for
  int num_pairs = pow(10,5);

  TRandom3 *r1=new TRandom3();
  r1->SetSeed(102448);
  TRandom3 *r2=new TRandom3();
  r2->SetSeed(475653);
  // Simulated detector resolution
  double res = 100;
  double res2 = 130;

  std::vector<double> x1_vec;
  std::vector<double> y1_vec;
  std::vector<double> z1_vec;
  std::vector<double> x2_vec;
  std::vector<double> y2_vec;
  std::vector<double> z2_vec;

  // Generate some x values for our events
  for( int i_event = 0; i_event < num_events; i_event++)
    {
      // Draw from gaussian with detector resolution
      double eve_x = r1->Gaus(0, res);
      double eve_y = r1->Gaus(0, res);
      double eve_z = r1->Gaus(0, res);
      double eve_x2 = r2->Gaus(0, res2);
      double eve_y2 = r2->Gaus(0, res2);
      double eve_z2 = r2->Gaus(0, res2);

      // Push back in vectors
      x1_vec.push_back(eve_x);
      y1_vec.push_back(eve_y);
      z1_vec.push_back(eve_z);

      x2_vec.push_back(eve_x2);
      y2_vec.push_back(eve_y2);
      z2_vec.push_back(eve_z2);

      // Fill histograms of original data
      h_x->Fill(eve_x);
      h_y->Fill(eve_y);
      h_z->Fill(eve_z);

      h_x2->Fill(eve_x2);
      h_y2->Fill(eve_y2);
      h_z2->Fill(eve_z2);
    }

  // Loop over pairs
  for( int i_event = 0; i_event < num_pairs; i_event++)
    {
      // Draw two random integers from 0 to num_events
      int eve_num_1 = floor(num_events*r1->Uniform());
      //      int eve_num_2 = floor(num_events*r3->Uniform());

      // Select the two events and calc distance between them
      double x1 = x1_vec[eve_num_1];
      double x2 = x2_vec[eve_num_1];
      double y1 = y1_vec[eve_num_1];
      double y2 = y2_vec[eve_num_1];
      double z1 = z1_vec[eve_num_1];
      double z2 = z2_vec[eve_num_1];

      double distance = sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2)  );

      // Fill histogram
      h_distance->Fill(distance);
    }
 
  // Now make a couple of nice plots

  // First, distribution of events just to check it's Gaussian with right width
  TCanvas *eve_canv = new TCanvas("eve_canv", "Event Distribution", 800, 600);
  eve_canv->SetFrameLineWidth(2);
  eve_canv->SetGrid(1);
  eve_canv->SetLeftMargin(0.13);
  eve_canv->cd();

  h_x->SetLineWidth(2);
  h_x->SetLineStyle(2);
  h_x->GetXaxis()->SetTitle("Coordinate, mm");
  h_x->GetYaxis()->SetTitle("Number of Events");
  h_x->GetYaxis()->SetTitleOffset(1.5);
  h_x->Draw();
  h_y->SetLineWidth(2);
  h_y->SetLineStyle(2);
  h_y->SetLineColor(kRed);
  h_y->Draw("same");
  h_z->SetLineWidth(2);
  h_z->SetLineStyle(2);
  h_z->SetLineColor(kBlue);
  h_z->Draw("same");
  TLegend* leg = new TLegend(0.65, 0.6, 0.8, 0.72);
  leg->SetLineWidth(2);
  leg->AddEntry(h_x, "X", "l");
  leg->AddEntry(h_y, "Y", "l");
  leg->AddEntry(h_z, "Z", "l");
  leg->Draw();
  eve_canv->SaveAs("event_distribution1.pdf");

  // First, distribution of events just to check it's Gaussian with right width
  TCanvas *eve_canv2 = new TCanvas("eve_canv2", "Event Distribution", 800, 600);
  eve_canv2->SetFrameLineWidth(2);
  eve_canv2->SetGrid(1);
  eve_canv2->SetLeftMargin(0.13);
  eve_canv2->cd();

  h_x2->SetLineWidth(2);
  h_x2->SetLineStyle(2);
  h_x2->GetXaxis()->SetTitle("Coordinate, mm");
  h_x2->GetYaxis()->SetTitle("Number of Events");
  h_x2->GetYaxis()->SetTitleOffset(1.5);
  h_x2->Draw();
  h_y2->SetLineWidth(2);
  h_y2->SetLineStyle(2);
  h_y2->SetLineColor(kRed);
  h_y2->Draw("same");
  h_z2->SetLineWidth(2);
  h_z2->SetLineStyle(2);
  h_z2->SetLineColor(kBlue);
  h_z2->Draw("same");
  TLegend* leg2 = new TLegend(0.65, 0.6, 0.8, 0.72);
  leg2->SetLineWidth(2);
  leg2->AddEntry(h_x2, "X", "l");
  leg2->AddEntry(h_y2, "Y", "l");
  leg2->AddEntry(h_z2, "Z", "l");
  leg2->Draw();
  eve_canv2->SaveAs("event_distribution2.pdf");

  // Now the distance between pairs
  TCanvas *eve_distance = new TCanvas("eve_distance", "Distances between Pairs", 800, 600);
  eve_distance->SetFrameLineWidth(2);
  eve_distance->SetGrid(1);
  eve_distance->SetLeftMargin(0.13);
  eve_distance->cd();

  //Fit gaus to distance histogram
  TF1* gaus_func = new TF1("gaus_function", "gaus", 100,400);
  h_distance->Fit("gaus_function");

  h_distance->SetLineWidth(2);
  h_distance->GetXaxis()->SetTitle("Distance, mm");
  h_distance->GetYaxis()->SetTitle("Number of Pairs");
  h_distance->GetYaxis()->SetTitleOffset(1.5);
  gaus_func->SetLineColor(kRed);
  gaus_func->SetLineWidth(2);
  h_distance->Draw();

  TLegend* leg = new TLegend(0.4, 0.6, 0.88, 0.72);
  leg->SetLineWidth(2);
  TString label = Form("#mu = %.2f #pm %.2f; #sigma = %.2f #pm %.2f", gaus_func->GetParameter(1), gaus_func->GetParError(1), gaus_func->GetParameter(2), gaus_func->GetParError(2));
  leg->AddEntry(gaus_func, label, "l");
  leg->Draw();
  eve_distance->SaveAs("event_distance.pdf");
}
