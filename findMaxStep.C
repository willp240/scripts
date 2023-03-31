#include <TStyle.h>
#include <TTree.h>

void findMaxStep( std::string dataset_dir, std::string fit_dir ) {

  std::string filepath = "/data/snoplus/parkerw/bb_sigex/" + dataset_dir + "/" + fit_dir + "/" + fit_dir;

  double maxLLH = -999;
  int maxFile = -999;
  int step = -999;
  //Loop over files
  for(int i=0; i<100; i++){
    TString fname = Form("%s_%d/%s_%d_.root",filepath.c_str(),i,fit_dir.c_str(),i);

    std::cout << fname << std::endl;
    if(!gSystem->AccessPathName(fname)){
      TFile *File = new TFile(fname , "OPEN");
      TChain* chain = new TChain("posteriors","");
      chain->Add(fname);

      // Get max LLH
      double llh = chain->GetMaximum("LogL");

      // If LLH > current max LLH
      if(llh > maxLLH){
	// current file is max file
	maxLLH = llh;
	maxFile = i;
      }
    }
  }

  //print out max file number
  std::cout << "Max LLH is " << setprecision(10) << maxLLH << " in file " << maxFile << std::endl;

}
