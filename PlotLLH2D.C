

void PlotLLH2D()
{


  TFile* file = TFile::Open("llh.root");

  TH2D* LLHDiff = (TH2D*)file->Get("hLLHDiff");

  TCanvas* c1 = new TCanvas("c1", "Delta LLH", 1200, 800);
  gPad->SetGrid();
  gPad->SetFrameLineWidth(2);
  gPad->SetLogz();
  c1->SetRightMargin(0.13);
  c1->SetLeftMargin(0.13);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(53);

  LLHDiff->SetMaximum(0.01);
  LLHDiff->SetMinimum(0.00000001);
  LLHDiff->SetTitle("Fractional Difference to Maximum LLH");
  LLHDiff->GetXaxis()->SetTitle("X, mm");
  LLHDiff->GetYaxis()->SetTitle("Y, mm");
  LLHDiff->GetXaxis()->SetRangeUser(-500,500);
  LLHDiff->GetYaxis()->SetRangeUser(-500,500);
  LLHDiff->Draw("colz");

  c1->SaveAs("fractionalLLH.pdf");
  
}
