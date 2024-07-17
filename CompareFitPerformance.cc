#include <iostream>
#include <TLine.h>
#include <TStyle.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TAxis.h>


void MakeCanvas( std::string, std::string, std::string, std::string, double[8], double[8], TLine*, std::string );

void CompareFitPerformance( std::string fname_orig_2p5, std::string fname_new_2p5, std::string fname_orig_1to10, std::string fname_new_1to10, std::string orig_label, std::string new_label, std::string canv_name ) {
  /*
  std::string orig_r_2p5_filename = fname_orig_2p5 + "_R_Rlt6m.root";
  std::string new_r_2p5_filename  = fname_new_2p5 + "_r.root";
  std::string orig_x_2p5_filename = fname_orig_2p5 + "_X_Rlt6m.root";
  std::string new_x_2p5_filename  = fname_new_2p5 + "_x.root";
  std::string orig_y_2p5_filename = fname_orig_2p5 + "_Y_Rlt6m.root";
  std::string new_y_2p5_filename  = fname_new_2p5 + "_y.root";
  std::string orig_z_2p5_filename = fname_orig_2p5 + "_Z_Rlt6m.root";
  std::string new_z_2p5_filename  = fname_new_2p5 + "_z.root";

  std::string orig_r_1to10_filename = fname_orig_1to10 + "_R_Rlt6m.root";
  std::string new_r_1to10_filename  = fname_new_1to10 + "_r.root";
  std::string orig_x_1to10_filename = fname_orig_1to10 + "_X_Rlt6m.root";
  std::string new_x_1to10_filename  = fname_new_1to10 + "_x.root";
  std::string orig_y_1to10_filename = fname_orig_1to10 + "_Y_Rlt6m.root";
  std::string new_y_1to10_filename  = fname_new_1to10 + "_y.root";
  std::string orig_z_1to10_filename = fname_orig_1to10 + "_Z_Rlt6m.root";
  std::string new_z_1to10_filename  = fname_new_1to10 + "_z.root";
  std::string orig_e_1to10_filename = fname_orig_1to10 + "_E_Rlt4m.root";
  std::string new_e_1to10_filename  = fname_new_1to10 + "_e.root";
  */
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

  // Fit Perf FV 5.5 (4m for E), Axis Truth
  double r_2p5_ymax[8] = {30, 150, 30, 150, 30, 150, 100, 150};
  double r_2p5_ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};
  double x_2p5_ymax[8] = {100, 150, 30, 150, 30, 150, 100, 175};
  double x_2p5_ymin[8] = {-100, 0, -30, 0, -30, 0, -10, 0};
  double y_2p5_ymax[8] = {30, 150, 100, 150, 30, 150, 100, 175};
  double y_2p5_ymin[8] = {-30, 0, -100, 0, -30, 0, -10, 0};
  double z_2p5_ymax[8] = {30, 150, 30, 150, 100, 175, 100, 175};
  double z_2p5_ymin[8] = {-30, 0, -30, 0, -100, 0, -10, 0};

  double r_1to10_ymax[8] = {30, 200, 30, 200, 30, 200, 100, 150};
  double r_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};
  double x_1to10_ymax[8] = {30, 150, 30, 150, 175, 150, 100, 150};
  double x_1to10_ymin[8] = {-30, 0, -30, 0, -175, 0, -10, 0};
  double y_1to10_ymax[8] = {30, 150, 30, 150, 175, 150, 100, 150};
  double y_1to10_ymin[8] = {-30, 0, -30, 0, -175, 0, -10, 0};
  double z_1to10_ymax[8] = {30, 150, 30, 150, 175, 150, 100, 150};
  double z_1to10_ymin[8] = {-30, 0, -30, 0, -175, 0, -10, 0};
  double e_1to10_ymax[8] = {30, 150, 30, 150, 30, 150, 40, 150};
  double e_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};

  // Fit Perf FV 5.5, Axis Truth
  /*  double r_2p5_ymax[8] = {30, 150, 30, 150, 30, 150, 60, 150};
  double r_2p5_ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};
  double x_2p5_ymax[8] = {100, 150, 30, 150, 30, 150, 100, 175};
  double x_2p5_ymin[8] = {-100, 0, -30, 0, -30, 0, -10, 0};
  double y_2p5_ymax[8] = {30, 150, 100, 150, 30, 150, 100, 175};
  double y_2p5_ymin[8] = {-30, 0, -100, 0, -30, 0, -10, 0};
  double z_2p5_ymax[8] = {30, 150, 30, 150, 100, 175, 100, 175};
  double z_2p5_ymin[8] = {-30, 0, -30, 0, -100, 0, -10, 0};

  double r_1to10_ymax[8] = {30, 200, 30, 200, 30, 200, 100, 150};
  double r_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};
  double z_1to10_ymax[8] = {30, 150, 30, 150, 175, 150, 100, 150};
  double z_1to10_ymin[8] = {-30, 0, -30, 0, -175, 0, -10, 0};
  double e_1to10_ymax[8] = {30, 150, 30, 150, 30, 150, 60, 150};
  double e_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};*/

  // Fit Perf FV 6, Axis Truth
  /*double r_2p5_ymax[8] = {30, 150, 30, 150, 30, 150, 100, 150};
  double r_2p5_ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};
  double x_2p5_ymax[8] = {100, 150, 30, 150, 30, 150, 100, 175};
  double x_2p5_ymin[8] = {-100, 0, -30, 0, -30, 0, -10, 0};
  double y_2p5_ymax[8] = {30, 150, 100, 150, 30, 150, 100, 175};
  double y_2p5_ymin[8] = {-30, 0, -100, 0, -30, 0, -10, 0};
  double z_2p5_ymax[8] = {30, 150, 30, 150, 100, 175, 100, 175};
  double z_2p5_ymin[8] = {-30, 0, -30, 0, -100, 0, -10, 0};

  double r_1to10_ymax[8] = {30, 200, 30, 200, 30, 200, 150, 150};
  double r_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};
  double z_1to10_ymax[8] = {30, 150, 30, 150, 175, 150, 100, 150};
  double z_1to10_ymin[8] = {-30, 0, -30, 0, -175, 0, -10, 0};
  double e_1to10_ymax[8] = {30, 150, 30, 150, 30, 150, 100, 150};
  double e_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};*/

  // Fit Perf FV 8, Axis Truth
  /*double r_2p5_ymax[8] = {30, 150, 30, 150, 30, 150, 100, 150};
  double r_2p5_ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};
  double x_2p5_ymax[8] = {100, 150, 30, 150, 30, 150, 100, 175};
  double x_2p5_ymin[8] = {-100, 0, -30, 0, -30, 0, -10, 0};
  double y_2p5_ymax[8] = {30, 150, 100, 150, 30, 150, 100, 175};
  double y_2p5_ymin[8] = {-30, 0, -100, 0, -30, 0, -10, 0};
  double z_2p5_ymax[8] = {30, 150, 30, 150, 100, 175, 100, 175};
  double z_2p5_ymin[8] = {-30, 0, -30, 0, -100, 0, -10, 0};

  double r_1to10_ymax[8] = {30, 200, 30, 200, 30, 200, 150, 150};
  double r_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};
  double z_1to10_ymax[8] = {30, 150, 30, 150, 175, 150, 125, 150};
  double z_1to10_ymin[8] = {-30, 0, -30, 0, -175, 0, -10, 0};
  double e_1to10_ymax[8] = {30, 150, 30, 150, 30, 150, 100, 150};
  double e_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};*/

  // Fit Perf FV inner av, Axis Truth
  /*double r_2p5_ymax[8] = {30, 150, 30, 150, 30, 150, 100, 150};
  double r_2p5_ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};
  double x_2p5_ymax[8] = {100, 150, 30, 150, 30, 150, 100, 175};
  double x_2p5_ymin[8] = {-100, 0, -30, 0, -30, 0, -10, 0};
  double y_2p5_ymax[8] = {30, 150, 100, 150, 30, 150, 100, 175};
  double y_2p5_ymin[8] = {-30, 0, -100, 0, -30, 0, -10, 0};
  double z_2p5_ymax[8] = {30, 150, 30, 150, 100, 175, 100, 175};
  double z_2p5_ymin[8] = {-30, 0, -30, 0, -100, 0, -10, 0};

  double r_1to10_ymax[8] = {30, 200, 30, 200, 30, 200, 150, 150};
  double r_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};
  double z_1to10_ymax[8] = {30, 150, 30, 150, 175, 150, 125, 150};
  double z_1to10_ymin[8] = {-30, 0, -30, 0, -175, 0, -10, 0};
  double e_1to10_ymax[8] = {30, 150, 30, 150, 30, 150, 100, 150};
  double e_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};*/

  // Fit Perf FV 4m, Axis Recon
  /*
  double r_2p5_ymax[8] = {30, 150, 30, 150, 30, 150, 100, 150};
  double r_2p5_ymin[8] = {-30, 0, -30, 0, -30, 0, -20, 0};
  double x_2p5_ymax[8] = {100, 150, 30, 150, 30, 150, 100, 175};
  double x_2p5_ymin[8] = {-100, 0, -30, 0, -30, 0, -10, 0};
  double y_2p5_ymax[8] = {30, 150, 100, 150, 30, 150, 100, 175};
  double y_2p5_ymin[8] = {-30, 0, -100, 0, -30, 0, -10, 0};
  double z_2p5_ymax[8] = {30, 175, 30, 175, 175, 175, 175, 175};
  double z_2p5_ymin[8] = {-30, 0, -30, 0, -150, 0, -20, 0};

  double r_1to10_ymax[8] = {30, 150, 30, 150, 30, 150, 125, 150};
  double r_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -30, 0};
  double z_1to10_ymax[8] = {30, 175, 30, 175, 175, 175, 175, 175};
  double z_1to10_ymin[8] = {-30, 0, -30, 0, -175, 0, -10, 0};
  double e_1to10_ymax[8] = {30, 150, 30, 150, 30, 150, 30, 150};
  double e_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -30, 0};*/

  // Fit Perf FV 5.5, Axis Recoon
  /*  double r_2p5_ymax[8] = {30, 175, 30, 175, 30, 175, 300, 150};
  double r_2p5_ymin[8] = {-30, 0, -30, 0, -30, 0, -20, 0};
  double x_2p5_ymax[8] = {100, 150, 30, 150, 30, 150, 100, 175};
  double x_2p5_ymin[8] = {-100, 0, -30, 0, -30, 0, -10, 0};
  double y_2p5_ymax[8] = {30, 150, 100, 150, 30, 150, 100, 175};
  double y_2p5_ymin[8] = {-30, 0, -100, 0, -30, 0, -10, 0};
  double z_2p5_ymax[8] = {30, 150, 30, 150, 120, 175, 100, 175};
  double z_2p5_ymin[8] = {-30, 0, -30, 0, -120, 0, -10, 0};

  double r_1to10_ymax[8] = {30, 200, 30, 200, 30, 200, 250, 150};
  double r_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -20, 0};
  double z_1to10_ymax[8] = {30, 150, 30, 150, 175, 220, 140, 220};
  double z_1to10_ymin[8] = {-30, 0, -30, 0, -175, 0, -10, 0};
  double e_1to10_ymax[8] = {30, 150, 30, 150, 30, 150, 80, 150};
  double e_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};*/

  // Fit Perf FV 6m, Axis Recon
  /*double r_2p5_ymax[8] = {30, 150, 30, 150, 30, 150, 100, 150};
  double r_2p5_ymin[8] = {-30, 0, -30, 0, -30, 0, -20, 0};
  double x_2p5_ymax[8] = {100, 150, 30, 150, 30, 150, 100, 175};
  double x_2p5_ymin[8] = {-100, 0, -30, 0, -30, 0, -10, 0};
  double y_2p5_ymax[8] = {30, 150, 100, 150, 30, 150, 100, 175};
  double y_2p5_ymin[8] = {-30, 0, -100, 0, -30, 0, -10, 0};
  double z_2p5_ymax[8] = {30, 175, 30, 175, 175, 175, 175, 175};
  double z_2p5_ymin[8] = {-30, 0, -30, 0, -150, 0, -20, 0};

  double r_1to10_ymax[8] = {30, 150, 30, 150, 30, 150, 125, 150};
  double r_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -30, 0};
  double z_1to10_ymax[8] = {30, 175, 30, 175, 175, 175, 175, 175};
  double z_1to10_ymin[8] = {-30, 0, -30, 0, -175, 0, -10, 0};
  double e_1to10_ymax[8] = {30, 150, 30, 150, 30, 150, 100, 175};
  double e_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};*/

  // Fit Perf FV 8m, Axis Recon
  /*double r_2p5_ymax[8] = {30, 150, 30, 150, 30, 150, 125, 150};
  double r_2p5_ymin[8] = {-30, 0, -30, 0, -30, 0, -20, 0};
  double x_2p5_ymax[8] = {100, 150, 30, 150, 30, 150, 100, 175};
  double x_2p5_ymin[8] = {-100, 0, -30, 0, -30, 0, -10, 0};
  double y_2p5_ymax[8] = {30, 150, 100, 150, 30, 150, 100, 175};
  double y_2p5_ymin[8] = {-30, 0, -100, 0, -30, 0, -10, 0};
  double z_2p5_ymax[8] = {30, 150, 30, 150, 125, 150, 125, 150};
  double z_2p5_ymin[8] = {-30, 0, -30, 0, -125, 0, -20, 0};

  double r_1to10_ymax[8] = {30, 150, 30, 150, 30, 150, 150, 150};
  double r_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -20, 0};
  double z_1to10_ymax[8] = {30, 150, 30, 150, 175, 150, 175, 150};
  double z_1to10_ymin[8] = {-30, 0, -30, 0, -175, 0, -10, 0};
  double e_1to10_ymax[8] = {30, 150, 30, 150, 30, 150, 100, 150};
  double e_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};*/

  // Fit Perf FV 6m, Axis Recon
  /*double r_2p5_ymax[8] = {30, 150, 30, 150, 30, 150, 100, 150};
  double r_2p5_ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};
  double x_2p5_ymax[8] = {100, 150, 30, 150, 30, 150, 100, 175};
  double x_2p5_ymin[8] = {-100, 0, -30, 0, -30, 0, -10, 0};
  double y_2p5_ymax[8] = {30, 150, 100, 150, 30, 150, 100, 175};
  double y_2p5_ymin[8] = {-30, 0, -100, 0, -30, 0, -10, 0};
  double z_2p5_ymax[8] = {30, 175, 30, 175, 175, 175, 175, 175};
  double z_2p5_ymin[8] = {-30, 0, -30, 0, -150, 0, -20, 0};

  double r_1to10_ymax[8] = {30, 150, 30, 150, 30, 150, 125, 150};
  double r_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -30, 0};
  double z_1to10_ymax[8] = {30, 175, 30, 175, 175, 175, 175, 175};
  double z_1to10_ymin[8] = {-30, 0, -30, 0, -175, 0, -10, 0};
  double e_1to10_ymax[8] = {30, 150, 30, 150, 30, 150, 100, 175};
  double e_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};*/

  // Fit Perf FV inner av, Axis Recon
  /*double r_2p5_ymax[8] = {30, 300, 30, 300, 30, 300, 100, 300};
  double r_2p5_ymin[8] = {-30, 0, -30, 0, -30, 0, -50, 0};
  double x_2p5_ymax[8] = {100, 150, 30, 150, 30, 150, 100, 175};
  double x_2p5_ymin[8] = {-100, 0, -30, 0, -30, 0, -10, 0};
  double y_2p5_ymax[8] = {30, 150, 100, 150, 30, 150, 100, 175};
  double y_2p5_ymin[8] = {-30, 0, -100, 0, -30, 0, -10, 0};
  double z_2p5_ymax[8] = {30, 300, 30, 300, 125, 300, 100, 300};
  double z_2p5_ymin[8] = {-30, 0, -30, 0, -125, 0, -20, 0};

  double r_1to10_ymax[8] = {30, 300, 30, 300, 30, 300, 125, 300};
  double r_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -50, 0};
  double z_1to10_ymax[8] = {30, 300, 30, 300, 175, 300, 150, 300};
  double z_1to10_ymin[8] = {-30, 0, -30, 0, -175, 0, -10, 0};
  double e_1to10_ymax[8] = {30, 150, 30, 150, 30, 150, 100, 150};
  double e_1to10_ymin[8] = {-30, 0, -30, 0, -30, 0, -10, 0};*/


  TLine *r_line = new TLine(0, 0, 6048, 0);
  TLine *xyz_line = new TLine(-6048, 0, 6048, 0);
  TLine *e_line = new TLine(1,0,10,0);

  std::string r_2p5_cname = canv_name + "_2p5_r.pdf";
  std::string x_2p5_cname = canv_name + "_2p5_x.pdf";
  std::string y_2p5_cname = canv_name + "_2p5_y.pdf";
  std::string z_2p5_cname = canv_name +"_2p5_z.pdf";
  std::string r_1to10_cname = canv_name +"_1to10_r.pdf";
  std::string x_1to10_cname = canv_name +"_1to10_x.pdf";
  std::string y_1to10_cname = canv_name +"_1to10_y.pdf";
  std::string z_1to10_cname = canv_name +"_1to10_z.pdf";
  std::string e_1to10_cname = canv_name +"_1to10_e.pdf";

  MakeCanvas( orig_r_2p5_filename, new_r_2p5_filename, orig_label, new_label, r_2p5_ymax, r_2p5_ymin, r_line, r_2p5_cname);
  MakeCanvas( orig_x_2p5_filename, new_x_2p5_filename, orig_label, new_label, x_2p5_ymax, x_2p5_ymin, xyz_line, x_2p5_cname);
  MakeCanvas( orig_y_2p5_filename, new_y_2p5_filename, orig_label, new_label, y_2p5_ymax, y_2p5_ymin, xyz_line, y_2p5_cname);
  MakeCanvas( orig_z_2p5_filename, new_z_2p5_filename, orig_label, new_label, z_2p5_ymax, z_2p5_ymin, xyz_line, z_2p5_cname);
  MakeCanvas( orig_r_1to10_filename, new_r_1to10_filename, orig_label, new_label, r_1to10_ymax, r_1to10_ymin, r_line, r_1to10_cname);
  MakeCanvas( orig_x_1to10_filename, new_x_1to10_filename, orig_label, new_label, x_1to10_ymax, x_1to10_ymin, xyz_line, x_1to10_cname);
  MakeCanvas( orig_y_1to10_filename, new_y_1to10_filename, orig_label, new_label, y_1to10_ymax, y_1to10_ymin, xyz_line, y_1to10_cname);
  MakeCanvas( orig_z_1to10_filename, new_z_1to10_filename, orig_label, new_label, z_1to10_ymax, z_1to10_ymin, xyz_line, z_1to10_cname);
   MakeCanvas( orig_e_1to10_filename, new_e_1to10_filename, orig_label, new_label, e_1to10_ymax, e_1to10_ymin, e_line, e_1to10_cname);

}


