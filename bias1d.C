#include <TH2Poly.h>
#include "TMath.h"
#include <TH2D.h>
#include <vector>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <THStack.h>
#include <iostream>
#include "TF1.h"
#include <TLatex.h>

void GetGaussian(TH1D *& hpost, TF1 *& gauss, double &central, double &error);
void GetMode(TH1D * const hpost, double &central, double &error, double &error_pos, double &error_neg);
void GetArithmetic(TH1D * const hpost, double &mean, double &error);
void GetMedian(TH1D * const hpost, double &central, double &error, double &error_pos, double &error_neg);
double GetMedianValue(const TH1D * h1);

int bias1d(){

  TCanvas* c0 = new TCanvas("c0", "c0", 0, 0, 1024, 1024);
  c0->SetGrid();
  gStyle->SetOptStat(0);
  //  gPad->SetLogy();
  //  gStyle->SetOptTitle(0);
  c0->SetTickx();
  c0->SetTicky();
  c0->SetBottomMargin(0.1);
  c0->SetTopMargin(0.1);
  c0->SetRightMargin(0.05);
  c0->SetLeftMargin(0.13);

  TString canvasname = "1dbiasesR_Z6000.pdf[";
  c0->Print(canvasname);
  canvasname.ReplaceAll("[","");

  //  TFile* fileTree = TFile::Open("redoneoutfit.root");
  TFile* fileTree = TFile::Open("tree.root");
  TTree* biasTree = (TTree*)fileTree->Get("biasTree");
  float trueX, trueY, trueZ, recoX_4PDFS, recoY_4PDFS, recoZ_4PDFS, recoX_8PDFS, recoY_8PDFS, recoZ_8PDFS;
  biasTree->SetBranchAddress("trueX", &trueX);
  biasTree->SetBranchAddress("trueY", &trueY);
  biasTree->SetBranchAddress("trueZ", &trueZ);
  biasTree->SetBranchAddress("recoX_4PDFs", &recoX_4PDFS);
  biasTree->SetBranchAddress("recoY_4PDFs", &recoY_4PDFS);
  biasTree->SetBranchAddress("recoZ_4PDFs", &recoZ_4PDFS);
  biasTree->SetBranchAddress("recoX_8PDFs", &recoX_8PDFS);
  biasTree->SetBranchAddress("recoY_8PDFs", &recoY_8PDFS);
  biasTree->SetBranchAddress("recoZ_8PDFs", &recoZ_8PDFS);

  Long64_t nentries = biasTree->GetEntries();

  TH1D *bias1dZ[30][2];
  for (int i=0; i<30; i++){
    for(int j=0; j<2; j++){
      TString hname = Form("BiasZ1D_%d_Fitter_%d",i,j);
      bias1dZ[i][j] = new TH1D(hname, hname, 25, -400, 400);
    }
  }

  for(int i=0; i<nentries; i++){
    biasTree->GetEntry(i);
    float r = sqrt(trueX*trueX + trueY*trueY + trueZ*trueZ);
    float r_4pdfs = sqrt(recoX_4PDFS*recoX_4PDFS + recoY_4PDFS*recoY_4PDFS + (recoZ_4PDFS-855.48)*(recoZ_4PDFS-855.48));
    float r_8pdfs = sqrt(recoX_8PDFS*recoX_8PDFS + recoY_8PDFS*recoY_8PDFS + (recoZ_8PDFS-855.48)*(recoZ_8PDFS-855.48));
    int bin = floor(15+r*15/6000)-1;

    /*    if(fabs(recoX_4PDFS-recoX_8PDFS)>1000 || fabs(recoY_4PDFS-recoY_8PDFS)>1000 || fabs(recoZ_4PDFS-recoZ_8PDFS)>1000)
      bias1dZ[bin][0]->Fill(recoY_4PDFS-trueY);
    else if((r_4pdfs<1500 && r_8pdfs>1500) || (r_4pdfs>1500 && r_8pdfs<1500))
      bias1dZ[bin][0]->Fill(recoY_4PDFS-trueY);
    else
    bias1dZ[bin][0]->Fill(recoY_4PDFS-trueY);*/

    bias1dZ[bin][0]->Fill(recoX_8PDFS-trueX);
    bias1dZ[bin][1]->Fill(recoX_8PDFS-trueX);
  }

  TFile * outFile = new TFile("testSwitchY.root","RECREATE");
  TH1D *gaussplot[2];
  TH1D *modeplot[2];
  TH1D *medianplot[2];
  TH1D *arithmeticplot[2];
  TF1 *gauss = new TF1("gauss","[0]/sqrt(2.0*3.14159)/[2]*TMath::Exp(-0.5*pow(x-[1],2)/[2]/[2])",-5,5);
  gauss->SetLineWidth(2);
  gauss->SetLineColor(kOrange-5);
  
  for(int j=0; j<2; j++){
   
    gaussplot[j] = new TH1D("gaus", "gaus", 30, -6000, 6000);
    modeplot[j] = new TH1D("mode", "mode", 31, -6000, 6000);
    medianplot[j] = new TH1D("median", "median", 31, -6000, 6000);
    arithmeticplot[j] = new TH1D("mean", "mean", 31, -6000, 6000);
  
    for(int i=0; i<30; i++){
      double central, error;
      GetGaussian(bias1dZ[i][j], gauss, central, error);
      gaussplot[j]->SetBinContent(i, gauss->GetParameter(2));
      gaussplot[j]->SetBinError(i, gauss->GetParError(2));
      double peakval, sigma_p, sigma_m, sigma_mode;
      GetMode(bias1dZ[i][j], peakval, sigma_mode, sigma_p, sigma_m);
      modeplot[j]->SetBinContent(i, peakval);
      modeplot[j]->SetBinError(i, sigma_mode);
      double mean, rms;
      GetArithmetic(bias1dZ[i][j], mean, rms);
      arithmeticplot[j]->SetBinContent(i, mean);
      arithmeticplot[j]->SetBinError(i, rms);
      std::cout << i << " " << gauss->GetParameter(2) << " " << gauss->GetParError(2) << std::endl;
      double median, sigma_med_p, sigma_med_m, sigma_med;
      GetMedian(bias1dZ[i][j], median, sigma_med, sigma_med_p, sigma_med_m);
      medianplot[j]->SetBinContent(i, median);
      medianplot[j]->SetBinError(i, sigma_med);

      if(gauss->GetParameter(1)>1000 || gauss->GetParameter(1)<-1000 || gauss->GetParameter(2)>1000 || gauss->GetParameter(2)<-1000){
      	gaussplot[j]->SetBinContent(i, peakval);
	gaussplot[j]->SetBinError(i, sigma_mode);
      }

      TLine *mode = new TLine(peakval, bias1dZ[i][j]->GetMinimum(), peakval, bias1dZ[i][j]->GetMaximum());
      mode->SetLineColor(kBlue);
      mode->SetLineWidth(2);
      mode->SetLineStyle(kSolid);

      TLine *medianline = new TLine(median, bias1dZ[i][j]->GetMinimum(), median, bias1dZ[i][j]->GetBinContent(bias1dZ[i][j]->FindBin(median)));
      medianline->SetLineColor(kRed);
      medianline->SetLineWidth(2);
      medianline->SetLineStyle(10);

      TLegend *leg = new TLegend(0.13, 0.6, 0.6, 0.88);
      leg->SetTextSize(0.03);
      leg->AddEntry(bias1dZ[i][j], Form("#splitline{Arithmetic}{#mu = %.2f, #sigma = %.2f}", mean, rms, "l"));
      leg->AddEntry(gauss, Form("#splitline{Gauss}{#mu = %.2f, #sigma = %.2f (%.2f)}", gauss->GetParameter(1), gauss->GetParError(1), gauss->GetParameter(2)), "l");
      leg->AddEntry(mode, Form("#splitline{Mode}{#mu = %.2f, #sigma = %.2f (+%.2f-%.2f)}", peakval, sigma_mode, sigma_p, sigma_m), "l");
      leg->AddEntry(medianline, Form("#splitline{Median}{#mu = %.2f, #sigma = %.2f (+%.2f-%.2f)}", median, sigma_med, sigma_med_p, sigma_med_m), "l");

      bias1dZ[i][j]->GetXaxis()->SetTitle("Bias in Y [mm]");
      bias1dZ[i][j]->GetYaxis()->SetTitleOffset(1.3);
      bias1dZ[i][j]->GetYaxis()->SetTitle("Events");
      bias1dZ[i][j]->SetTitle(Form("R: %.2d #minus %.2d mm", (i-15)*6000/15, (i+1-15)*6000/15 ));

      bias1dZ[i][j]->Draw();
      mode->Draw("same");
      medianline->Draw("same");

      leg->SetLineColor(0);
      leg->SetLineStyle(0);
      leg->SetFillColor(0);
      leg->SetFillStyle(0);
      leg->Draw("same");

      //      c0->SetName(bias1dZ[i][j]->GetName());
      //c0->SetTitle(bias1dZ[i][j]->GetTitle());
      if(j==0)
	c0->Print(canvasname);

      bias1dZ[i][j]->Write();
    }
   
  }
  
  TLine *line = new TLine(-6000,0,6000,0);
  
  gaussplot[0]->SetLineColor(kOrange-5);
  gaussplot[0]->Draw("L E1");
  //  modeplot[0]->SetLineColor(kRed);
  //modeplot[0]->Draw("same L E1");
  //arithmeticplot[0]->SetLineColor(kBlack);
  //arithmeticplot[0]->Draw("same L E1");
  ////medianplot[0]->SetLineColor(kBlue);
  //medianplot[0]->Draw("same L E1");
  
  TLegend *leg2 = new TLegend(0.15, 0.7, 0.4, 0.88);
  leg2->SetTextSize(0.03);
  leg2->AddEntry(arithmeticplot[0], "Mean", "l");
  leg2->AddEntry(gaussplot[0], "Gaussian", "l");
  leg2->AddEntry(modeplot[0], "Mode", "l");
  leg2->AddEntry(medianplot[0], "Median" , "l");
  leg2->Draw("same");
  //  line->Draw("same");
  gaussplot[0]->GetYaxis()->SetRangeUser(40,120);
  gaussplot[0]->GetXaxis()->SetRangeUser(0,6000);
  gaussplot[0]->Write();
  c0->Write("c0");
  canvasname+="]";
  c0->Print(canvasname);
}

