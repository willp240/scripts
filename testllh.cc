// Antinu headers
#include <PDFConfigLoader.hh>
#include <DistBuilder.hh>
#include <FitConfigLoader.hh>
#include <EventConfigLoader.hh>
#include <SystConfigLoader.hh>
#include <SystFactory.hh>
#include <OscGridConfigLoader.hh>

// OXO headers
#include <ROOTNtuple.h>
#include <BinnedNLLH.h>
#include <IO.h>
#include <Rand.h>
#include <Minuit.h>

// ROOT headers
#include <TH1D.h>

// c++ headers
#include <sys/stat.h>

using namespace antinufit;

int main(int argc, char *argv[])
{
  if (argc != 1)
  {
    std::cout << "\nUsage: testllh" << std::endl;
    return 1;
  }
  std::cout << "Starting " << std::endl;

  // Make a binnedED
  AxisCollection ax;
  ax.AddAxis(BinAxis("energy", 0, 2, 2));
  BinnedED pdf1("pdf1", ax);
  BinnedED pdf2("pdf2", ax);
  BinnedED pdf3("pdf3", ax);
  BinnedED data("data", ax);

  pdf1.SetBinContent(0,4);
  pdf1.SetBinContent(1,6);

  pdf2.SetBinContent(0,3);
  pdf2.SetBinContent(1,2);

  pdf3.SetBinContent(0,2);
  pdf3.SetBinContent(1,4);

  std::cout << "Made PDFs " << std::endl;

  data.SetBinContent(0,3);
  data.SetBinContent(1,4);

  std::cout << "Made data " << data.Integral() << std::endl;

  IO::SaveHistogram(data.GetHistogram(), "./data.root", "data");

  std::cout << "Saved hist " << std::endl;

  // Make a systematic
  Shift *shift = new Shift("shift");
  shift->RenameParameter("shift", "energy_shift");
  shift->SetShift(0.0);
  ObsSet obs = {"energy"};
  shift->SetAxes(ax);
  shift->SetTransformationObs(obs);
  shift->SetDistributionObs(obs);
  shift->Construct();

  std::cout << "Made shift 1 " << std::endl;

  // Make a systematic
  Shift *shift2 = new Shift("shift2");
  shift2->RenameParameter("shift", "energy_shift2");
  shift2->SetShift(0.0);
  shift2->SetAxes(ax);
  shift2->SetTransformationObs(obs);
  shift2->SetDistributionObs(obs);
  shift2->Construct();

  std::cout << "Made shift 2 " << std::endl;

  Shift *shift3 = new Shift("shift3");
  shift3->RenameParameter("shift", "energy_shift3");
  shift3->SetShift(0.0);
  shift3->SetAxes(ax);
  shift3->SetTransformationObs(obs);
  shift3->SetDistributionObs(obs);
  shift3->Construct();

  std::cout << "Made shift 3 " << std::endl;

  // Make a LLH object
  BinnedNLLH lh;
  lh.SetDataDist(data);
  lh.AddSystematic(shift, "");
  lh.AddSystematic(shift2, "");
  //lh.AddSystematic(shift3, "");
  std::vector<std::string> group1vec = {""};
  std::vector<std::string> group2vec = {""};
  std::vector<std::string> group3vec = {"pdf3"};
  lh.AddPdf(pdf1, group1vec, 100);
  lh.AddPdf(pdf2, group2vec,100);
  //lh.AddPdf(pdf3, group3vec,100);
  lh.RegisterFitComponents();

  std::cout << "Made LLH " << std::endl;

  // Set param to diff values and save histo
  ParameterDict parameterValues;
  parameterValues["pdf1"] = 1;
  parameterValues["pdf2"] = 1;
  //parameterValues["pdf3"] = 1;
  parameterValues["energy_shift"] = 0.5;
  parameterValues["energy_shift2"] = 0.1;
  //parameterValues["energy_shift3"] = -0.3;

  std::cout << "Made paramDict " << std::endl;

  lh.SetParameters(parameterValues);
  double llh = lh.Evaluate();
  std::cout << llh << std::endl;

  return 0;
}
