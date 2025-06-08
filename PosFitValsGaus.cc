#include <iostream>
#include <TLine.h>
#include <TStyle.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TAxis.h>
#include <TH1D.h>

void MakeCanvas( std::string, std::string, std::string, std::string, std::string, int );

void PosFitValsGaus( std::string fname_orig_2p5, std::string fname_new_2p5, std::string fname_orig_1to10, std::string fname_new_1to10, std::string orig_label, std::string new_label, std::string canv_name ) {

  gStyle->SetOptStat(0);
  
  std::string orig_r_2p5_filename = fname_orig_2p5 + "_r.root";
  std::string new_r_2p5_filename  = fname_new_2p5 + "_r.root";
  std::string orig_x_2p5_filename = fname_orig_2p5 + "_x.root";
  std::string new_x_2p5_filename  = fname_new_2p5 + "_x.root";
  std::string orig_y_2p5_filename = fname_orig_2p5 + "_y.root";
  std::string new_y_2p5_filename  = fname_new_2p5 + "_y.root";
  std::string orig_z_2p5_filename = fname_orig_2p5 + "_z.root";
  std::string new_z_2p5_filename  = fname_new_2p5 + "_z.root";
  
  std::string orig_r_1to10_filename = fname_orig_1to10 + "_r.root";
  std::string new_r_1to10_filename  = fname_new_1to10 + "_r.root";
  std::string orig_x_1to10_filename = fname_orig_1to10 + "_x.root";
  std::string new_x_1to10_filename  = fname_new_1to10 + "_x.root";
  std::string orig_y_1to10_filename = fname_orig_1to10 + "_y.root";
  std::string new_y_1to10_filename  = fname_new_1to10 + "_y.root";
  std::string orig_z_1to10_filename = fname_orig_1to10 + "_z.root";
  std::string new_z_1to10_filename  = fname_new_1to10 + "_z.root";
  std::string orig_e_1to10_filename = fname_orig_1to10 + "_e.root";
  std::string new_e_1to10_filename  = fname_new_1to10 + "_e.root";

  // prolly delete these
  /*
  double r_2p5_ymax[10] = {30, 100, 30, 100, 30, 100, 60, 100, 200, 100};
  double r_2p5_ymin[10] = {-30, 0, -30, 0, -30, 0, -10, 0, 0, 0};
  double x_2p5_ymax[10] = {50, 100, 30, 100, 30, 100, 60, 100, 200, 100};
  double x_2p5_ymin[10] = {-50, 0, -30, 0, -30, 0, -10, 0, 0, 0};
  double y_2p5_ymax[10] = {30, 100, 50, 100, 30, 100, 60, 100, 200, 100};
  double y_2p5_ymin[10] = {-30, 0, -50, 0, -30, 0, -10, 0, 0, 0};
  double z_2p5_ymax[10] = {30, 100, 30, 100, 50, 100, 60, 100, 200, 100};
  double z_2p5_ymin[10] = {-30, 0, -30, 0, -50, 0, -10, 0, 0, 0};
  
  double r_1to10_ymax[10] = {30, 100, 30, 100, 30, 100, 100, 100, 200, 100};
  double r_1to10_ymin[10] = {-30, 0, -30, 0, -30, 0, -10, 0, 0, 0};
  double x_1to10_ymax[10] = {100, 100, 30, 100, 30, 100, 100, 100, 200, 100};
  double x_1to10_ymin[10] = {-100, 0, -30, 0, -30, 0, -10, 0, 0, 0};
  double y_1to10_ymax[10] = {30, 100, 100, 100, 30, 100, 100, 100, 200, 100};
  double y_1to10_ymin[10] = {-30, 0, -100, 0, -30, 0, -10, 0, 0, 0};
  double z_1to10_ymax[10] = {30, 100, 30, 100, 100, 100, 100, 100, 200, 100};
  double z_1to10_ymin[10] = {-30, 0, -30, 0, -100, 0, -10, 0, 0, 0};
  double e_1to10_ymax[10] = {30, 100, 30, 100, 30, 100, 100, 100, 200, 100};
  double e_1to10_ymin[10] = {-30, 0, -30, 0, -30, 0, -10, 0, 0, 0};
  */

  std::string r_2p5_cname = canv_name + "_2p5_r_";
  std::string x_2p5_cname = canv_name + "_2p5_x_";
  std::string y_2p5_cname = canv_name + "_2p5_y_";
  std::string z_2p5_cname = canv_name + "_2p5_z_";
  std::string r_1to10_cname = canv_name +"_1to10_r_";
  std::string x_1to10_cname = canv_name +"_1to10_x_";
  std::string y_1to10_cname = canv_name +"_1to10_y_";
  std::string z_1to10_cname = canv_name +"_1to10_z_";
  std::string e_1to10_cname = canv_name +"_1to10_e_";


  MakeCanvas( orig_r_2p5_filename, new_r_2p5_filename, orig_label, new_label, r_2p5_cname, 15);
  //MakeCanvas( orig_x_2p5_filename, new_x_2p5_filename, orig_label, new_label, x_2p5_cname, 30);
  //MakeCanvas( orig_y_2p5_filename, new_y_2p5_filename, orig_label, new_label, y_2p5_cname, 30);
  //MakeCanvas( orig_z_2p5_filename, new_z_2p5_filename, orig_label, new_label, z_2p5_cname, 30);

  MakeCanvas( orig_r_1to10_filename, new_r_1to10_filename, orig_label, new_label, r_1to10_cname, 15);
  //MakeCanvas( orig_x_1to10_filename, new_x_1to10_filename, orig_label, new_label, x_1to10_cname, 30);
  //MakeCanvas( orig_y_1to10_filename, new_y_1to10_filename, orig_label, new_label, y_1to10_cname, 30);
  //MakeCanvas( orig_z_1to10_filename, new_z_1to10_filename, orig_label, new_label, z_1to10_cname, 30);
  MakeCanvas( orig_e_1to10_filename, new_e_1to10_filename, orig_label, new_label, e_1to10_cname, 20);

}


