#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"
#include <TMath.h>

#include <string>
using namespace std;

TH1D* Fit4Gauss4( )
{
  gStyle->SetOptStat(0);
  TFile *_fileAll = TFile::Open("timeresid1m.root");
  int numhistos = 6;
  int numpar = 12;

  TH1D* histvec[6];
  TH1D* hnorm[6];

  double maxs[12] = { 7,0,130,0,0,60,0.075,7,4,0.05,15,7.5 };
  double mins[12] = { 0,-425,0,-0.5,-100,0,0,0,0,0,0,0 };

  TH1D* hpar[12];
  for(int i=0; i<numpar; i++){
    TString parname = Form("hpar%d",i);
    hpar[i] = new TH1D(parname,parname, 6,2,14);
    hpar[i]->Reset();
  }

  //  double init[6][12] = //{ {0.052, 3.25, 2.34, 0.028, 7.8, 4.93, -2.94, -96.98, 29.95, 14.65, -388.08, 105.78},
  //			 {{0.052, 3.23, -2.34, 0.027, 7.78, -4.68, -0.37, -47.99, 18.80, 6.07, -338.43, 98.80},
  //			 {0.041, 3.73, -2.53, 0.021, 9.84, -5.20, -0.22, -76.18, 33.32, 3.55, -360.36, 111.90},
  //			 {0.032, 3.91, -2.60, 0.017, 10.18, -5.14, -0.23, -75.37, 33.52, 3.70, -358.70, 112.51},
  //			 {0.030, 3.92, -2.58, 0.015, 10.20, -5.08, -0.24, -74.97, 33.66, 3.78, -357.94, 112.83},
  //	 {0.023, 4.02, -2.58, 0.014, 10.18, -5.34, -0.21, -76.67, 37.76, 3.63, -354.71, 114.72},
  //			 {0.021, 3.93, -2.33, 0.0097, 10.20, 4.07, -0.27, -73.3, 33.96, 4.08, -354.18, 113.97} };
  //			 {-147.30, 27.05, -0.0019, 0.036, 5.44, 3.86, -0.32, -66.03, 26.67, 37.86, -562.30, 145.59 },
  //			 {-2829.2, -14.99, -0.43, 0.033, 5.55, 4.06, -0.30, -72.77, 29.37, 4.91, -473.4, 142.86},
  //			 {0.027, 5.83, 4.18, 0.044, -39.32, 49.83, -0.0259, -86.89, -13.57, -4.19, -220.2, 69.28},
  //			 {0.02, 4.17, 2.94, -0.036, -7.86, -8.29, 10.9, -416.2, 120.63, -9.52, -211.7, 37.03} };

  
  double init[6][12] = {{6.07, -338.43, 98.80, -0.37, -47.99, 18.80, 0.052, 3.23, -2.34, 0.027, 7.78, -4.68 },
			{3.55, -360.36, 111.90, -0.22, -76.18, 33.32, 0.041, 3.73, -2.53, 0.021, 9.84, -5.20},
			{3.70, -358.70, 112.51, -0.23, -75.37, 33.52, 0.032, 3.91, -2.60, 0.017, 10.18, -5.14},
			{3.78, -357.94, 112.83, -0.24, -74.97, 33.66, 0.030, 3.92, -2.58, 0.015, 10.20, -5.08},
			{3.63, -354.71, 114.72, -0.21, -76.67, 37.76, 0.023, 4.02, -2.58,  0.014, 10.18, -5.34},
			{4.08, -354.18, 113.97, -0.27, -73.3, 33.96, 0.021, 3.93, -2.33, 0.0097, 10.20, 4.07} };


  TF1 *ffit[6];
  TF1 *fitnorm[6];

  for(int ihist = 0; ihist < numhistos; ihist++){
    //push back histo

    TString hname = Form("h%d",ihist);
    TH1D* h = (TH1D*) _fileAll->Get(hname);
    histvec[ihist] = h;
    
    TF1 *f = new TF1("f", "gaus(0) + gaus(3) + gaus(6) + gaus(9)", -10, 100);

    f->SetLineColor(h->GetLineColor());

    for(int ipar=0; ipar<12; ipar++){
      f->SetParameter(ipar,init[ihist][ipar]);
    }

    double d = 2*ihist + 4;
    double a1 = 0.061*d*d - 1.11*d +8.41;//3.7;
    double mu1 = 0.509*d*d - 9.32*d -317.98;//-360;//0.736*d*d - 13.09*d -303.48;//-360;
    double s1 = -0.298*d*d + 5.65*d + 88.41;//0.717*d + 106.86;
    double a2 = -0.005*d*d + d*0.086 - 0.533;//-0.225;
    double mu2 = 1.66*d - 88.41;//-75;
    double s2 = -0.89*d + 41.89;//0.357*d + 30.98;
    double a3 = 0.00024*d*d - 0.0072*d + 0.073;//0.00029*d*d - 0.0079*d + 0.075;
    double mu3 = 0.197*d + 2.61;//-0.0117*d*d + 0.25*d + 2.67;//3.9;
    double s3 = 0.092*d + 2.08;//-0.013*d*d + 0.19*d + 1.92;//2.0;
    double a4 = -0.0026*d + 0.038;//0.0000976*d*d -0.0032*d + 0.037;//-0.000066*d*d*d + 0.00176*d*d - 0.016*d +0.07;
    double mu4 = 0.345*d*d - 4.03*d + 18.66;//-0.027*d*d + 0.583*d + 6.58;//0.2256*d + 6.75;
    double s4 = 7.5;//-0.051*d*d + 0.69*d + 4.11;//-0.158*d + 6.099;
    /*    f->FixParameter(0,a1);
    f->FixParameter(1,mu1);
    f->FixParameter(2,s1);
    f->FixParameter(3,a2);
    f->FixParameter(4,mu2);
    //f->FixParameter(5,s2);
    f->FixParameter(6,a3);
    f->FixParameter(7,mu3);
    f->FixParameter(8,s3);
    f->FixParameter(9,a4);
    f->FixParameter(10,mu4);
    f->FixParameter(11,s4);*/

    histvec[ihist]->Fit("f", "MIWWR0");
    ffit[ihist] = f;

    double intf = f->Integral(-5,100);
    //TF1 *fn = new TF1("fn", "0.918147*f/[0]", -10, 100);
    TF1 *fn = new TF1("fn", "0.92*f/[0]", -10, 100);
    fn->SetParameter(0,intf);
    fn->SetLineColor(f->GetLineColor());
    fn->SetLineStyle(kDashed);
    fitnorm[ihist] = fn;

    ffit[ihist]->SetRange(-20,100);
    histvec[ihist]->GetXaxis()->SetRangeUser(-20,100);
    histvec[ihist]->GetYaxis()->SetRangeUser(0,0.1);

    for(int ipar=0; ipar<numpar; ipar++){
    
      if(ipar==2 || ipar==5 || ipar==8 || ipar==11)
        hpar[ipar]->SetBinContent(ihist+1, fabs(f->GetParameter(ipar)));
      else
        hpar[ipar]->SetBinContent(ihist+1, f->GetParameter(ipar));
    }
  }

  TCanvas *c1 = new TCanvas("c1","c1",1500,700);
  TLegend* t1 = new TLegend( 0.5, 0.6, 0.88, 0.88);

  //  c1->Divide(3,2);

  for(int ihist = 0; ihist < numhistos; ihist++){
    //c1->cd(ihist+1);

    histvec[ihist]->GetXaxis()->SetTitle("Hit time residuals [ns]");
    histvec[ihist]->GetYaxis()->SetTitle("Normalised Counts");
    histvec[ihist]->GetXaxis()->SetRangeUser(-5,100);
    histvec[ihist]->GetYaxis()->SetRangeUser(-0.02,0.1);

    //Fill histos with param values for median distance
    if(ihist == 0){      
      histvec[ihist]->Draw();
      //ffit[ihist]->Draw("same");
      fitnorm[ihist]->Draw("same");
    }
    else{
      histvec[ihist]->Draw("same");
      //      ffit[ihist]->Draw("same");
      fitnorm[ihist]->Draw("same");
    }
    //    TLegend* t1 = new TLegend( 0.5, 0.7, 0.88, 0.85);
    t1->AddEntry( histvec[ihist], Form("%dm < d < %dm", (2*ihist)+3, (2*ihist)+5), "l" );
    //t1->Draw();

  }
  
  t1->Draw();
  /*
  TCanvas *c2 = new TCanvas("c2","c2",1500,700);
  c2->Divide(3,2);

  for(int ihist = 0; ihist < numhistos; ihist++){
    TLegend* t2 = new TLegend( 0.7, 0.6, 0.88, 0.88);
    c2->cd(ihist+1);
    TF1 *f1 = new TF1("f1", "gaus",-400,400);
    TF1 *f2 = new TF1("f2", "gaus",-400,400);
    TF1 *f3 = new TF1("f3", "gaus",-400,400);
    TF1 *f4 = new TF1("f4", "gaus",-400,400);
    f1->SetLineColor(kBlack);
    f2->SetLineColor(kBlue);
    f3->SetLineColor(kRed);
    f4->SetLineColor(kGreen);
    f1->SetMinimum(-0.1);
    f1->SetMaximum(0.1);
    
    f1->SetParameter(0, ffit[ihist]->GetParameter(0));
    f1->SetParameter(1, ffit[ihist]->GetParameter(1));
    f1->SetParameter(2, ffit[ihist]->GetParameter(2));
    f2->SetParameter(0, ffit[ihist]->GetParameter(3));
    f2->SetParameter(1, ffit[ihist]->GetParameter(4));
    f2->SetParameter(2, ffit[ihist]->GetParameter(5));
    f3->SetParameter(0, ffit[ihist]->GetParameter(6));
    f3->SetParameter(1, ffit[ihist]->GetParameter(7));
    f3->SetParameter(2, ffit[ihist]->GetParameter(8));
    f4->SetParameter(0, ffit[ihist]->GetParameter(9));
    f4->SetParameter(1, ffit[ihist]->GetParameter(10));
    f4->SetParameter(2, ffit[ihist]->GetParameter(11));

    ffit[ihist]->SetRange(-5,200);
    ffit[ihist]->SetLineColor(kMagenta);
    ffit[ihist]->SetLineStyle(2);
    ffit[ihist]->SetLineWidth(1);
    
    //Fill histos with param values for median distance
    f1->Draw();
    f2->Draw("same");
    f3->Draw("same");
    f4->Draw("same");
    ffit[ihist]->Draw("same");
    f1->SetTitle( Form("%dm < d < %dm", (2*ihist)+2, (2*ihist)+4) );
    f1->GetXaxis()->SetTitle("Hit Time Residuals [ns]");
    t2->AddEntry(f1, "Gauss 0", "l");
    t2->AddEntry(f2, "Gauss 1", "l");
    t2->AddEntry(f3, "Gauss 2", "l");
    t2->AddEntry(f4, "Gauss 3", "l");
    t2->AddEntry(ffit[ihist], "Sum", "l");
    t2->Draw();
    }*/

  /*  TCanvas *c3 = new TCanvas("c1","c1",1500,700);
  c3->Divide(2,2);
  TLegend* t3 = new TLegend( 0.5, 0.6, 0.88, 0.88);
  for(int ihist = 0; ihist < 4; ihist++){
    c3->cd(ihist+1);
    TF1 *f1 = new TF1("f1", "gaus",-400,400);
    TF1 *f2 = new TF1("f2", "gaus",-400,400);
    TF1 *f3 = new TF1("f3", "gaus",-400,400);
    TF1 *f4 = new TF1("f4", "gaus",-400,400);
    TF1 *f5 = new TF1("f5", "gaus",-400,400);
    TF1 *f6 = new TF1("f6", "gaus",-400,400);

    f1->SetMinimum(-0.1);
    f1->SetMaximum(0.1);
    f1->GetXaxis()->SetTitle("Hit Time Residuals [ns]");
    TString fname = Form("Gauss %d",ihist);
    f1->SetTitle(fname);

    f1->SetLineColor(ffit[0]->GetLineColor());
    f2->SetLineColor(ffit[1]->GetLineColor());
    f3->SetLineColor(ffit[2]->GetLineColor());
    f4->SetLineColor(ffit[3]->GetLineColor());
    f5->SetLineColor(ffit[4]->GetLineColor());
    f6->SetLineColor(ffit[5]->GetLineColor());

    f1->SetParameter(0, ffit[0]->GetParameter(ihist*3   ));
    f1->SetParameter(1, ffit[0]->GetParameter(ihist*3 +1));
    f1->SetParameter(2, ffit[0]->GetParameter(ihist*3 +2));
    f2->SetParameter(0, ffit[1]->GetParameter(ihist*3   ));
    f2->SetParameter(1, ffit[1]->GetParameter(ihist*3 +1));
    f2->SetParameter(2, ffit[1]->GetParameter(ihist*3 +2));
    f3->SetParameter(0, ffit[2]->GetParameter(ihist*3   ));
    f3->SetParameter(1, ffit[2]->GetParameter(ihist*3 +1));
    f3->SetParameter(2, ffit[2]->GetParameter(ihist*3 +2));
    f4->SetParameter(0, ffit[3]->GetParameter(ihist*3   ));
    f4->SetParameter(1, ffit[3]->GetParameter(ihist*3 +1));
    f4->SetParameter(2, ffit[3]->GetParameter(ihist*3 +2));
    f5->SetParameter(0, ffit[4]->GetParameter(ihist*3   ));
    f5->SetParameter(1, ffit[4]->GetParameter(ihist*3 +1));
    f5->SetParameter(2, ffit[4]->GetParameter(ihist*3 +2));
    f6->SetParameter(0, ffit[5]->GetParameter(ihist*3   ));
    f6->SetParameter(1, ffit[5]->GetParameter(ihist*3 +1));
    f6->SetParameter(2, ffit[5]->GetParameter(ihist*3 +2));

    f1->GetXaxis()->SetTitle("Hit Time Residuals [ns]");
    //Fill histos with param values for median distance
    f1->Draw();
    f2->Draw("same");
    f3->Draw("same");
    f4->Draw("same");
    f5->Draw("same");
    f6->Draw("same");

    TLegend* t1 = new TLegend( 0.7, 0.6, 0.88, 0.85);
    t1->AddEntry( f1, Form("%dm < d < %dm", (2*0)+2, (2*0)+4), "l" );
    t1->AddEntry( f2, Form("%dm < d < %dm", (2*1)+2, (2*1)+4), "l" );
    t1->AddEntry( f3, Form("%dm < d < %dm", (2*2)+2, (2*2)+4), "l" );
    t1->AddEntry( f4, Form("%dm < d < %dm", (2*3)+2, (2*3)+4), "l" );
    t1->AddEntry( f5, Form("%dm < d < %dm", (2*4)+2, (2*4)+4), "l" );
    t1->AddEntry( f6, Form("%dm < d < %dm", (2*5)+2, (2*5)+4), "l" );
    t1->Draw();
  }
  */

  TGraph* gpar2[12];
  for(int i=0; i<numpar; i++){
    
    gpar2[i] = new TGraph(1);
    int pointcount=0;
    for (int j=1; j<=numhistos; j++){
      //if( i==0 && j==1 )
      //continue;
      //else
      pointcount++;
      gpar2[i]->SetPoint(pointcount-1, hpar[i]->GetBinCenter(j), hpar[i]->GetBinContent(j));
    
    }
  }

  TString parstr[3] = {"A", "#mu", "#sigma"};
  TCanvas *c2 = new TCanvas("c2","c2",1600,800);
  c2->Divide(3,4);
  for(int ipar=0; ipar<numpar; ipar++){

    c2->cd(ipar+1);
    gpar2[ipar]->SetMarkerColor(kBlack);

    gpar2[ipar]->Draw("AP");
    gpar2[ipar]->SetMarkerSize(4);
    gpar2[ipar]->SetMarkerStyle(2);
    gpar2[ipar]->GetXaxis()->SetTitle("Distance to PMT [m]");
    int gnum = ipar%3;
    
    TString name = Form("%s_%d",parstr[gnum].Data(),ipar/3);
    gpar2[ipar]->GetYaxis()->SetTitle(name);
    gpar2[ipar]->SetTitle(name);
    gpar2[ipar]->GetXaxis()->SetLimits(2,14);
    gpar2[ipar]->SetMinimum(mins[ipar]);
    gpar2[ipar]->SetMaximum(maxs[ipar]);

  }

}
