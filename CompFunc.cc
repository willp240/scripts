#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"
#include <TMath.h>
#include <TF2.h>

#include <string>
using namespace std;

double* PrintFunc( )
{

  TF2* fPDFfunc = new TF2("fPDFfunc", "100000* ( ([0]*y*y + [1]*y + [2]) * exp(-0.5 * ( (x - ([3]*y*y + [4]*y + [5]) ) / ([6]*y*y + [7]*y + [8]) )^2) + ([9]*y*y + [10]*y + [11]) * exp(-0.5 * ( (x - ([12]*y*y + [13]*y + [14]) ) / ([15]*y*y + [16]*y + [17]) )^2) + ([18]*y*y + [19]*y + [20]) * exp(-0.5 * ( (x - ([21]*y*y + [22]*y + [23]) ) / ([24]*y*y + [25]*y + [26]) )^2) + ([27]*y*y + [28]*y + [29]) * exp(-0.5 * ( (x - ([30]*y*y + [31]*y + [32]) ) / ([33]*y*y + [34]*y + [35]) )^2) )", -100, 300, 2, 14);

  double Pars[36] = {0,0,3.7,0,0,-360,0,0.717,106.86,0,0,-0.225,0,0,-75,0,0.357,30.98,0.00029,-0.0079,0.075,-0.0117,0.25,2.67,-0.013,0.19,1.92,0.0000976,-0.0032,0.037,-0.027,0.583,6.58,-0.051,0.69,4.11};

  //  for(int i=0; i<36; i++){
  //fFuncPars[i] = Pars[i];
  // }
  fPDFfunc->SetParameters(Pars);

  for(int i=-100; i<301; i++)
    std::cout << fPDFfunc(i,7) << ", " ;

}