void MakeCanvas( std::string orig_filename, std::string new_filename, std::string orig_label, std::string new_label, std::string canvas_name, int numhists ) {

  std::cout << orig_filename << std::endl;
  std::cout << new_filename  << std::endl;

  gStyle->SetGridStyle(3);

  std::vector<std::string> coords = {"x", "y", "z", "r", "d"};

  TFile *_orig = TFile::Open(orig_filename.c_str());
  TFile *_new  = TFile::Open(new_filename.c_str());

  for(size_t icoord = 0; icoord < coords.size(); icoord++ )
    {
      
      TString cname1 = Form("c_%s1", coords.at(icoord).c_str());
      TCanvas* c1 = new TCanvas(cname1, cname1, 1500, 800);
      std::string canvname1 = canvas_name + coords.at(icoord) + "1.pdf";
      c1->Divide(5,3);
      
      TString cname2 = Form("c_%s2", coords.at(icoord).c_str());
      TCanvas* c2 = new TCanvas(cname2, cname2, 1500, 800);
      std::string canvname2 = canvas_name + coords.at(icoord) + "2.pdf";
      c2->Divide(5,3);
     
      for(int ihist = 0; ihist < numhists; ihist++)
	{
	  if(ihist < 15)
	    c1->cd(ihist+1);
	  else
	    c2->cd(ihist+1-15);
	  gPad->SetLogy();
	  gPad->SetGrid(1);

	  TString hname = Form("%s_%d", coords.at(icoord).c_str(), ihist);
	  std::cout << "getting " << hname << " " << numhists << std::endl;
	  TH1D* hOrig = (TH1D*)_orig->Get(hname);
	  hOrig->SetName("horig");
	  
	  TH1D* hNew = (TH1D*)_new->Get(hname);
	  hNew->SetName("hnew");

	  hOrig->SetLineColor(kRed);
	  hNew->SetLineColor(kBlue);
	  hOrig->GetYaxis()->SetRangeUser( 0.1, hOrig->GetMaximum()*100 );
	  //hOrig->GetXaxis()->SetRangeUser( -1000, 1000 );
	  hOrig->GetXaxis()->SetRangeUser( -3000, 3000 );
	  hNew->SetLineStyle(2);

	  hOrig->Draw("hist");
	  hNew->Draw("hist same");

	  TLegend* t1 = new TLegend( 0.15, 0.75, 0.7, 0.9 );
	  t1->AddEntry( hOrig, orig_label.c_str(), "l" );
	  t1->AddEntry( hNew, new_label.c_str(), "l" );
	  t1->Draw("same");
	}
      
      c1->SaveAs(canvname1.c_str());
      if(numhists > 15)
	c2->SaveAs(canvname2.c_str());
      delete c1;
      delete c2;
    }

}
