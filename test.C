#include <TH3D.h>
#include <TCanvas.h>
#include "TFile.h"
#include "TColor.h"

#include <string>
#include <iostream>

void test() {

  int fNhits = 100;
  double alphaNhits = 230.9;
  double eventNhits = fNhits;
  double probabilityBi = (alphaNhits/eventNhits)*1;
  double probabilityPo = ((eventNhits-alphaNhits)/eventNhits)*1;
  std::cout<<probabilityBi<<" ," <<probabilityPo<<"\n";
  double probability = probabilityBi + probabilityPo;
}
