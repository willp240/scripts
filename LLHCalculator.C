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


void LLHCalculator(){

  gStyle->SetPalette(51);
  gStyle->SetLineWidth(2);
  gStyle->SetOptStat(0);

  TFile *fFD = TFile::Open("/data/snoplus3/parkerw/antinu/Jan15_gridllhtest/oscasimov_8.root");
  TFile *fAsmv = TFile::Open("/data/snoplus3/parkerw/antinu/Jan15_gridllhtest/asimov.root");

  TH1D *hFD = (TH1D*) fFD->Get("asimov")->Clone("hFD");
  TH1D *hAsmv = (TH1D*) fAsmv->Get("asimov")->Clone("hAsmv");

  hFD->SetLineColor(kRed);
  hAsmv->SetLineColor(kBlue);

  double llh_fd = 0;
  double llh_asmv = 0;
  
  for(int i=1; i <= hFD->GetXaxis()->GetNbins(); i++){

    double mu_fd = hFD->GetBinContent(i);
    double mu_asmv = hAsmv->GetBinContent(i);

    double dat = hAsmv->GetBinContent(i);

    double binllh_fd = - dat*log(mu_fd) + mu_fd;
    double binllh_asmv = - dat*log(mu_asmv) + mu_asmv;

    llh_fd += binllh_fd;
    llh_asmv += binllh_asmv;

    if(binllh_fd < binllh_asmv)
      std::cout << "bin " << i << " (" << hFD->GetBinCenter(i) << ") " << binllh_fd << " " << binllh_asmv << std::endl;
  }

  std::cout << "Final LLH: " << llh_fd << " " << llh_asmv << std::endl;
}

