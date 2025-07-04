#include <iostream>
#include <TLine.h>
#include <TStyle.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TAxis.h>


void MakeCanvas( std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, double[8], double[8], TLine*, std::string );

void CompareFitPerformance( std::string fname_1_2p5, std::string fname_2_2p5, std::string fname_3_2p5, std::string fname_4_2p5, std::string fname_5_2p5, std::string fname_6_2p5, std::string fname_7_2p5, std::string fname_1_1to10, std::string fname_2_1to10, std::string fname_3_1to10, std::string fname_4_1to10, std::string fname_5_1to10, std::string fname_6_1to10, std::string fname_7_1to10, std::string label1, std::string label2, std::string label3, std::string label4, std::string label5, std::string label6, std::string label7, std::string canv_name ) {
  
  std::string r_2p5_filename1 = fname_1_2p5 + "_r.root";
  std::string r_2p5_filename2 = fname_2_2p5 + "_r.root";
  std::string r_2p5_filename3 = fname_3_2p5 + "_r.root";
  std::string r_2p5_filename4 = fname_4_2p5 + "_r.root";
  std::string r_2p5_filename5 = fname_5_2p5 + "_r.root";
  std::string r_2p5_filename6 = fname_6_2p5 + "_r.root";
  std::string r_2p5_filename7 = fname_7_2p5 + "_r.root";
  
  std::string r_1to10_filename1 = fname_1_1to10 + "_r.root";
  std::string r_1to10_filename2 = fname_2_1to10 + "_r.root";
  std::string r_1to10_filename3 = fname_3_1to10 + "_r.root";
  std::string r_1to10_filename4 = fname_4_1to10 + "_r.root";
  std::string r_1to10_filename5 = fname_5_1to10 + "_r.root";
  std::string r_1to10_filename6 = fname_6_1to10 + "_r.root";
  std::string r_1to10_filename7 = fname_7_1to10 + "_r.root";
  std::string e_1to10_filename1 = fname_1_1to10 + "_e.root";
  std::string e_1to10_filename2 = fname_2_1to10 + "_e.root";
  std::string e_1to10_filename3 = fname_3_1to10 + "_e.root";
  std::string e_1to10_filename4 = fname_4_1to10 + "_e.root";
  std::string e_1to10_filename5 = fname_5_1to10 + "_e.root";
  std::string e_1to10_filename6 = fname_6_1to10 + "_e.root";
  std::string e_1to10_filename7 = fname_7_1to10 + "_e.root";
  
  
  double r_2p5_ymax[8] = {150, 700, 150, 700, 100, 700, 400, 700};
  double r_2p5_ymin[8] = {-20, 0, -20, 0, -100, 0, -100, 0};
  double x_2p5_ymax[8] = {100, 150, 30, 150, 30, 150, 100, 175};
  double x_2p5_ymin[8] = {-100, 0, -30, 0, -30, 0, -10, 0};
  double y_2p5_ymax[8] = {30, 150, 100, 150, 30, 150, 100, 175};
  double y_2p5_ymin[8] = {-30, 0, -100, 0, -30, 0, -10, 0};
  double z_2p5_ymax[8] = {30, 150, 30, 150, 100, 175, 100, 175};
  double z_2p5_ymin[8] = {-30, 0, -30, 0, -100, 0, -10, 0};

  double r_1to10_ymax[8] = {150, 700, 150, 700, 100, 700, 400, 700};
  double r_1to10_ymin[8] = {-20, 0, -20, 0, -100, 0, -50, 0};
  double z_1to10_ymax[8] = {30, 150, 30, 150, 175, 150, 100, 150};
  double z_1to10_ymin[8] = {-30, 0, -30, 0, -175, 0, -10, 0};
  double e_1to10_ymax[8] = {120, 700, 120, 700, 100, 700, 220, 700};
  double e_1to10_ymin[8] = {-20, 0, -20, 0, -100, 0, -100, 0};
 
  /*
  double r_2p5_ymax[8] = {50, 200, 50, 200, 50, 200, 200, 200};
  double r_2p5_ymin[8] = {-20, 0, -20, 0, -20, 0, -10, 0};
  double x_2p5_ymax[8] = {100, 150, 30, 150, 30, 150, 100, 175};
  double x_2p5_ymin[8] = {-100, 0, -30, 0, -30, 0, -10, 0};
  double y_2p5_ymax[8] = {30, 150, 100, 150, 30, 150, 100, 175};
  double y_2p5_ymin[8] = {-30, 0, -100, 0, -30, 0, -10, 0};
  double z_2p5_ymax[8] = {30, 150, 30, 150, 100, 175, 100, 175};
  double z_2p5_ymin[8] = {-30, 0, -30, 0, -100, 0, -10, 0};

  double r_1to10_ymax[8] = {50, 200, 50, 200, 50, 200, 200, 200};
  double r_1to10_ymin[8] = {-20, 0, -20, 0, -20, 0, -10, 0};
  double z_1to10_ymax[8] = {30, 150, 30, 150, 175, 150, 100, 150};
  double z_1to10_ymin[8] = {-30, 0, -30, 0, -175, 0, -10, 0};
  double e_1to10_ymax[8] = {50, 200, 50, 200, 50, 200, 100, 200};
  double e_1to10_ymin[8] = {-20, 0, -20, 0, -20, 0, -10, 0};
*/
  TLine *r_line = new TLine(0, 0, 6048, 0);
  TLine *xyz_line = new TLine(-6048, 0, 6048, 0);
  TLine *e_line = new TLine(1,0,10,0);

  std::string r_2p5_cname = canv_name + "_2p5_r.pdf";
  std::string r_1to10_cname = canv_name +"_1to10_r.pdf";
  std::string e_1to10_cname = canv_name +"_1to10_e.pdf";

  MakeCanvas( r_2p5_filename1, r_2p5_filename2, r_2p5_filename3, r_2p5_filename4, r_2p5_filename5, r_2p5_filename6, r_2p5_filename7,  label1, label2, label3, label4, label5, label6, label7, r_2p5_ymax, r_2p5_ymin, r_line, r_2p5_cname);
  MakeCanvas( r_1to10_filename1, r_1to10_filename2, r_1to10_filename3, r_1to10_filename4, r_1to10_filename5, r_1to10_filename6, r_1to10_filename7, label1, label2, label3, label4, label5, label6, label7, r_1to10_ymax, r_1to10_ymin, r_line, r_1to10_cname);
  MakeCanvas( e_1to10_filename1, e_1to10_filename2, e_1to10_filename3, e_1to10_filename4, e_1to10_filename5, e_1to10_filename6, e_1to10_filename7, label1, label2, label3, label4, label5, label6, label7, e_1to10_ymax, e_1to10_ymin, e_line, e_1to10_cname);

}