// **************************
// Get the mean and RMS of a 1D posterior
void GetArithmetic(TH1D * const hpost, double &mean, double &error) {
  // **************************
  mean = hpost->GetMean();
  error = hpost->GetRMS();
}

// **************************
double GetMedianValue(const TH1D * h1) { 
  // **************************

  int n = h1->GetXaxis()->GetNbins();  
  std::vector<double>  x(n);
  h1->GetXaxis()->GetCenter( &x[0] );
  const double * y = h1->GetArray(); 
  // exclude underflow/overflows from bin content array y
  return TMath::Median(n, &x[0], &y[1]); 
}


// **************************
// Get Gaussian characteristics
void GetGaussian(TH1D *& hpost, TF1 *& gauss, double &central, double &error) {
  // **************************
  double mean = hpost->GetMean();
  double err = hpost->GetRMS();
  double peakval = hpost->GetBinCenter(hpost->GetMaximumBin());
  // Set the range for the Gaussian fit
  gauss->SetRange(mean - 1.5*err , mean + 1.5*err);
  // Set the starting parameters close to RMS and peaks of the histograms
  gauss->SetParameters(hpost->GetMaximum()*err*sqrt(2*3.14), peakval, err);

  // Perform the fit
  hpost->Fit(gauss->GetName(),"Rq");
  hpost->SetStats(0);

  central = gauss->GetParameter(1);
  error = gauss->GetParameter(2);
}

