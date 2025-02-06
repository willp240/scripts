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

// ROOT headers
#include <TH1D.h>

// c++ headers
#include <sys/stat.h>

using namespace antinufit;

void test()
{
 
    // Make BinnedEDs
    AxisCollection ax;
    ax.AddAxis(BinAxis("xaxis", 0, 3, 3));
    BinnedED pdf1("pdf1", ax);
    BinnedED data("data", ax);

    // Some simple bin contents
    double bin0_pdf1 = 1./3.;
    double bin1_pdf1 = 1./3.;
    double bin2_pdf1 = 1./3.;

    double bin0_data = 1.;
    double bin1_data = 1.;
    double bin2_data = 1.;
 
    pdf1.SetBinContent(0, bin0_pdf1);
    pdf1.SetBinContent(1, bin1_pdf1);
    pdf1.SetBinContent(2, bin2_pdf1);

    data.SetBinContent(0, bin0_data);
    data.SetBinContent(1, bin1_data);
    data.SetBinContent(2, bin2_data);
    
    // Make a LLH object
    BinnedNLLH lh1;
    std::cout << "We do shrink" << std::endl;
    lh1.SetBuffer("xaxis", 1, 1);
    lh1.SetDataDist(data);
    lh1.AddPdf(pdf1, INDIRECT);
    lh1.RegisterFitComponents();

    double pdfnorm1 = 3.0;
    ParameterDict parameterValues;
    parameterValues["pdf1"] = pdfnorm1;
    lh1.SetParameters(parameterValues);
    double llh = lh1.Evaluate();

    double bin0llh = -bin0_data * log(bin0_pdf1*pdfnorm1) + bin0_pdf1*pdfnorm1;
    double bin1llh = -bin1_data * log(bin1_pdf1*pdfnorm1) + bin1_pdf1*pdfnorm1;
    double bin2llh = -bin2_data * log(bin2_pdf1*pdfnorm1) + bin2_pdf1*pdfnorm1;

    double unshrunkllh = bin0llh + bin1llh + bin2llh;
    double shrunkllh = bin1llh;
    
    std::cout << "Evaluated LLH: " << llh  << std::endl;
    std::cout << "Unshrunk LLH hand-calculated: " << unshrunkllh << std::endl;
    std::cout << "Shrunk LLH hand-calculated: " << shrunkllh  << std::endl;
}

int main(int argc, char *argv[])
{

  test();

  return 0;
}
