#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"
#include <TMath.h>

#include <string>
using namespace std;

TH1D* FitPDF3MultPDF( )
{
  gStyle->SetOptStat(0);
  TFile *_fileAll = TFile::Open("timeresid1m.root");
  int numhistos = 1;

  TH1D* histvec[6];
  
  TH1D* hpar0 = new TH1D("hpar0","hpar0", 6,2,14);
  TH1D* hpar1 =new TH1D("hpar1","hpar1", 6,2,14);
  TH1D* hpar2 =new TH1D("hpar2","hpar2", 6,2,14);

  double max1[6] = {2, 0, 0, 3, 0, 0};
  double min1[6] = {-20, -20, -10, -10, -20, -20};
  double max2[6] = {8, 10, 6, 6, 10, 10};
  double min2[6] = {2, 0, 0, 3, 0, 0};
  double max3[6] = {25, 25, 21, 25, 25, 25};
  double min3[6] = {8, 10, 6, 6, 10, 10};
  //double max4[6] = {25, 100, 100, 100, 100, 100};
  //double min4[6] = {17, 25, 25, 25, 25, 25};
  double max4[6] = {100, 100, 100, 100, 100, 100};
  double min4[6] = {25, 25, 21, 25, 25, 25};

  TF1 *ffit[6];

  for(int ihist = 3; ihist < 4; ihist++){
    //push back histo

    TString hname = Form("h%d",ihist);
    TH1D* h = (TH1D*) _fileAll->Get(hname);
    histvec[ihist] = h;
    //    histvec[ihist]->GetXaxis()->SetRangeUser(-5,30);
    
    //    TF1 *f = new TF1("f", "[0]*exp(-0.5* (x-[1])*(x-[1])/([2]*[2])) + [3]*exp(-0.5* (x-[4])*(x-[4])/([5]*[5])) + [6]*exp(-0.5* (x-[7])*(x-[7])/([8]*[8])) ");
    TF1 *f = new TF1("f", "gaus(0) + gaus(3) + gaus(6) + gaus(9)", -10, 100);

    TF1 *f1 = new TF1("f1", "gaus", min1[ihist],max1[ihist]);
    TF1 *f2 = new TF1("f2", "gaus", min2[ihist],max2[ihist]);
    TF1 *f3 = new TF1("f3", "gaus", min3[ihist],max3[ihist]);
    TF1 *f4 = new TF1("f4", "gaus", min4[ihist],max4[ihist]);
    //  TF1 *f5 = new TF1("f5", "gaus", min5[ihist],max5[ihist]);
   
    f->SetLineColor(h->GetLineColor());
    f1->SetLineColor(kGreen);
    f2->SetLineColor(kBlue);
    f3->SetLineColor(kRed);
    f4->SetLineColor(kYellow);
    //f5->SetLineColor(kMagenta); 

    histvec[ihist]->Fit("f1", "R");
    histvec[ihist]->Fit("f2", "R+");
    histvec[ihist]->Fit("f3", "R+");
    histvec[ihist]->Fit("f4", "R+");
    //histvec[ihist]->Fit("f5", "R+"); 

    double par[15];
    
    f1->GetParameters(&par[0]);
    f2->GetParameters(&par[3]);
    f3->GetParameters(&par[6]);
    f4->GetParameters(&par[9]);
    //f5->GetParameters(&par[12]);

    f->SetParameters(par);

    ffit[ihist] = f;    

    histvec[ihist]->Fit("f", "MILR+");
    
    for(int i =0; i<12; i++){
      std::cout << par[i] << std::endl;
    }

    ffit[ihist]->SetRange(-20,100);
    histvec[ihist]->GetXaxis()->SetRangeUser(-20,100);
    histvec[ihist]->GetYaxis()->SetRangeUser(0,0.1);

    hpar0->SetBinContent(ihist+1, f->GetParameter(0));
    hpar1->SetBinContent(ihist+1, f->GetParameter(1));
    hpar2->SetBinContent(ihist+1, f->GetParameter(2));
  }

  TCanvas *c1 = new TCanvas("c1","c1",1500,700);
  TLegend* t1 = new TLegend( 0.5, 0.6, 0.88, 0.88);

  //c1->Divide(3,2);

  for(int ihist = 3; ihist < 4; ihist++){
    //    c1->cd(ihist+1);
    //Fill histos with param values for median distance
    if(ihist == 0){
      histvec[ihist]->Draw();
      ffit[ihist]->Draw("same");
    }
    else{
      histvec[ihist]->Draw("");
      ffit[ihist]->Draw("same");
    }
    t1->AddEntry( histvec[ihist], Form("%dm < d < %dm", (2*ihist)+2, (2*ihist)+4), "l" );
  }

  t1->Draw();

  hpar0->SetMarkerColor(kBlack);
  hpar1->SetMarkerColor(kBlue);
  hpar2->SetMarkerColor(kRed);

  /*  
  hpar0->Draw("P");
  hpar0->SetMarkerSize(4);
  hpar0->SetMarkerStyle(2);
  hpar0->GetXaxis()->SetTitle("Distance to PMT [m]");
  hpar0->GetYaxis()->SetTitle("#lambda");
  TCanvas *c2 = new TCanvas("c2","c2",800,600);
  hpar1->Draw("P");
  hpar1->SetMarkerSize(4);
  hpar1->SetMarkerStyle(2);
  hpar1->GetXaxis()->SetTitle("Distance to PMT [m]");
  hpar1->GetYaxis()->SetTitle("#mu");
  TCanvas *c3 = new TCanvas("c3","c3",800,600);
  hpar2->Draw("P");
  hpar2->SetMarkerSize(4);
  hpar2->SetMarkerStyle(2);
  hpar2->GetXaxis()->SetTitle("Distance to PMT [m]");
  hpar2->GetYaxis()->SetTitle("#sigma");
  */
}
