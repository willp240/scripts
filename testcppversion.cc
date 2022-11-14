#include <TVector3.h>
//#include <AnaEvent.hh>
//#include <EventCut.hh>
//#include <CoincidenceCut.hh>
//#include <InputFile.hh>
//#include <CutConfigLoader.hh>
//#include <testClass.hh>
#include <array>
#include <iostream>

int main( ){

  std::array<int, 3> arr = {2, 3, 5};
  
#if __cplusplus==201402L
  std::cout << "C++14" << std::endl;
#elif __cplusplus==201103L
  std::cout << "C++11 " << __cplusplus << std::endl;
#else
  std::cout << "C++" << std::endl;
#endif

#if (__cplusplus >= 201103L)
  std::cout << "ok " << std::endl;
#endif

  //std::vector<EventCut> evcutConfs;

  //const std::string cutname = "./cut_config.ini";
  //CutConfigLoader cutConfLoader( cutname );
  //  evcutConfs = cutConfLoader.LoadActiveEventCuts();

  return 0;
}

//open config read name, fitter name, debug, savehistos, muonfollower
//read cuts

//make output files, trees

//loop over events backwards

// get ith event

// check for muon followers

// passed analysis cuts?
// passed delay cuts?

// if delay

// count back til 0
// get (i - counter)th event

// passes prompt cuts?
// passes delta cuts?

// if delta t > delta t max: break (will find max delta t from vec of coinc cuts)

// fill relevant histos, output txt files

//histogram class?
