#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"

#include <string>
using namespace std;

TH1D* FitPDFFlour( )
{
  gStyle->SetOptStat(0);
  TFile *_fileAll = TFile::Open("timeresid.root");
  int numhistos = 6;

  //declare histo vector
  TH1D* histvec[6];

  //  TH1D* h = (TH1D*) _fileAll->Get("h4");
  //h->GetXaxis()->SetRangeUser(-5,100);
  //h->Fit("landau", "MIL");
  //h->Draw();

  TH1D* hpar0 = new TH1D("hpar0","hpar0", 6,2,14);
  TH1D* hpar1 =new TH1D("hpar1","hpar1", 6,2,14);
  TH1D* hpar2 =new TH1D("hpar2","hpar2", 6,2,14);
  TF1* ffit[6];// = new TF1("f1", "([0]*(1 - exp(-x*[1]))) * exp(-x*[2])");

  //Loop over histos
  for(int ihist = 0; ihist < numhistos; ihist++){
         //push back histo
    TString hname = Form("h%d",ihist);
    TH1D* h = (TH1D*) _fileAll->Get(hname);
    histvec[ihist] = h;
    histvec[ihist]->GetXaxis()->SetRangeUser(-5,30);
    //    TF1 *f = new TF1("f", "[0] * (exp(-x/[1]) - exp(-x/[2])) / ([1] - [2])");
    TF1 *f = new TF1("f", "(([0]) * (exp(-x/[1]) - exp(-x/[2])) / ([1]-[2])) ");//* [3]*exp(-0.5*((x-[4])/[5])^2)");
    //f->SetParameter(0,0.295);
    //f->SetParameter(1,4.36);
    //f->SetParameter(2,10.5);
    f->SetParameter(0,0.821);
    f->SetParameter(1,11.13);
    f->SetParameter(2,0.101);

    f->SetLineColor(h->GetLineColor());
    
    TF1 *f1 = new TF1("f1","gaus");

    TF1 *f2 = new TF1("f2","f*f1");

    histvec[ihist]->Fit("f", "ML");
    
    ffit[ihist] = f;
    ffit[ihist]->SetRange(-200,5000);
    histvec[ihist]->GetXaxis()->SetRangeUser(-200,500);    
    hpar0->SetBinContent(ihist+1, f->GetParameter(0));
    hpar1->SetBinContent(ihist+1, f->GetParameter(1));
    hpar2->SetBinContent(ihist+1, f->GetParameter(2));
    
  }

  TCanvas *c1 = new TCanvas("c1","c1",800,600);
  TLegend* t1 = new TLegend( 0.5, 0.6, 0.88, 0.88);

  for(int ihist = 0; ihist < numhistos; ihist++){
    //Fill histos with param values for median distance
    if(ihist == 0){
      histvec[ihist]->Draw();
      ffit[ihist]->Draw("same");
    }
    else{
      ffit[ihist]->Draw("same");
      histvec[ihist]->Draw("same");
    }
    t1->AddEntry( histvec[ihist], Form("%dm < d < %dm", (2*ihist)+2, (2*ihist)+4), "l" );
  }

  t1->Draw();

  hpar0->SetMarkerColor(kBlack);
  hpar1->SetMarkerColor(kBlue);
  hpar2->SetMarkerColor(kRed);
  
  hpar0->Draw("P");
  hpar0->SetMarkerSize(4);
  hpar0->SetMarkerStyle(2);
  hpar0->GetXaxis()->SetTitle("Distance to PMT [m]");
  hpar0->GetYaxis()->SetTitle("A");
  TCanvas *c2 = new TCanvas("c2","c2",800,600);
  hpar1->Draw("P");
  hpar1->SetMarkerSize(4);
  hpar1->SetMarkerStyle(2);
  hpar1->GetXaxis()->SetTitle("Distance to PMT [m]");
  hpar1->GetYaxis()->SetTitle("B");
  TCanvas *c3 = new TCanvas("c3","c3",800,600);
  hpar2->Draw("P");
  hpar2->SetMarkerSize(4);
  hpar2->SetMarkerStyle(2);
  hpar2->GetXaxis()->SetTitle("Distance to PMT [m]");
  hpar2->GetYaxis()->SetTitle("C");
}