// **************************
void GetMode(TH1D * const hpost, double &central, double &error, double &error_pos, double &error_neg) {
  // **************************

  // Get the bin which has the largest posterior density
  int MaxBin = hpost->GetMaximumBin();
  // And it's value
  double peakval = hpost->GetBinCenter(MaxBin);

  // The total integral of the posterior
  double integral = hpost->Integral();

  // Keep count of how much area we're covering
  double sum = 0.0;

  // Counter for current bin
  int CurrBin = MaxBin;
  while (sum/integral < 0.6827/2.0 && CurrBin < hpost->GetNbinsX()+1) {
    sum += hpost->GetBinContent(CurrBin);
    CurrBin++;
  }
  double sigma_p = fabs(hpost->GetBinCenter(MaxBin)-hpost->GetBinCenter(CurrBin));
  // Reset the sum
  sum = 0.0;

  // Reset the bin counter
  CurrBin = MaxBin;
  // Counter for current bin
  while (sum/integral < 0.6827/2.0 && CurrBin >= 0) {
    sum += hpost->GetBinContent(CurrBin);
    CurrBin--;
  }
  double sigma_m = fabs(hpost->GetBinCenter(CurrBin)-hpost->GetBinCenter(MaxBin));

  // Now do the double sided HPD
  sum = 0.0;
  int LowBin = MaxBin-1;
  int HighBin = MaxBin+1;
  double LowCon = 0.0;
  double HighCon = 0.0;
  while (sum/integral < 0.6827 && LowBin >= 0 && HighBin < hpost->GetNbinsX()+1) {

    // Get the slice
    LowCon = hpost->GetBinContent(LowBin);
    HighCon = hpost->GetBinContent(HighBin);

    // If we're on the last slice and the lower contour is larger than the upper
    if ((sum+LowCon+HighCon)/integral > 0.6827 && LowCon > HighCon) {
      sum += LowCon;
      break;
      // If we're on the last slice and the upper contour is larger than the lower
    } else if ((sum+LowCon+HighCon)/integral > 0.6827 && HighCon >= LowCon) {
      sum += HighCon;
      break;
    } else {
      sum += LowCon + HighCon;
    }

    LowBin--;
    HighBin++;
  }

  double sigma_hpd = 0.0;
  if (LowCon > HighCon) {
    sigma_hpd = fabs(hpost->GetBinCenter(LowBin)-hpost->GetBinCenter(MaxBin));
  } else {
    sigma_hpd = fabs(hpost->GetBinCenter(HighBin)-hpost->GetBinCenter(MaxBin));
  }

  central = peakval;
  error = sigma_hpd;
  error_pos = sigma_p;
  error_neg = sigma_m;

}

