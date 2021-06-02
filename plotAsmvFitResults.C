#include <string.h>
#include <cstddef>

static void SetPalette();

void plotAsmvFitResults(std::string name) {

  std::string resDir = "/data/snoplus/parkerw/bb_sigex/"+name+"/result/";
  std::string prefitDir = "/data/snoplus/parkerw/bb_sigex/"+name+"/asimovdata/";
  std::string llhDir = resDir+"1dlhproj/";
  std::string postfitDir = resDir+"scaled_dists/";
  std::cout << resDir << std::endl;
  gStyle->SetPalette(51);
  gStyle->SetOptStat(0);
  gStyle->SetNumberContours(255);

  char* dir = gSystem->ExpandPathName(prefitDir.c_str());
  void* dirp = gSystem->OpenDirectory(dir);
  const char* ext = ".root";

  const char* entry;
  std::string prefitname, postfitname, llhname;
  TH2D* prefit;
  TH2D* postfit;
  TH1D* llh;
  TCanvas c1;
  std::string outfileext = "_asmvResultPlots.pdf";
  c1.Print((postfitDir+name+outfileext+"[").c_str());
  
  while((entry = (char*)gSystem->GetDirEntry(dirp))) {
    std::cout << entry << std::endl;
    std::string str = entry;
    //    if(str.substr( str.length() - 5 ) == ".root"){
    if (str.find(".root") != std::string::npos) { 
    //prefitname= gSystem->ConcatFileName(dir, entry);
      prefitname = prefitDir + str;
      TFile *fPrefit = TFile::Open(prefitname.c_str());
      TIter next(fPrefit->GetListOfKeys());
      TKey *key;
      while ((key = (TKey*)next())) {
	std::string classname = std::string(key->GetClassName());
	if (classname == "TH2D") {
	  prefit = (TH2D*)key->ReadObj();;
	  prefit->GetXaxis()->SetTitle("Energy, MeV");
	  prefit->GetYaxis()->SetTitle("(R/R_{eff})^3");
	  prefit->SetTitle(prefitname.c_str());
	  c1.cd();
	  c1.SetRightMargin(0.13);
	  prefit->Draw("colz");
	  c1.Print((postfitDir+name+outfileext).c_str());
	  TH1D* prefitX = (TH1D*)prefit->ProjectionX();
	  prefitX->SetMinimum(0);
	  prefitX->Draw();
	  c1.Print((postfitDir+name+outfileext).c_str());
	  TH1D* prefitY = (TH1D*)prefit->ProjectionY();
          prefitY->SetMinimum(0);
	  prefitY->Draw();
	  c1.Print((postfitDir+name+outfileext).c_str());
	}

	postfitname = postfitDir + str;
	TFile *fPostfit = TFile::Open(postfitname.c_str());
	TIter next(fPostfit->GetListOfKeys());
	TKey *key;
	while ((key = (TKey*)next())) {
	  classname = std::string(key->GetClassName());
	  if (classname == "TH2D") {
	    postfit = (TH2D*)key->ReadObj();;
	    postfit->GetXaxis()->SetTitle("Energy, MeV");
	    postfit->GetYaxis()->SetTitle("(R/R_{eff})^3");
	    postfit->SetTitle(postfitname.c_str());
	    c1.cd();
	    c1.SetRightMargin(0.13);
	    postfit->Draw("colz");
	    c1.Print((postfitDir+name+outfileext).c_str());
	    TH1D* postfitX = (TH1D*)postfit->ProjectionX();
	    postfitX->SetMinimum(0);
	    postfitX->Draw();
	    c1.Print((postfitDir+name+outfileext).c_str());
	    TH1D* postfitY = (TH1D*)postfit->ProjectionY();
	    postfitY->SetMinimum(0);
	    postfitY->Draw();
	    c1.Print((postfitDir+name+outfileext).c_str());
	  }

	  TH2D* ratio = (TH2D*)prefit->Clone("ratio");
	  ratio->Divide(postfit);
	  ratio->GetZaxis()->SetRangeUser(0.7,1.3);
	  ratio->SetTitle((str + " prefit/postfit ratio").c_str());
	  SetPalette();
	  ratio->Draw("colz");
	  c1.Print((postfitDir+name+outfileext).c_str());
	}
	gStyle->SetPalette(51);
	llhname = llhDir + str;
	TFile *fLLH = TFile::Open(llhname.c_str());
        TIter next(fLLH->GetListOfKeys());
        TKey *key;
        while ((key = (TKey*)next())) {
	  classname = std::string(key->GetClassName());
          if (classname == "TH1D") {
            llh = (TH1D*)key->ReadObj();;
            llh->GetXaxis()->SetTitle("Events per Year");
            llh->GetYaxis()->SetTitle("Steps");
            llh->SetTitle(llhname.c_str());
            c1.cd();
            llh->Draw();
	    c1.Print((postfitDir+name+outfileext).c_str());
	  }
	}
      }
    }
  }
  c1.Print((postfitDir+name+outfileext+"]").c_str());
}


static void SetPalette() {

TColor::InitializeColors();
const Int_t NRGBs = 5;
const Int_t NCont = 255;
/*
     way this works is that you define the stops and what fraction of colour
     should be applied. e.g. for stop 0.10 we have 0.25 red, 0.25 green, 1.00
     blue
*/

Double_t stops[NRGBs] = { 0.00, 0.25, 0.50, 0.75, 1.00 };
Double_t red[NRGBs]   = { 0.00, 0.25, 1.00, 1.00, 0.50 };
Double_t green[NRGBs] = { 0.00, 0.25, 1.00, 0.25, 0.00 };
Double_t blue[NRGBs]  = { 0.50, 1.00, 1.00, 0.25, 0.00 };

int start = TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
//gStyle->SetNumberContours(NCont);
}
