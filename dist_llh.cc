#include <string>
#include <FitConfigLoader.hh>
#include <DistConfigLoader.hh>
#include <DistTools.h>
#include <DistConfig.hh>
#include <DistBuilder.hh>
#include <CutConfigLoader.hh>
#include <FitConfigLoader.hh>
#include <FitConfig.hh>
#include <CutFactory.hh>
#include <CutCollection.h>
#include <fstream>
#include <ROOTNtuple.h>
#include <BinnedNLLH.h>
#include <sys/stat.h>
#include <Rand.h>
#include <AxisCollection.h>
#include <IO.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TKey.h>
#include "TColor.h"
#include <TStyle.h>
//#include <Minuit.h>

#include <SystematicManager.h>
#include <Scale.h>
#include <Convolution.h>
#include <Gaussian.h>
#include <Event.h>
#include <gsl/gsl_cdf.h>

using namespace bbfit;

void BlueRedPalette();

void
dist_llh(const std::string& mcmcConfigFile_,
	 const std::string& distConfigFile_,
    const std::string& cutConfigFile_, 
    const std::string& dataPath_,
    const std::string& asmvRatesPath,
    const std::string& asmvDistsFileName){
    Rand::SetSeed(0);

    // Load up the configuration data
    FitConfig mcConfig;
  
    typedef std::vector<CutConfig> CutVec;
    CutVec cutConfs;    
    {
        FitConfigLoader mcLoader(mcmcConfigFile_);
        mcConfig = mcLoader.LoadActive();
    
        CutConfigLoader cutConfLoader(cutConfigFile_);
        cutConfs = cutConfLoader.LoadActive();
    }    

  // Make the cuts
  CutCollection cutCol;
  for(CutVec::iterator it = cutConfs.begin(); it != cutConfs.end();
      ++it){
    std::string name = it->GetName();
    std::string type = it->GetType();
    std::string obs = it->GetObs();
    double val = it->GetValue();
    double val2 = it->GetValue2();
    Cut *cut = CutFactory::New(name, type, obs, val, val2);
    cutCol.AddCut(*cut);
    delete cut; // cut col takes its own copy
  }
  

  // Load up the dists
  DistConfigLoader dLoader(distConfigFile_);
  DistConfig pConfig = dLoader.Load();
  std::string distDir = pConfig.GetPDFDir();

  std::vector<BinnedED> dists;
    
  // the ones you actually want to fit are those listed in mcmcconfig
  typedef std::set<std::string> StringSet;
  StringSet distsToFit = mcConfig.GetParamNames();
  
  for(StringSet::iterator it = distsToFit.begin(); it != distsToFit.end();
      ++it){
    std::string distPath = distDir + "/" + *it + ".h5";
    dists.push_back(BinnedED(*it, IO::LoadHistogram(distPath)));
  }

  // if its a root tree then load it up
  BinnedED dataDist;
 
  if(dataPath_.substr(dataPath_.find_last_of(".") + 1) == "h5"){
      Histogram loaded = IO::LoadHistogram(dataPath_);
      dataDist = BinnedED("data", loaded);
      dataDist.SetObservables(pConfig.GetBranchNames());
  }
  else{
      // Load up the data set
      ROOTNtuple dataToFit(dataPath_, "pruned");
      
      // Log the effects of the cuts
      CutLog log(cutCol.GetCutNames());
      
      // and bin the data inside
      dataDist = DistBuilder::Build("data", pConfig, (DataSet*)&dataToFit, cutCol, log);
       
  }

  std::cout<< "Marginilising for 2d" << std::endl;
  std::vector<std::string> keepObs;
  keepObs.push_back("energy");
  keepObs.push_back("r");
  dataDist = dataDist.Marginalise(keepObs);
  
  
  // now build the likelihood
  BinnedNLLH lh;
  lh.AddPdfs(dists);
  lh.SetCuts(cutCol);
  lh.SetDataDist(dataDist);

  ParameterDict constrMeans  = mcConfig.GetConstrMeans();
  ParameterDict constrSigmas = mcConfig.GetConstrSigmas();
  ParameterDict mins = mcConfig.GetMinima();
  ParameterDict maxs = mcConfig.GetMaxima();
 
  for(ParameterDict::iterator it = constrMeans.begin(); it != constrMeans.end();
      ++it)
      lh.SetConstraint(it->first, it->second, constrSigmas.at(it->first));

  //Load in asimov rates for central value of each parameter
  TFile *asmvRatesFile = new TFile(asmvRatesPath.c_str(), "OPEN");
  asmvRatesFile->cd();
  std::map<std::string, double>* tempMap;
  asmvRatesFile->GetObject("AsimovRates",tempMap);
  ParameterDict asimovRates = (ParameterDict)*tempMap;
  lh.RegisterFitComponents();

  ParameterDict parameterValues;// = asimovRates;

  for(ParameterDict::iterator it = mins.begin(); it != mins.end(); ++it)
    parameterValues[it->first] = asimovRates[it->first];
  for(ParameterDict::iterator it = constrMeans.begin(); it != constrMeans.end();
      ++it)
    parameterValues[it->first] = constrMeans[it->first];
  lh.SetParameters(parameterValues);

  parameterValues["bi214_id"] = 58.233;
  parameterValues["co60"] = 0.33438;
  parameterValues["tl208_id"] = 99.787;
  parameterValues["tl210"] = 11.738;
  lh.SetParameters(parameterValues);

  ///Set Params to Asmv rates
  ///Print LLH 
  double LLH = lh.Evaluate();
  std::cout << "Asimov " << LLH << std::endl;
  ///save histo
  BinnedED scaleddist;
  AxisCollection axes = DistBuilder::BuildAxes(pConfig);
  scaleddist = BinnedED("scaleddist", axes);

  for(size_t i = 0; i < dists.size(); i++){
    std::string name = dists.at(i).GetName();
    dists[i].Normalise();
    dists[i].Scale(parameterValues[name]);
    scaleddist.Add(dists[i]);
  }
  TH2D* scaledhist = new TH2D (DistTools::ToTH2D(scaleddist));
 
  TFile *asmvDistsFile = new TFile(asmvDistsFileName.c_str(), "READ");
  asmvDistsFile->cd();
  TH2D* prefitDist;
  TIter next(asmvDistsFile->GetListOfKeys());
  TKey *key;
  while ((key = (TKey*)next())) {
    std::string classname = std::string(key->GetClassName());
    if(classname == "TH2D")
      prefitDist = (TH2D*)key->ReadObj();
  }
  
  TCanvas* c0 = new TCanvas("c0", "c0", 0, 0, 800, 500);  
  //BlueRedPalette();
  //  prefitDist->Divide(scaledhist);
  //prefitDist->GetZaxis()->SetRangeUser(0.7,1.3);
  prefitDist->Draw("colz");

  TFile* file = new TFile("output.root", "RECREATE");
  file->cd();
  c0->Write("canvas");
  prefitDist->Write("prefitDist");
  scaledhist->Write("scaledhist");
  ///Set params to postfit
  ///Print LLH
  ///save histo

}