// **************************
void GetMedian(TH1D * const hpost, double &central, double &error, double &error_pos, double &error_neg) {
  // **************************

  // Get the median value
  double medval = GetMedianValue(hpost);
  // And it's bin num
  int MedBin = hpost->GetXaxis()->FindBin(medval);

  // The total integral of the posterior
  double integral = hpost->Integral();

  // Keep count of how much area we're covering
  double sum = 0.0;

  // Counter for current bin
  int CurrBin = MedBin;
  while (sum/integral < 0.6827/2.0 && CurrBin < hpost->GetNbinsX()+1) {
    sum += hpost->GetBinContent(CurrBin);
    CurrBin++;
  }
  double sigma_p = fabs(hpost->GetBinCenter(MedBin)-hpost->GetBinCenter(CurrBin));
  // Reset the sum
  sum = 0.0;

  // Reset the bin counter
  CurrBin = MedBin;
  // Counter for current bin
  while (sum/integral < 0.6827/2.0 && CurrBin >= 0) {
    sum += hpost->GetBinContent(CurrBin);
    CurrBin--;
  }
  double sigma_m = fabs(hpost->GetBinCenter(CurrBin)-hpost->GetBinCenter(MedBin));

  // Now do the double sided HPD
  sum = 0.0;
  int LowBin = MedBin-1;
  int HighBin = MedBin+1;
  double LowCon = 0.0;
  double HighCon = 0.0;
  while (sum/integral < 0.6827 && LowBin >= 0 && HighBin < hpost->GetNbinsX()+1) {

    // Get the slice
    LowCon = hpost->GetBinContent(LowBin);
    HighCon = hpost->GetBinContent(HighBin);

    // If we're on the last slice and the lower contour is larger than the upper
    if ((sum+LowCon+HighCon)/integral > 0.6827 && LowCon > HighCon) {
      sum += LowCon;
      break;
      // If we're on the last slice and the upper contour is larger than the lower
    } else if ((sum+LowCon+HighCon)/integral > 0.6827 && HighCon >= LowCon) {
      sum += HighCon;
      break;
    } else {
      sum += LowCon + HighCon;
    }

    LowBin--;
    HighBin++;
  }

  double sigma_hpd = 0.0;
  if (LowCon > HighCon) {
    sigma_hpd = fabs(hpost->GetBinCenter(LowBin)-hpost->GetBinCenter(MedBin));
  } else {
    sigma_hpd = fabs(hpost->GetBinCenter(HighBin)-hpost->GetBinCenter(MedBin));
  }

  central = medval;
  error = sigma_hpd;
  error_pos = sigma_p;
  error_neg = sigma_m;

}
