
ParameterDict parameterValues;
ParameterDict initialPos;
//parameterValues["bi214_id"] = 175.689;
//parameterValues["2v"] = 155668;
//parameterValues["tl208_id"] = 299.366;
//parameterValues["tl210"] = 38.9602;
//parameterValues["bi214_av"] = 116821;
//  lh.SetParameters(parameterValues);
TFile *asmvRatesFile = new TFile("/data/snoplus/parkerw/bb_sigex/May_20_all/asimovdata/asimovRates.ro\
ot", "OPEN");
std::map<std::string, double>* tempMap;
asmvRatesFile->GetObject("AsimovRates",tempMap);
parameterValues = (ParameterDict)*tempMap;
parameterValues["0v"] = 0.0;

for(ParameterDict::iterator it = sigmas.begin(); it != sigmas.end(); ++it){
  initialPos[it->first] = parameterValues[it->first];
  std::cout << ". " << it->first << " " << parameterValues[it->first] << std::endl;
 }
//  mh.SetInitialTrial(initialPos);
asmvRatesFile->Close();
