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

  // Make a binnedED
  AxisCollection ax;
  ax.AddAxis(BinAxis("energy", 0, 10, 10));
  BinnedED pdf("pdf", ax);

  for (size_t i = 0; i < 10; i++)
  {
    std::vector<double> vals;
    double val = exp(-(i - 5.) * (i - 5.));
    std::cout << val << " " << i << std::endl;
    pdf.SetBinContent(i + 1, val);
  }
  pdf.AddPadding(1E-6);
  pdf.Normalise();
  IO::SaveHistogram(pdf.GetHistogram(), "./pdf.root", "pdf");

  // Make a systematic
  Scale *scale = new Scale("scale");
  scale->RenameParameter("scaleFactor", "energy_scale");
  scale->SetScaleFactor(4.0);

  ObsSet obs = {"energy"};
  scale->SetAxes(ax);
  scale->SetTransformationObs(obs);
  scale->SetDistributionObs(obs);
  scale->Construct();

  // Make Asimov dist
  BinnedED asimov("asimov", ax);
  asimov.Add(pdf);
  asimov.Scale(10);
  asimov = scale->operator()(asimov);
  IO::SaveHistogram(asimov.GetHistogram(), "./asimov.root", "asimov");

  // Make a LLH object
  BinnedNLLH lh;
  lh.AddPdf(pdf);
  lh.SetDataDist(asimov);
  lh.AddSystematic(scale, "");
  lh.RegisterFitComponents();

  // Set param to diff values and save histo
  ParameterDict parameterValues;
  parameterValues["pdf"] = 10;
  parameterValues["energy_scale"] = 4.0;

  lh.SetParameters(parameterValues);
  double llh = lh.Evaluate();
  std::cout << llh << std::endl;

  parameterValues["energy_scale"] = 10.0;
  lh.SetParameters(parameterValues);
  llh = lh.Evaluate();
  std::cout << llh << std::endl;

  return 0;
}