int main(int argc, char *argv[]){
  if (argc != 7){
    std::cout << "\nUsage: dist_llh <mcmcConfigFile> <dist_config_file> <cut_config_file> <data_to_fit> <asimovRatesFile> <asmvDistsFileName>" << std::endl;
      return 1;
  }

  std::string fitConfigFile(argv[1]);
  std::string pdfPath(argv[2]);
  std::string cutConfigFile(argv[3]);
  std::string dataPath(argv[4]);
  std::string asmvRatesPath(argv[5]);
  std::string asmvDistsFileName(argv[6]);

  dist_llh(fitConfigFile, pdfPath, cutConfigFile, dataPath, asmvRatesPath, asmvDistsFileName);

  return 0;
}

void BlueRedPalette() {

  // Take away the stat box
  gStyle->SetOptStat(0);
  // Make pretty correlation colors (red to blue)
  const int NRGBs = 5;
  TColor::InitializeColors();
  Double_t stops[NRGBs] = { 0.00, 0.25, 0.50, 0.75, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.25, 1.00, 1.00, 0.50 };
  Double_t green[NRGBs] = { 0.00, 0.25, 1.00, 0.25, 0.00 };
  Double_t blue[NRGBs]  = { 0.50, 1.00, 1.00, 0.25, 0.00 };
  TColor::CreateGradientColorTable(5, stops, red, green, blue, 255);
  gStyle->SetNumberContours(255);
}