void MakeCanvas( std::string filename1, std::string filename2, std::string filename3, std::string filename4, std::string filename5, std::string filename6, std::string filename7, std::string label1, std::string label2, std::string label3, std::string label4, std::string label5, std::string label6, std::string label7, double ymax[8], double ymin[8], TLine* line, std::string canvas_name ) {

  std::cout << filename1 << std::endl;
  std::cout << filename2 << std::endl;
  std::cout << filename3 << std::endl;
  std::cout << filename4 << std::endl;
  std::cout << filename5 << std::endl;
  std::cout << filename6 << std::endl;
  std::cout << filename7 << std::endl;
  
  gStyle->SetGridStyle(3);

  std::string coord[4] = {"x", "y", "z", "r"};
  std::string title[2] = {"bias", "resolution"};

  TFile *_f1 = TFile::Open(filename1.c_str());
  TFile *_f2  = TFile::Open(filename2.c_str());
  TFile *_f3  = TFile::Open(filename3.c_str());
  TFile *_f4  = TFile::Open(filename4.c_str());
  TFile *_f5  = TFile::Open(filename5.c_str());
  TFile *_f6  = TFile::Open(filename6.c_str());
  TFile *_f7  = TFile::Open(filename7.c_str());
  
  TCanvas* c1 = new TCanvas("c1", "c1", 1500,700);
  c1->Divide(4,2,0.01,0.05);

  for(int j=0; j<2; j++){
    for(int i=0; i<4; i++){

      std::string gname = coord[i] + "_" + title[j];
      std::cout << "getting " << gname << " " << 3*j+i+1 << std::endl;

      TGraphErrors* h1 = (TGraphErrors*)_f1->Get(gname.c_str())->Clone();
      TGraphErrors* h2 = (TGraphErrors*)_f2->Get(gname.c_str())->Clone();
      TGraphErrors* h3 = (TGraphErrors*)_f3->Get(gname.c_str())->Clone();
      TGraphErrors* h4 = (TGraphErrors*)_f4->Get(gname.c_str())->Clone();
      TGraphErrors* h5 = (TGraphErrors*)_f5->Get(gname.c_str())->Clone();
      TGraphErrors* h6 = (TGraphErrors*)_f6->Get(gname.c_str())->Clone();
      TGraphErrors* h7 = (TGraphErrors*)_f7->Get(gname.c_str())->Clone();
      
      h1->SetLineColor(kBlue+2);
      h2->SetLineColor(kRed+2);
      h3->SetLineColor(kBlack);
      h4->SetLineColor(kGreen+2);
      h5->SetLineColor(kYellow+2);
      h6->SetLineColor(kPink+2);
      h7->SetLineColor(kOrange+2);
      h1->SetLineWidth(1);
      h2->SetLineWidth(1);
      h3->SetLineWidth(1);
      h4->SetLineWidth(1);
      h5->SetLineWidth(1);
      h6->SetLineWidth(1);
      h7->SetLineWidth(1);

      TLegend* t1 = new TLegend( 0.4, 0.55, 0.9, 0.9 );
      t1->AddEntry( h1, label1.c_str(), "l" );
      t1->AddEntry( h2, label2.c_str(), "l" );
      t1->AddEntry( h3, label3.c_str(), "l" );
      t1->AddEntry( h4, label4.c_str(), "l" );
      t1->AddEntry( h5, label5.c_str(), "l" );
      t1->AddEntry( h6, label6.c_str(), "l" );
      t1->AddEntry( h7, label7.c_str(), "l" );

      h1->GetYaxis()->SetRangeUser(ymin[2*i+j],ymax[2*i+j]);
      title[j][0] = toupper(title[j][0]);
      coord[i][0] = toupper(coord[i][0]);
      gname = coord[i] + " " + title[j];
      h1->SetTitle(gname.c_str());
      title[j][0] = tolower(title[j][0]);
      coord[i][0] = tolower(coord[i][0]);

      c1->cd(4*j + i + 1);
      gPad->SetGrid(1);

      h1->Draw("ap");
      h2->Draw("same p");
      h3->Draw("same p");
      h4->Draw("same p");
      h5->Draw("same p");
      h6->Draw("same p");
      h7->Draw("same p");
      t1->Draw("same");

      if(j==0)
	line->Draw("same");
      else
	h1->GetYaxis()->SetTitleOffset(1.2);
      
    }
  }

  c1->SaveAs(canvas_name.c_str());
}
