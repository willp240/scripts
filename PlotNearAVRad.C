#include <TH1D.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"
#include "TTree.h"
#include <algorithm>
#include <cctype>
#include <string>

#include <string>


void PlotNearAVRad(){

  gStyle->SetPalette(51);
  gStyle->SetLineWidth(2);
  gStyle->SetOptStat(0);

  TFile *fFile = TFile::Open("/home/parkerw/Scripts/Dec30_4to6m_NearAVTree.root");
  TTree* fTree = (TTree*)fFile->Get("eveTree")->Clone("fTree");

  std::string cut1 = "nearAV>0 && xNearAV > -999999 && nhits < 4500 && nhits > 0 ";
  std::string nhitname = "nhit0to5000";
  std::string nhittitle = "0 to 4500 Nhits";
  std::vector<std::string> coord{"X", "Y", "Z", "R", "D"};
  std::vector<std::string> axs{"xFit-xTrue", "yFit-yTrue", "zFit-zTrue", "-( (xFit-xTrue)*xFit + (yFit-yTrue)*yFit + (zFit-zTrue)*zFit ) / sqrt( xFit*xFit+yFit*yFit+zFit*zFit)", "sqrt( (xFit-xTrue)*(xFit-xTrue) + (yFit-yTrue)*(yFit-yTrue) + (zFit-zTrue)*(zFit-zTrue) )"};
   std::vector<std::string> nearAVaxs{"xNearAV-xTrue", "yNearAV-yTrue", "zNearAV+184.4-zTrue", "-( (xNearAV-xTrue)*xNearAV + (yNearAV-yTrue)*yNearAV + (zNearAV+184.4-zTrue)*zNearAV ) / sqrt( xNearAV*xNearAV+yNearAV*yNearAV+(zNearAV+184.4)*(zNearAV+184.4))", "sqrt( (xNearAV-xTrue)*(xNearAV-xTrue) + (yNearAV-yTrue)*(yNearAV-yTrue) + (zNearAV+184.4-zTrue)*(zNearAV+184.4-zTrue) )"};
   std::vector<std::string> combinedaxs{"xCombined-xTrue", "yCombined-yTrue", "zCombined-zTrue", "-( (xCombined-xTrue)*xCombined + (yCombined-yTrue)*yCombined + (zCombined-zTrue)*zCombined ) / sqrt( xCombined*xCombined+yCombined*yCombined+(zCombined)*(zCombined))", "sqrt( (xCombined-xTrue)*(xCombined-xTrue) + (yCombined-yTrue)*(yCombined-yTrue) + (zCombined-zTrue)*(zCombined-zTrue) )"};
   std::vector<std::string> labels{"Fit - True X, mm", "Fit - True Y, mm", "Fit - True Z, mm", "Radial Bias, mm", "Fit - Truth Distance, mm"};

  TCanvas* c4 = new TCanvas("c4", "c4", 1500,800);
  c4->SetRightMargin(0.13);
  c4->Divide(3,2);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  TCanvas* c5 = new TCanvas("c5", "c5", 1500,800);
  c5->SetRightMargin(0.13);
  c5->Divide(3,2);
  gPad->SetFrameLineWidth(2);
  gPad->SetGrid(1);

  
  for(int i=0; i<5; i++){

      TCanvas* c1 = new TCanvas("c1", "c1", 1500,800);
      c1->SetRightMargin(0.13);
      c1->Divide(3,2);
      gPad->SetFrameLineWidth(2);
      gPad->SetGrid(1);

      TCanvas* c2 = new TCanvas("c2", "c2", 1500,800);
      c2->SetRightMargin(0.13);
      c2->Divide(3,2);
      gPad->SetFrameLineWidth(2);
      gPad->SetGrid(1);

      TCanvas* c3 = new TCanvas("c3", "c3", 1500,800);
      c3->SetRightMargin(0.13);
      c3->Divide(3,2);
      gPad->SetFrameLineWidth(2);
      gPad->SetGrid(1);
      
      TGraphErrors* gmpdfbias = new TGraphErrors(5);
      gmpdfbias->SetTitle( (coord[i] + " Bias" ).c_str() );
      gmpdfbias->GetYaxis()->SetRangeUser(-30, 30);
      TGraphErrors* gnearavbias = new TGraphErrors(5);
      gnearavbias->SetTitle( (coord[i] + " Bias" ).c_str() );
      gnearavbias->GetYaxis()->SetRangeUser(-30, 30);
      TGraphErrors* gcombinedbias = new TGraphErrors(5);
      gcombinedbias->SetTitle( (coord[i] + " Bias" ).c_str() );
      gcombinedbias->GetYaxis()->SetRangeUser(-30, 30);
      
      TGraphErrors* gmpdfres = new TGraphErrors(5);
      gmpdfres->GetYaxis()->SetRangeUser(0, 400);
      gmpdfres->SetTitle( (coord[i] + " Resolution" ).c_str() );
      TGraphErrors* gnearavres = new TGraphErrors(5);
      gnearavres->SetTitle( (coord[i] + " Resolution" ).c_str() );
      gnearavres->GetYaxis()->SetRangeUser(0, 400);
      TGraphErrors* gcombinedres = new TGraphErrors(5);
      gcombinedres->SetTitle( (coord[i] + " Resolution" ).c_str() );
      gcombinedres->GetYaxis()->SetRangeUser(0, 400);
 
      for(int j=0; j<5; j++){

	c1->cd(j+1);
	gPad->SetFrameLineWidth(2);
	gPad->SetGrid(1);

	double rmin = 5400+(j*100);
	double rmax = 5400+(j+1)*100;

	TString rcut = Form("&& sqrt( xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) < %f && sqrt( xTrue*xTrue + yTrue*yTrue + zTrue*zTrue) > %f", rmax, rmin);
	TString hname = Form("r%fto%f_%s", rmin, rmax, nhitname.c_str());
	TString rtitle = Form("%.0f to %.0f mm", rmin, rmax);

	TH1F *htemp1;
	
	if(i==4)
	  htemp1 = new TH1F("htemp1","htemp1", 50, 0, 2000);
	else
	  htemp1 = new TH1F("htemp1","htemp1", 50, -1000, 1000);
	
	fTree->Draw( (axs.at(i)+">> htemp1").c_str(), (cut1).c_str()+rcut, "");
	htemp1->GetXaxis()->SetTitle(labels.at(i).c_str());
	htemp1->GetYaxis()->SetTitle("Events");
	htemp1->SetTitle("MultiPDF, " + rtitle);

	TF1 *f1 = new TF1("f1","gaus");
	f1->SetLineColor(kBlack);
	htemp1->Fit(f1);
	f1->Draw("same");

        gmpdfbias->SetPoint(j, rmin + (rmax-rmin)/2, f1->GetParameter(1));
        gmpdfbias->SetPointError(j, 50, f1->GetParError(1));
	gmpdfres->SetPoint(j, rmin + (rmax-rmin)/2, f1->GetParameter(2));
        gmpdfres->SetPointError(j, 50, f1->GetParError(2));
	
	// just repeat here for nearav with a different axs string
	c2->cd(j+1);
        gPad->SetFrameLineWidth(2);
        gPad->SetGrid(1);

	TH1F *htemp2;

        if(i==4)
          htemp2 = new TH1F("htemp2","htemp2", 50, 0, 2000);
        else
          htemp2 = new TH1F("htemp2","htemp2", 50, -1000, 1000);

        fTree->Draw( (nearAVaxs.at(i)+">> htemp2").c_str(), (cut1).c_str()+rcut, "");
        htemp2->GetXaxis()->SetTitle(labels.at(i).c_str());
        htemp2->GetYaxis()->SetTitle("Events");
        htemp2->SetTitle("NearAV, " + rtitle);

        TF1 *f2 = new TF1("f2","gaus");
        f2->SetLineColor(kRed);
        htemp2->Fit(f2);
        f2->Draw("same");

	gnearavbias->SetPoint(j, rmin + (rmax-rmin)/2, f2->GetParameter(1));
	gnearavbias->SetPointError(j, 50, f2->GetParError(1));
	gnearavres->SetPoint(j, rmin + (rmax-rmin)/2, f2->GetParameter(2));
        gnearavres->SetPointError(j, 50, f2->GetParError(2));

	// just repeat here for combined with a different axs string
        c3->cd(j+1);
        gPad->SetFrameLineWidth(2);
        gPad->SetGrid(1);

        TH1F *htemp3;

        if(i==4)
          htemp3 = new TH1F("htemp3","htemp3", 50, 0, 2000);
        else
          htemp3 = new TH1F("htemp3","htemp3", 50, -1000, 1000);

        fTree->Draw( (combinedaxs.at(i)+">> htemp3").c_str(), (cut1).c_str()+rcut, "");
        htemp3->GetXaxis()->SetTitle(labels.at(i).c_str());
        htemp3->GetYaxis()->SetTitle("Events");
        htemp3->SetTitle("Combined, " + rtitle);

        TF1 *f3 = new TF1("f3","gaus");
        f3->SetLineColor(kBlue);
        htemp3->Fit(f3);
        f3->Draw("same");

        gcombinedbias->SetPoint(j, rmin + (rmax-rmin)/2, f3->GetParameter(1));
        gcombinedbias->SetPointError(j, 50, f3->GetParError(1));
        gcombinedres->SetPoint(j, rmin + (rmax-rmin)/2, f3->GetParameter(2));
        gcombinedres->SetPointError(j, 50, f3->GetParError(2));

      }

      c1->SaveAs(("multipdf"+coord.at(i)+".pdf").c_str());
      c2->SaveAs(("nearav"+coord.at(i)+".pdf").c_str());
      c3->SaveAs(("combined"+coord.at(i)+".pdf").c_str());


      if(i < 3){

	c4->cd(i+1);
	gPad->SetFrameLineWidth(2);
	gPad->SetGrid(1);

	gmpdfbias->GetYaxis()->SetRangeUser(-100, 100);
	gmpdfbias->Draw("AP");
	gnearavbias->GetYaxis()->SetRangeUser(-100, 100);
	gnearavbias->SetLineColor(kRed);
	gnearavbias->Draw("Psame");
	gcombinedbias->GetYaxis()->SetRangeUser(-100, 100);
        gcombinedbias->SetLineColor(kBlue);
        gcombinedbias->Draw("Psame");
	
	TLegend* t1 = new TLegend( 0.15, 0.7, 0.35, 0.85 );
	t1->AddEntry( gmpdfbias, "MultiPDF", "l");
	t1->AddEntry( gnearavbias, "NearAV Angular", "l" );
	t1->AddEntry( gcombinedbias, "Combined", "l" );
	t1->Draw();

	c4->cd(3+i+1);
	gPad->SetFrameLineWidth(2);
        gPad->SetGrid(1);
	gmpdfres->GetYaxis()->SetRangeUser(0, 400);
	gmpdfres->Draw("AP");
	gnearavres->GetYaxis()->SetRangeUser(0, 400);
	gnearavres->SetLineColor(kRed);
	gnearavres->Draw("Psame");
	gcombinedres->GetYaxis()->SetRangeUser(0, 400);
        gcombinedres->SetLineColor(kBlue);
        gcombinedres->Draw("Psame");
	TLegend* t2 = new TLegend( 0.15, 0.7, 0.35, 0.85 );
	t2->AddEntry( gmpdfbias, "MultiPDF", "l");
	t2->AddEntry( gnearavbias, "NearAV Angular", "l" );
	t2->AddEntry( gcombinedbias, "Combined", "l" );
	t2->Draw();
      }
      else{

	c5->cd(i+1-3);
        gPad->SetFrameLineWidth(2);
        gPad->SetGrid(1);

	if(i==4)
	  gmpdfbias->GetYaxis()->SetRangeUser(0, 600);
	else
	  gmpdfbias->GetYaxis()->SetRangeUser(-100, 100);
	gmpdfbias->Draw("AP");
	if(i==4)
	  gnearavbias->GetYaxis()->SetRangeUser(0, 600);
	else
	  gnearavbias->GetYaxis()->SetRangeUser(-100, 100);
	gnearavbias->SetLineColor(kRed);
	gnearavbias->Draw("Psame");
	if(i==4)
          gcombinedbias->GetYaxis()->SetRangeUser(0, 600);
        else
          gcombinedbias->GetYaxis()->SetRangeUser(-100, 100);
        gcombinedbias->SetLineColor(kBlue);
        gcombinedbias->Draw("Psame");


        TLegend* t1 = new TLegend( 0.15, 0.7, 0.35, 0.85 );
        t1->AddEntry( gmpdfbias, "MultiPDF", "l");
        t1->AddEntry( gnearavbias, "NearAV Angular", "l" );
	t1->AddEntry( gcombinedbias, "Combined", "l" );
        t1->Draw();

	c5->cd(i+1);
        gPad->SetFrameLineWidth(2);
        gPad->SetGrid(1);
        gmpdfres->GetYaxis()->SetRangeUser(0, 400);
        gmpdfres->Draw("AP");
        gnearavres->GetYaxis()->SetRangeUser(0, 400);
	gnearavres->SetLineColor(kRed);
	gnearavres->Draw("Psame");
	gcombinedres->GetYaxis()->SetRangeUser(0, 400);
        gcombinedres->SetLineColor(kBlue);
        gcombinedres->Draw("Psame");
        TLegend* t2 = new TLegend( 0.15, 0.7, 0.35, 0.85 );
	t2->AddEntry( gmpdfbias, "MultiPDF", "l");
	t2->AddEntry( gnearavbias, "NearAV Angular", "l" );
	t2->AddEntry( gcombinedbias, "Combined", "l" );
	t2->Draw();
      }
  }

  c4->SaveAs((nhitname+"_xyz.pdf").c_str());
  c5->SaveAs((nhitname+"_rd.pdf").c_str());  

}

