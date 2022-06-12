
#include <RAT/DU/DSReader.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/DU/LightPathCalculator.hh>
#include <RAT/DU/GroupVelocity.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/MC.hh>
#include <RAT/DS/EV.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/FitResult.hh>
#include <RAT/LightPathStraightScint.hh>
#include <RAT/DU/TimeResidualCalculator.hh>
#include <RAT/SunUtil.hh>

#include <TFile.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TMath.h>

#include <string>

void* checkDir( const std::string& fileName)
{

  TFile* file = new TFile(fileName.c_str(),"READ");
  TH2D* histogram1 = (TH2D*)file->Get("hCosVSTime");

  std::cout << "probability: [";
  for(int i=1; i<=histogram1->GetXaxis()->GetNbins(); i++){
    for(int j=1; j<=histogram1->GetYaxis()->GetNbins(); j++){
      std::cout << std::fixed << std::setprecision(2)<< float(histogram1->GetBinContent(i,j)) << ", ";
    }
  }
  std::cout << "]," << endl;

}
