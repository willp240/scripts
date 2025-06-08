#include <iostream>
#include <TLine.h>
#include <TStyle.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TAxis.h>


void MakeCanvas( std::string, std::string, std::string, std::string, double[8], double[8], TLine*, std::string );

void PosFitVals( std::string fname_orig_2p5, std::string fname_new_2p5, std::string fname_orig_1to10, std::string fname_new_1to10, std::string orig_label, std::string new_label, std::string canv_name ) {

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

  // e-
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
  double e_1to10_ymin[10] = {-30, 0, -30, 0, -30, 0, -10, 0, 0, 0};*/

  // alpha
  double r_2p5_ymax[10] = {50, 400, 50, 400, 50, 400, 250, 400, 600, 400};
  double r_2p5_ymin[10] = {-50, 0, -50, 0, -50, 0, -20, 0, 0, 0};
 
  double r_1to10_ymax[10] = {50, 350, 50, 350, 50, 350, 200, 400, 400, 400};
  double r_1to10_ymin[10] = {-50, 0, -50, 0, -50, 0, -20, 0, 0, 0};
  double e_1to10_ymax[10] = {50, 700, 50, 700, 50, 700, 300, 800, 1000, 700};
  double e_1to10_ymin[10] = {-50, 0, -50, 0, -50, 0, -20, 0, 0, 0};

  
  TLine *r_line = new TLine(0, 0, 6048, 0);
  TLine *xyz_line = new TLine(-6048, 0, 6048, 0);
  TLine *e_line = new TLine(1,0,10,0);

  std::string r_2p5_cname = canv_name + "_2p5_r";
  std::string x_2p5_cname = canv_name + "_2p5_x";
  std::string y_2p5_cname = canv_name + "_2p5_y";
  std::string z_2p5_cname = canv_name +"_2p5_z";
  std::string r_1to10_cname = canv_name +"_1to10_r";
  std::string x_1to10_cname = canv_name +"_1to10_x";
  std::string y_1to10_cname = canv_name +"_1to10_y";
  std::string z_1to10_cname = canv_name +"_1to10_z";
  std::string e_1to10_cname = canv_name +"_1to10_e";

  MakeCanvas( orig_r_2p5_filename, new_r_2p5_filename, orig_label, new_label, r_2p5_ymax, r_2p5_ymin, r_line, r_2p5_cname);
  //MakeCanvas( orig_x_2p5_filename, new_x_2p5_filename, orig_label, new_label, x_2p5_ymax, x_2p5_ymin, xyz_line, x_2p5_cname);
  //MakeCanvas( orig_y_2p5_filename, new_y_2p5_filename, orig_label, new_label, y_2p5_ymax, y_2p5_ymin, xyz_line, y_2p5_cname);
  //MakeCanvas( orig_z_2p5_filename, new_z_2p5_filename, orig_label, new_label, z_2p5_ymax, z_2p5_ymin, xyz_line, z_2p5_cname);
  MakeCanvas( orig_r_1to10_filename, new_r_1to10_filename, orig_label, new_label, r_1to10_ymax, r_1to10_ymin, r_line, r_1to10_cname);
  //MakeCanvas( orig_x_1to10_filename, new_x_1to10_filename, orig_label, new_label, x_1to10_ymax, x_1to10_ymin, xyz_line, x_1to10_cname);
  //MakeCanvas( orig_y_1to10_filename, new_y_1to10_filename, orig_label, new_label, y_1to10_ymax, y_1to10_ymin, xyz_line, y_1to10_cname);
  //MakeCanvas( orig_z_1to10_filename, new_z_1to10_filename, orig_label, new_label, z_1to10_ymax, z_1to10_ymin, xyz_line, z_1to10_cname);
  MakeCanvas( orig_e_1to10_filename, new_e_1to10_filename, orig_label, new_label, e_1to10_ymax, e_1to10_ymin, e_line, e_1to10_cname);

}


void MakeCanvas( std::string orig_filename, std::string new_filename, std::string orig_label, std::string new_label, double ymax[8], double ymin[8], TLine* line, std::string canvas_name ) {

  std::cout << orig_filename << std::endl;
  std::cout << new_filename  << std::endl;

  gStyle->SetGridStyle(3);

  std::string coord[5] = {"x", "y", "z", "r", "d"};
  std::string title[2] = {"bias", "resolution"};
  std::string dtitle[2] = {"mean", "width"};

  TFile *_orig = TFile::Open(orig_filename.c_str());
  TFile *_new  = TFile::Open(new_filename.c_str());

  TCanvas* c1 = new TCanvas("c1", "c1", 1125,700);
  c1->Divide(3,2,0.01,0.05);
  TCanvas* c2 = new TCanvas("c2", "c2", 750,700);
  c2->Divide(2,2,0.01,0.05);
  std::string canvname1 = canvas_name+"_xyz.pdf";
  std::string canvname2 = canvas_name+"_rd.pdf";

  for(int j=0; j<2; j++){
    for(int i=0; i<5; i++){

      std::string gname;
      if(i < 4)
	gname = coord[i] + "_" + title[j];
      else
	gname = coord[i] + "_" + dtitle[j];
      std::cout << "getting " << gname << " " << 3*j+i+1 << std::endl;

      TGraphErrors* horig = (TGraphErrors*)_orig->Get(gname.c_str())->Clone();
      TGraphErrors* hnew = (TGraphErrors*)_new->Get(gname.c_str())->Clone();

      horig->SetLineColor(kBlue+2);
      hnew->SetLineColor(kRed+2);
      horig->SetLineWidth(1);
      hnew->SetLineWidth(1);

      TLegend* t1 = new TLegend( 0.1, 0.75, 0.6, 0.9 );
      t1->AddEntry( horig, orig_label.c_str(), "l" );
      t1->AddEntry( hnew, new_label.c_str(), "l" );

      horig->GetYaxis()->SetRangeUser(ymin[2*i+j],ymax[2*i+j]);
      title[j][0] = toupper(title[j][0]);
      coord[i][0] = toupper(coord[i][0]);
      gname = coord[i] + " " + title[j];
      horig->SetTitle(gname.c_str());
      title[j][0] = tolower(title[j][0]);
      coord[i][0] = tolower(coord[i][0]);

      if(i < 3)
        c1->cd(3*j + i + 1);
      else
        c2->cd(2*j + (i-3) + 1);

      gPad->SetGrid(1);
      
      horig->Draw("ap");
      hnew->Draw("same p");
      t1->Draw("same");

      if(j==0)
	{
	  std::cout << "drawing line for " << gname << std::endl;
	  line->Draw("same");
	  std::cout << "drew " << std::endl;
	}
      else
	horig->GetYaxis()->SetTitleOffset(1.2);

    }
  }

  c1->SaveAs(canvname1.c_str());
  c2->SaveAs(canvname2.c_str());

  delete c1;
  delete c2;
}
