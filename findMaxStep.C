#include <TStyle.h>
#include <TTree.h>

void findMaxStep( ) {

  std::string filepath = "/data/snoplus/parkerw/bb_sigex/Sep9_allbg_shift0.1/Sep9_20000_allbg_scaleshift_fdshift0.1/Sep9_20000_allbg_scaleshift_fdshift0.1";

  double maxLLH = -999;
  int maxFile = -999;

  //Loop over files
  for(int i=0; i<20; i++){    
    TString fname = Form("%s_%d_.root",filepath.c_str(),i);

    if(!gSystem->AccessPathName(fname)){
      TFile *File = new TFile(fname , "OPEN");
      TChain* chain = new TChain("posteriors","");
      chain->Add(fname);

      // Get max LLH
      double llh = chain->GetMinimum("LogL");

      // If LLH > current max LLH
      if(llh > maxLLH){
	// current file is max file
	maxLLH = llh;
	maxFile = i;
      }
    }
  }

  //print out max file number
  std::cout << "Max LLH is " << maxLLH << " in file " << maxFile << std::endl;

}