void MakeCanvas( std::string orig_filename, std::string new_filename, std::string orig_label, std::string new_label, double ymax[8], double ymin[8], TLine* line, std::string canvas_name ) {

  std::cout << orig_filename << std::endl;
  std::cout << new_filename  << std::endl;

  gStyle->SetGridStyle(3);

  std::string coord[4] = {"x", "y", "z", "r"};
  std::string title[2] = {"bias", "resolution"};

  TFile *_orig = TFile::Open(orig_filename.c_str());
  TFile *_new  = TFile::Open(new_filename.c_str());

  TCanvas* c1 = new TCanvas("c1", "c1", 1500,700);
  c1->Divide(4,2,0.01,0.05);

  for(int j=0; j<2; j++){
    for(int i=0; i<4; i++){

      std::string gname = coord[i] + "_" + title[j];
      std::cout << "getting " << gname << " " << 3*j+i+1 << std::endl;

      TGraphErrors* horig = (TGraphErrors*)_orig->Get(gname.c_str())->Clone();
      TGraphErrors* hnew = (TGraphErrors*)_new->Get(gname.c_str())->Clone();

      horig->SetLineColor(kBlue+2);
      hnew->SetLineColor(kRed+2);
      horig->SetLineWidth(1);
      hnew->SetLineWidth(1);

      TLegend* t1 = new TLegend( 0.4, 0.75, 0.9, 0.9 );
      t1->AddEntry( horig, orig_label.c_str(), "l" );
      t1->AddEntry( hnew, new_label.c_str(), "l" );

      horig->GetYaxis()->SetRangeUser(ymin[2*i+j],ymax[2*i+j]);
      title[j][0] = toupper(title[j][0]);
      coord[i][0] = toupper(coord[i][0]);
      gname = coord[i] + " " + title[j];
      horig->SetTitle(gname.c_str());
      title[j][0] = tolower(title[j][0]);
      coord[i][0] = tolower(coord[i][0]);

      c1->cd(4*j + i + 1);
      gPad->SetGrid(1);

      horig->Draw("ap");
      hnew->Draw("same p");
      t1->Draw("same");

      if(j==0)
	line->Draw("same");
      else
	horig->GetYaxis()->SetTitleOffset(1.2);

    }
  }

  c1->SaveAs(canvas_name.c_str());
}
