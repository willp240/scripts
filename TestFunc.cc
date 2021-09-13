#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"
#include <TMath.h>

#include <string>
using namespace std;

TH1D* TestFunc( )
{
  gStyle->SetOptStat(0);
  TFile *_fileAll = TFile::Open("timeresid.root");
  int numhistos = 6;
  int numpar = 12;

  TCanvas *c1 = new TCanvas("c1","c1",1500,700);
  //c1->Divide(3,2);
  
  TLegend* t1 = new TLegend( 0.5, 0.6, 0.88, 0.88);
  for(int ihist = 0; ihist < 1; ihist++){
    //push back histo

    TString hname = Form("h%d",ihist);
    TH1D* h = (TH1D*) _fileAll->Get(hname);
    h->GetXaxis()->SetTitle("Hit time residuals [ns]");
    h->GetYaxis()->SetTitle("Normalised Counts");
    //h->GetXaxis()->SetRangeUser(-5,100);
    h->GetYaxis()->SetRangeUser(-0.05,0.1);
    h->Draw("same");


    TF1* f = new TF1("f", "(0.2) * (1/(([4]*[40]*[40]*[40] + [5]*[40]*[40] + [6]*[40] + [7]) * sqrt(2*TMath::Pi()))) * exp(-0.5*((x-([0]*[40]*[40]*[40] + [1]*[40]*[40] + [2]*[40] + [3]))/([4]*[40]*[40]*[40] + [5]*[40]*[40] + [6]*[40] + [7])*((x-([0]*[40]*[40]*[40] + [1]*[40]*[40] + [2]*[40] + [3]))/([4]*[40]*[40]*[40] + [5]*[40]*[40] + [6]*[40] + [7])))) + (0.2) * (1/(([12]*[40]*[40]*[40] + [13]*[40]*[40] + [14]*[40] + [15]) * sqrt(2*TMath::Pi()))) * exp(-0.5*((x-([8]*[40]*[40]*[40] + [9]*[40]*[40] + [10]*[40] + [11]))/([12]*[40]*[40]*[40] + [13]*[40]*[40] + [14]*[40] + [15])*((x-([8]*[40]*[40]*[40] + [9]*[40]*[40] + [10]*[40] + [11]))/([12]*[40]*[40]*[40] + [13]*[40]*[40] + [14]*[40] + [15])))) + (0.2) * (1/(([20]*[40]*[40]*[40] + [21]*[40]*[40] + [22]*[40] + [23]) * sqrt(2*TMath::Pi()))) * exp(-0.5*((x-([16]*[40]*[40]*[40] + [17]*[40]*[40] + [18]*[40] + [19]))/([20]*[40]*[40]*[40] + [21]*[40]*[40] + [22]*[40] + [23])*((x-([16]*[40]*[40]*[40] + [17]*[40]*[40] + [18]*[40] + [19]))/([20]*[40]*[40]*[40] + [21]*[40]*[40] + [22]*[40] + [23])))) + (0.2) * (1/(([28]*[40]*[40]*[40] + [29]*[40]*[40] + [30]*[40] + [31]) * sqrt(2*TMath::Pi()))) * exp(-0.5*((x-([24]*[40]*[40]*[40] + [25]*[40]*[40] + [26]*[40] + [27]))/([28]*[40]*[40]*[40] + [29]*[40]*[40] + [30]*[40] + [31])*((x-([24]*[40]*[40]*[40] + [25]*[40]*[40] + [26]*[40] + [27]))/([28]*[40]*[40]*[40] + [29]*[40]*[40] + [30]*[40] + [31])))) + (0.2) * (1/(([36]*[40]*[40]*[40] + [37]*[40]*[40] + [38]*[40] + [39]) * sqrt(2*TMath::Pi()))) * exp(-0.5*((x-([32]*[40]*[40]*[40] + [33]*[40]*[40] + [34]*[40] + [35]))/([36]*[40]*[40]*[40] + [37]*[40]*[40] + [38]*[40] + [39])*((x-([32]*[40]*[40]*[40] + [33]*[40]*[40] + [34]*[40] + [35]))/([36]*[40]*[40]*[40] + [37]*[40]*[40] + [38]*[40] + [39]))))", -100,300);

    double pars[41] = {0.00204014,-0.0536985,0.614027,0.580781,0.00056154,-0.0135426,0.215268,1.19425,0.0101883,-0.245022,2.39863,-1.13178,0.00142124,-0.029282,0.439138,1.06026,0.0155742,-0.366303,4.12082,-2.53441,0.00267831,-0.0777945,1.24862,0.00248356,-0.0243384,0.455121,1.23821,5.11949,0.0034488,-0.253894,4.05532,-3.86752,-0.0496268,0.402598,8.99794,-1.46508,0.00850524,-0.44958,6.38404,6.20506 };

    double d = 2*ihist + 3;
    pars[40] = d;
    f->SetParameters(pars);

    std::cout << d << " " << f->Integral(-1000,1000) << std::endl;
    f->SetLineColor(h->GetLineColor());
    f->SetLineStyle(kDashed);
    f->SetRange(-200,500);
    f->Draw("same");

    //TLegend* t1 = new TLegend( 0.5, 0.6, 0.88, 0.88);
    t1->AddEntry( h, Form("%dm < d < %dm", (2*ihist)+2, (2*ihist)+4), "l" );
    //t1->Draw("");
    //f2->GetXaxis()->SetTitle("Hit Time Residuals, [ns]");
    //f2->Get[75]axis()->SetTitle("Distance to PMT, [m]");
    //f2->GetXaxis()->SetTitleOffset(1.5);
    //f2->GetYaxis()->SetTitleOffset(1.5);
    //f2->Draw("surf5");
  }
  t1->Draw();

  /*
  TF2* f2 = new TF2("f2", "100000 * ((0.2) * (1/(([4]*y*y*y + [5]*y*y + [6]*y + [7]) * sqrt(2*TMath::Pi()))) * exp(-0.5*((x-([0]*y*y*y + [1]*y*y + [2]*y + [3]))/([4]*y*y*y + [5]*y*y + [6]*y + [7])*((x-([0]*y*y*y + [1]*y*y + [2]*y + [3]))/([4]*y*y*y + [5]*y*y + [6]*y + [7])))) + (0.2) * (1/(([12]*y*y*y + [13]*y*y + [14]*y + [15]) * sqrt(2*TMath::Pi()))) * exp(-0.5*((x-([8]*y*y*y + [9]*y*y + [10]*y + [11]))/([12]*y*y*y + [13]*y*y + [14]*y + [15])*((x-([8]*y*y*y + [9]*y*y + [10]*y + [11]))/([12]*y*y*y + [13]*y*y + [14]*y + [15])))) + (0.2) * (1/(([20]*y*y*y + [21]*y*y + [22]*y + [23]) * sqrt(2*TMath::Pi()))) * exp(-0.5*((x-([16]*y*y*y + [17]*y*y + [18]*y + [19]))/([20]*y*y*y + [21]*y*y + [22]*y + [23])*((x-([16]*y*y*y + [17]*y*y + [18]*y + [19]))/([20]*y*y*y + [21]*y*y + [22]*y + [23])))) + (0.2) * (1/(([28]*y*y*y + [29]*y*y + [30]*y + [31]) * sqrt(2*TMath::Pi()))) * exp(-0.5*((x-([24]*y*y*y + [25]*y*y + [26]*y + [27]))/([28]*y*y*y + [29]*y*y + [30]*y + [31])*((x-([24]*y*y*y + [25]*y*y + [26]*y + [27]))/([28]*y*y*y + [29]*y*y + [30]*y + [31])))) + (0.2) * (1/(([36]*y*y*y + [37]*y*y + [38]*y + [39]) * sqrt(2*TMath::Pi()))) * exp(-0.5*((x-([32]*y*y*y + [33]*y*y + [34]*y + [35]))/([36]*y*y*y + [37]*y*y + [38]*y + [39])*((x-([32]*y*y*y + [33]*y*y + [34]*y + [35]))/([36]*y*y*y + [37]*y*y + [38]*y + [39])))))", -100,300,2,14);

  double Pars[40] = {0.00204014,-0.0536985,0.614027,0.580781,0.00056154,-0.0135426,0.215268,1.19425,0.0101883,-0.245022,2.39863,-1.13178,0.00142124,-0.029282,0.439138,1.06026,0.0155742,-0.366303,4.12082,-2.53441,0.00267831,-0.0777945,1.24862,0.00248356,-0.0243384,0.455121,1.23821,5.11949,0.0034488,-0.253894,4.05532,-3.86752,-0.0496268,0.402598,8.99794,-1.46508,0.00850524,-0.44958,6.38404,6.20506 };

  f2->SetParameters(Pars);
  f2->GetXaxis()->SetTitle("Hit Time Residual, ns");
  f2->GetYaxis()->SetTitle("Distance to PMT, m");
  f2->GetZaxis()->SetTitle("PDF");
  f2->Draw("surf3");*/
}
