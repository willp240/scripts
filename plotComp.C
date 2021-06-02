#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <TMath.h>
#include <string>

#include "TObjArray.h"
#include "TChain.h"
#include "TFile.h"
#include "TBranch.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TLegend.h"
#include "TString.h"
#include "TStyle.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h"
#include "TGraphErrors.h"
#include "TVectorD.h"
#include "TColor.h"
#include "TKey.h"
#include "TROOT.h"

void plotComp(){

  TDirectory *dir = gDirectory;
  std::string postfitDir = "/data/snoplus/parkerw/bb_sigex/May_19_all/result_1000/scaled_dists";
  std::cout << postfitDir << std::endl;
  dir->cd(postfitDir.c_str());
  TDirectory *Dir = gDirectory;
  std::cout << postfitDir << std::endl;
  TIter next(Dir->GetListOfKeys());
  TKey *key;

  TH2* hPostfit;
  TH2* hPrefit;
  TCanvas *c = new TCanvas("canv", "canv", 1500, 1080);
  c->Print("test.pdf[");

  while ((key = (TKey*)next())) {

    std::string name = std::string(key->GetName());
    std::string dirname = std::string(dir->GetName());
    std::cout << "in " << dirname << " looking at " << name << std::endl;


    dir->cd(name.c_str());
    TDirectory *SubDir = gDirectory;

    if(name == "postfitdist.root") continue;

    TIter next2(SubDir->GetListOfKeys());
    TKey *key2;

    while ((key2 = (TKey*)next2())) {
      TClass *cl = gROOT->GetClass(key2->GetClassName());
      if (!cl->InheritsFrom("TH1")) continue;
      hPostfit = (TH2*)key->ReadObj();    

      //    name = name.substr(0, name.find(".root"));
      const size_t last_slash_idx = name.rfind('/');
      if (std::string::npos != last_slash_idx)
	name = name.substr(0, last_slash_idx);
      
      std::string asmv =  "/data/snoplus/parkerw/bb_sigex/May_19_all/asimovdata/" + name; 
      
      dir->cd(asmv.c_str());
      TDirectory *SubDir2 = gDirectory;
      
      TIter next3(SubDir2->GetListOfKeys());
      TKey *key3;
      
      while ((key3 = (TKey*)next3())) {
	TClass *cl2 = gROOT->GetClass(key3->GetClassName());
	if (!cl2->InheritsFrom("TH1")) continue;
	hPrefit = (TH2*)key->ReadObj();
      }   
    }
    hPrefit->Divide(hPostfit);
    hPrefit->GetZaxis()->SetRangeUser(0.7,1.3);
    hPrefit->SetTitle(name.c_str());
    hPrefit->Draw("colz");
    c->Print("test.pdf");
  }
  c->Print("test.pdf]");
}

