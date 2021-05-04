#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"
#include <TMath.h>

#include <string>
using namespace std;

TH1D* PlotNorm( )
{
  gStyle->SetOptStat(0);
  TFile *_fileAll = TFile::Open("timeresidUnnorm.root");
  int numhistos = 6;
  int numpar = 12;

  TCanvas *c1 = new TCanvas("c1","c1",1500,700);
  //c1->Divide(3,2);
  
  TLegend* t1 = new TLegend( 0.5, 0.6, 0.88, 0.88);
  for(int ihist = 2; ihist < 3; ihist++){
    std::cout << "h2" << std::endl;
    //push back histo
    // c1->cd(ihist+1);
    //int ihist=0;
    TString hname = Form("h%d",ihist);
    TH1D* h = (TH1D*) _fileAll->Get(hname);
    h->Scale(100000/h->Integral());
    h->GetXaxis()->SetTitle("Hit time residuals [ns]");
    h->GetYaxis()->SetTitle("Normalised Counts");
    //h->GetXaxis()->SetRangeUser(-5,100);
    // h->GetYaxis()->SetRangeUser(-0.05,0.1);
    h->Draw();

    TF1 *f = new TF1("f", "100000*( ([0]*[36]*[36] + [1]*[36] + [2]) * exp(-0.5 * ( (x - ([3]*[36]*[36] + [4]*[36] + [5]) ) / ([6]*[36]*[36] + [7]*[36] + [8]) )^2) + ([9]*[36]*[36] + [10]*[36] + [11]) * exp(-0.5 * ( (x - ([12]*[36]*[36] + [13]*[36] + [14]) ) / ([15]*[36]*[36] + [16]*[36] + [17]) )^2) + ([18]*[36]*[36] + [19]*[36] + [20]) * exp(-0.5 * ( (x - ([21]*[36]*[36] + [22]*[36] + [23]) ) / ([24]*[36]*[36] + [25]*[36] + [26]) )^2) + ([27]*[36]*[36] + [28]*[36] + [29]) * exp(-0.5 * ( (x - ([30]*[36]*[36] + [31]*[36] + [32]) ) / ([33]*[36]*[36] + [34]*[36] + [35]) )^2) )", -10, 100);
    TF1 *f2 = new TF1("f2", "50000*( ([0]*[36]*[36] + [1]*[36] + [2]) * exp(-0.5 * ( (x - ([3]*[36]*[36] + [4]*[36] + [5]) ) / ([6]*[36]*[36] + [7]*[36] + [8]) )^2) + ([9]*[36]*[36] + [10]*[36] + [11]) * exp(-0.5 * ( (x - ([12]*[36]*[36] + [13]*[36] + [14]) ) / ([15]*[36]*[36] + [16]*[36] + [17]) )^2) + ([18]*[36]*[36] + [19]*[36] + [20]) * exp(-0.5 * ( (x - ([21]*[36]*[36] + [22]*[36] + [23]) ) / ([24]*[36]*[36] + [25]*[36] + [26]) )^2) + ([27]*[36]*[36] + [28]*[36] + [29]) * exp(-0.5 * ( (x - ([30]*[36]*[36] + [31]*[36] + [32]) ) / ([33]*[36]*[36] + [34]*[36] + [35]) )^2) )", -10, 10);

    //TF2 *f2 = new TF2("f2", "([0]*y*y + [1]*y + [2]) * exp(-0.5 * ( (x - ([3]*y*y + [4]*y + [5]) ) / ([6]*y*y + [7]*y + [8]) )^2) + ([9]*y*y + [10]*y + [11]) * exp(-0.5 * ( (x - ([12]*y*y + [13]*y + [14]) ) / ([15]*y*y + [16]*y + [17]) )^2) + ([18]*y*y + [19]*y + [20]) * exp(-0.5 * ( (x - ([21]*y*y + [22]*y + [23]) ) / ([24]*y*y + [25]*y + [26]) )^2) + ([27]*y*y + [28]*y + [29]) * exp(-0.5 * ( (x - ([30]*y*y + [31]*y + [32]) ) / ([33]*y*y + [34]*y + [35]) )^2)", -10, 100, 2,14);

    double pars[37] = {0,0,3.7,0,0,-360,0,0.717,106.86,0,0,-0.225,0,0,-75,0,0.357,30.98,0.00029,-0.0079,0.075,-0.0117,0.25,2.67,-0.013,0.19,1.92,0.0000976,-0.0032,0.037,-0.027,0.583,6.58,-0.051,0.69,4.11};
   
    //double pars[36] = {0,0,3.7,0,0,-360,0,0.717,106.86,0,0,-0.225,0,0,-75,0,0.357,30.98,0.00029,-0.0079,0.075,-0.0117,0.25,2.67,-0.013,0.19,1.92,0.0000976,-0.0032,0.037,-0.027,0.583,6.58,-0.051,0.69,4.11};

    double d = 2*ihist + 3;
    //f->SetParameter(36,d);
    pars[36] = d;
    f->SetParameters(pars);
    f2->SetParameters(pars);

    f->SetLineColor(h->GetLineColor());
    f->SetLineStyle(kDashed);
    f->SetRange(-200,500);
    f->Draw("same");

    f2->SetLineColor(kBlack);
    f2->SetLineStyle(kDashed);
    //    f2->Draw("same");
    //TLegend* t1 = new TLegend( 0.5, 0.6, 0.88, 0.88);
    t1->AddEntry( h, Form("%dm < d < %dm", (2*ihist)+2, (2*ihist)+4), "l" );
    //t1->Draw("");
    //f2->GetXaxis()->SetTitle("Hit Time Residuals, [ns]");
    //f2->GetYaxis()->SetTitle("Distance to PMT, [m]");
    //f2->GetXaxis()->SetTitleOffset(1.5);
    //f2->GetYaxis()->SetTitleOffset(1.5);
    //f2->Draw("surf5");
  }
  t1->Draw();
}
