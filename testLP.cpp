#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>

#include <RAT/DU/DSReader.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/DU/LightPathCalculator.hh>
#include <RAT/LightPathStraightScintWater.hh>
#include <RAT/LightPathStraightScint.hh>
#include <RAT/DU/GroupVelocity.hh>
#include <RAT/DU/EffectiveVelocity.hh>
#include <RAT/DS/MC.hh>
#include <RAT/DS/MCEV.hh>
#include <RAT/DS/MCParticle.hh>
#include <RAT/DS/EV.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/Version.hh>


using namespace RAT;

void TestLightPath(){

  RAT::DU::DSReader dsReader("test.root");

  //  RAT::LP::LightPathStraightScint::BeginOfRun();

  TVector3 pmtPos(8000, 0, 0);
  TVector3 mcPos(0, 0, 0);

  double distInAV = 0.0;
  double distInWater = 0.0;
  double distInTarget = 0.0;
  double distInInnerAV = 0.0;
  
  RAT::LP::LightPathStraightScint::GetPath(pmtPos, mcPos, distInTarget, distInWater);

  std::cout << distInTarget << " " << distInWater << std::endl;

}


