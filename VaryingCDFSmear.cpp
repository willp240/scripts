/************************************************************************
 * In this example we use the VaryingCDF class to apply a smear which depends
 * on the spectrum itself. We set up a VaryingCDF which takes a gaussian as its
 * kernel and a function plot which governs the width of that kernel at that
 * point in the spectrum. 
 *
 * Physics case here is that in perfect detector the energy resolutions depends
 * on the nhit of the event. The width of this resolutions goes like
 * sqrt(nhit), therefore it may be desirable to have an energy resolution which
 * mirrors this dependence.  In the following the ploy function defines the
 * functional dependence of the width of the gaussian kernel used to smear the
 * spectrum.
 * 
 ************************************************************************/


#include <string>        
#include <vector>        
#include <math.h>	
#include <fstream>

#include <Rand.h>
#include <Exceptions.h>
#include <Convolution.h>
#include <Gaussian.h>
#include <ParameterDict.h>
#include <ContainerTools.hpp>
#include <SparseMatrix.h>
#include <DistTools.h>

#include <VaryingCDF.h>
#include <SquareRootScale.h>
#include <ContainerParameter.h>
#include <Formatter.hpp>
#include <Function.h>
#include <Gaussian.h>
#include <BinnedNLLH.h>

#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TFile.h>

#include <set>
#include <map>
#include <vector>

using ContainerTools::ToString;
using ContainerTools::GetValues;
using ContainerTools::GetKeys;


int main()
{
    AxisCollection axes;
    axes.AddAxis(BinAxis("axis1", 0, 200 ,200));

    ObsSet  obsSet("axis1");
    ObsSet  obsSetToTransform("axis1");


    VaryingCDF *smearer = new VaryingCDF("smear");
    Gaussian *gaus = new Gaussian(0, 0.5, "eres");
    gaus->RenameParameter("means_0", "mean");
    gaus->RenameParameter("stddevs_0", "sigma");
    smearer->SetKernel(gaus);

    SquareRootScale *sqrtscale = new SquareRootScale("e_smear_sigma_func");
    sqrtscale->RenameParameter("grad", "eres_mean");
    sqrtscale->SetGradient(0.5);
    smearer->SetDependance("sigma", sqrtscale);
    const auto param_names = smearer->GetParameterNames();
    for (const auto &name : param_names)
      {
        std::cout << name << std::endl;
      }

    Convolution *conv = new Convolution("conv");
    conv->SetConditionalPDF(smearer);
    conv->SetAxes(axes);
    // The "dimensions" the systematic applies too
    conv->SetTransformationObs(obsSetToTransform);
    // All the "dimensions" of the dataset
    conv->SetDistributionObs(obsSet);
    conv->Construct();

    ParameterDict newparams;
    newparams["mean"] = 0;
    newparams["sigma"] = 10.0;
    newparams["eres_mean"] = 0.5;
    conv->SetParameters(newparams);
    
    Gaussian gaus1(50, 10);
    BinnedED pdf1("pdf1", DistTools::ToHist(gaus1, axes));
    pdf1.SetObservables({"axis1"});
    pdf1.Scale(40000);

    Gaussian gaus2(150, 10);
    BinnedED pdf2("pdf2", DistTools::ToHist(gaus2, axes));
    pdf2.SetObservables({"axis1"});
    pdf2.Scale(40000);
    
    TH1D Pdf1 = DistTools::ToTH1D(pdf1);
    TH1D Pdf1AfterSmear = DistTools::ToTH1D(conv->operator()(pdf1));
    TH1D Pdf2 = DistTools::ToTH1D(pdf2);
    TH1D Pdf2AfterSmear = DistTools::ToTH1D(conv->operator()(pdf2));
    TLegend leg(0.6,0.6,0.9,0.9); 
    leg.AddEntry(&Pdf1,"PDF1 Before","lf"); 
    leg.AddEntry(&Pdf1AfterSmear,"PDF1 After","lf"); 
    leg.AddEntry(&Pdf2,"PDF2 Before","lf");
    leg.AddEntry(&Pdf2AfterSmear,"PDF2 After","lf");
    
    TCanvas c1;
    gStyle->SetOptStat(kFALSE);  
    Pdf1.SetTitle("Before/After smear");
    Pdf1.SetLineColor(kRed);
    Pdf1.SetLineWidth(2);
    Pdf1.Scale(1./pdf1.Integral());
    Pdf1.Draw();
    Pdf1AfterSmear.SetLineColor(kBlue);
    Pdf1AfterSmear.SetLineWidth(2);
    Pdf1AfterSmear.Draw("same");
    Pdf2.SetLineColor(kRed+2);
    Pdf2.SetLineWidth(2);
    Pdf2.Scale(1./pdf2.Integral());
    Pdf2.Draw("same");
    Pdf2AfterSmear.SetLineColor(kBlue+2);
    Pdf2AfterSmear.SetLineWidth(2);
    Pdf2AfterSmear.Draw("same");
    
    //    leg.Draw("same");

    TFile* f = new TFile("out.root","RECREATE");
    f->cd();
    c1.Write();
    f->Close();

    int BuffLow  = 20;
    int BuffHigh = 20;
    
    BinnedNLLH lh;
    lh.SetBufferAsOverflow(false);
    lh.SetBuffer("axis1",BuffLow,BuffHigh);
    lh.SetDataDist(pdf1);
    lh.AddSystematic(conv);
    lh.AddPdf(pdf1);
    lh.AddPdf(pdf2);
    lh.RegisterFitComponents();
    
    ParameterDict pname = lh.GetParameters();


    std::cout << pname.size() << std::endl;
    for (auto& str : pname) {
      std::cout << str.first << " " << str.second << std::endl;
    }

    return 0;
}
