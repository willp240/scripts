#include <string>

void testString() {

  std::string outDir = "/data/snoplus/parkerw/bb_sigex/May_12_allbg/result_redone_10";

  std::string chainFileName = outDir;
  std::string tempString1 = chainFileName;
  std::string tempString2 = chainFileName;

  //Strip off end after last / (keep both ends)
  
  //For end 1, strip off everything before last /
  //add end 1 + _ + end 2 + .root

  size_t last_slash_idx = tempString1.find_last_of("/");
  tempString2.erase(0, last_slash_idx+1 );
  if (std::string::npos != last_slash_idx){
    tempString1.erase(last_slash_idx,string::npos);
    last_slash_idx = tempString1.find_last_of("/");
    if (std::string::npos != last_slash_idx)
      tempString1.erase(0, last_slash_idx );
  }

  chainname = outDir+tempString1+"_"+tempString2+".root";
  std::cout << chainname << std::endl;

}
