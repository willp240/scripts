#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"
#include <TMath.h>

#include <string>
using namespace std;

TH1D* UnNormGauss( )
{
  gStyle->SetOptStat(0);
  TFile *_fileAll = TFile::Open("timeresidUnnorm.root");
  int numhistos = 6;
  int numpar = 11;

  double maxs[11] = { 6,5,15,7,30,15,60,20,100,50,3*pow(10,7) };
  double mins[11] = { 0,0,0,0,0,0,0,0,0,0,0 };

  TH1D* hpar[15];
  for(int i=0; i<numpar; i++){
    TString parname = Form("hpar%d",i);
    hpar[i] = new TH1D(parname,parname, 6,2,14);
    hpar[i]->Reset();
  }

  TH1D* histvec[6];
  
  double init2[11] = {4,2,5.5,2,11,3,23,10,60,20,1.125*pow(10,7)};

  TF1 *ffit[6];
 
  for(int ihist =0; ihist < 6; ihist++){
    //push back histo

    TString hname = Form("h%d",ihist);
    TH1D* h = (TH1D*) _fileAll->Get(hname);
    histvec[ihist] = h;
    
    TF1 *f = new TF1("f", "((0.2)*(1/([1]*sqrt(2*TMath::Pi())))*exp(-0.5*((x-[0])/[1])*((x-[0])/[1])) + (0.2)*(1/([3]*sqrt(2*TMath::Pi())))*exp(-0.5*((x-[2])/[3])*((x-[2])/[3])) + (0.2)*(1/([5]*sqrt(2*TMath::Pi())))*exp(-0.5*((x-[4])/[5])*((x-[4])/[5])) + 0.2*(1/([7]*sqrt(2*TMath::Pi())))*exp(-0.5*((x-[6])/[7])*((x-[6])/[7])) + 0.2*(1/([9]*sqrt(2*TMath::Pi())))*exp(-0.5*((x-[8])/[9])*((x-[8])/[9]))) * [10] ", -50, 100);


    f->SetLineColor(h->GetLineColor());
 
    for(int ipar=0; ipar<numpar; ipar++){
      f->SetParameter(ipar,init2[ipar]);
    }

    /*f->SetParLimits(0,-10,30);
    f->SetParLimits(2,-10,30);
    f->SetParLimits(4,-10,30);
    f->SetParLimits(6,-10,30);
    f->SetParLimits(8,-10,30);
    f->SetParLimits(1,0,100);
    f->SetParLimits(3,0,100);
    f->SetParLimits(5,0,100);
    f->SetParLimits(7,0,100);
    f->SetParLimits(9,0,100);*/
    // f->SetParLimits(2,-10,30);
    // f->SetParLimits(38,0,100);

    double d = 2*ihist + 3;
    double m1 = 0.00204014*d*d*d - 0.0536985*d*d + 0.614027*d + 0.580781;
    double s1 = 0.00056154*d*d*d - 0.0135426*d*d + 0.215268*d + 1.19425;
    double m2 = 0.0101883*d*d*d - 0.245022*d*d + 2.39863*d - 1.13178;
    double s2 = 0.00142124*d*d*d - 0.029282*d*d + 0.439138*d + 1.06026;
    double m3 = 0.0155742*d*d*d - 0.366303*d*d + 4.12082*d - 2.53441;
    double s3 = 0.00267831*d*d*d - 0.0777945*d*d + 1.24862*d + 0.00248356;
    double m4 = -0.0243384*d*d*d + 0.455121*d*d + 1.23821*d + 5.11949;
    double s4 = 0.0034488*d*d*d - 0.253894*d*d + 4.05532*d - 3.86752;
    double m5 = -0.0496268*d*d*d + 0.402598*d*d + 8.99794*d - 1.46508;
    double s5 = 0.00850524*d*d*d - 0.44958*d*d + 6.38404*d + 6.20506;
    double A1 = 75953.5*d*d*d - 2.74041*pow(10,6)*d*d + 2.71395*pow(10,7)*d - 4.68104*pow(10,7);
 
    
    f->FixParameter(0,m1);
    f->FixParameter(1,s1);
    f->FixParameter(2,m2);
    f->FixParameter(3,s2);
    f->FixParameter(4,m3);
    f->FixParameter(5,s3);
    f->FixParameter(6,m4);
    f->FixParameter(7,s4);
    f->FixParameter(8,m5);
    f->FixParameter(9,s5);
    //    f->FixParameter(10,A1);
    
    histvec[ihist]->Fit("f", "RMIWW0");
    ffit[ihist] = f;
        
    ffit[ihist]->SetRange(-100,300);
    histvec[ihist]->GetXaxis()->SetRangeUser(-100,300);
  
    for(int ipar=0; ipar<numpar; ipar++){
      if(ipar==2 || ipar==4 || ipar==6 || ipar==8)
        hpar[ipar]->SetBinContent(ihist+1, fabs(f->GetParameter(ipar)));
      else
        hpar[ipar]->SetBinContent(ihist+1, f->GetParameter(ipar));
    }
  }
  
  TCanvas *c1 = new TCanvas("c1","c1",1500,1000);
  TLegend* t1 = new TLegend( 0.5, 0.6, 0.88, 0.88);

  //  c1->Divide(3,2);

  for(int ihist = 0; ihist<6; ihist++){
    //c1->cd(ihist+1);

    histvec[ihist]->GetXaxis()->SetTitle("Hit time residuals [ns]");
    histvec[ihist]->GetYaxis()->SetTitle("Normalised Counts");
    histvec[ihist]->GetXaxis()->SetRangeUser(-50,100);
  
    //Fill histos with param values for median distance
    if(ihist == 0){      
      histvec[ihist]->Draw();
      ffit[ihist]->Draw("same");
    }
    else{
      histvec[ihist]->Draw("same");
      ffit[ihist]->Draw("same");
    }
    //    TLegend* t1 = new TLegend( 0.5, 0.7, 0.88, 0.85);
    t1->AddEntry( histvec[ihist], Form("%dm < d < %dm", (2*ihist)+2, (2*ihist)+4), "l" );
    //t1->Draw();
  }
  
  t1->Draw();

  TGraph* gpar2[11];
  for(int i=0; i<numpar; i++){
    gpar2[i] = new TGraph(1);
    int pointcount=0;
    for (int j=1; j<=numhistos; j++){
      pointcount++;
      gpar2[i]->SetPoint(pointcount-1, hpar[i]->GetBinCenter(j), hpar[i]->GetBinContent(j));
    }
  }

  double mx0,my0,mx1,my1,sx0,sy0,sx1,sy1;
  gpar2[6]->GetPoint(0,mx0,my0);
  gpar2[7]->GetPoint(0,sx0,sy0);
  gpar2[8]->GetPoint(0,mx1,my1);
  gpar2[9]->GetPoint(0,sx1,sy1);
  if(my0 > my1){
    gpar2[6]->SetPoint( 0,mx1,my1 );
    gpar2[7]->SetPoint( 0,sx1,sy1 );
    gpar2[8]->SetPoint( 0,mx0,my0 );
    gpar2[9]->SetPoint( 0,sx0,sy0 );
  }
  gpar2[6]->GetPoint(4,mx0,my0);
  gpar2[7]->GetPoint(4,sx0,sy0);
  gpar2[8]->GetPoint(4,mx1,my1);
  gpar2[9]->GetPoint(4,sx1,sy1);
  if(my0 > my1){
    gpar2[6]->SetPoint( 4,mx1,my1 );
    gpar2[7]->SetPoint( 4,sx1,sy1 );
    gpar2[8]->SetPoint( 4,mx0,my0 );
    gpar2[9]->SetPoint( 4,sx0,sy0 );
  }
 
  TString parstr[2] = {"#mu", "#sigma"};
  TCanvas *c2 = new TCanvas("c2","c2",1500,1000);
  c2->Divide(2,6);
  for(int ipar=0; ipar<numpar; ipar++){

    c2->cd(ipar+1);
    gpar2[ipar]->SetMarkerColor(kBlack);

    gpar2[ipar]->Draw("AP");
    gpar2[ipar]->SetMarkerSize(4);
    gpar2[ipar]->SetMarkerStyle(2);
    gpar2[ipar]->GetXaxis()->SetTitle("Distance to PMT [m]");
    int gnum = ipar%2;

    TString name = Form("%s_%d",parstr[gnum].Data(),ipar/2);
    gpar2[ipar]->GetYaxis()->SetTitle(name);
    gpar2[ipar]->SetTitle(name);
    gpar2[ipar]->GetXaxis()->SetLimits(2,14);
    gpar2[ipar]->SetMinimum(mins[ipar]);
    gpar2[ipar]->SetMaximum(maxs[ipar]);
  }
  
}
