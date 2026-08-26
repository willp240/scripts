#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2D.h>

#include <RAT/DU/DSReader.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/DU/LightPathCalculator.hh>
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

void FillFullTimeResidualsPlot(std::string material, TH2D *histogram, std::string tResidPos, double velocity = -999);
void PrintDirection2DPDFRatdb(std::string material, TH2D *histogram, int numTimeBins, float lowTimeLim, float upTimeLim,
                              int numAngleBins, float lowAngleLim, float upAngleLim, std::string tResidPos,
                              std::string evePos, double energy);
void SaveDirection2DPDFRootFile(std::string material, double energy, std::string tResidPos, std::string evePos,
                                TH2D *mcHistogram, TH2D *pdfHistogram);

struct WeightedRisetimeInput
{
  std::string suffix;
  double weight;
};

void GetFullPDF(std::string material, unsigned int numberOfRuns, double velocity = -999, std::string tResidPos = "FitPos", std::string evePos = "fullvolume", double energy = 6)
{

  const std::string inputDirectory = "/data/snoplus3/parkerw/ratSimulations/May14_risetime1p10";

  const int numTimeBins = 400;      // ns - width of bins in PDF
  const float lowTimeLim = -100.0;  // ns - lower end of PDF
  const float upTimeLim = 300.0;    // ns - upper end of PDF
  const int numAngleBins = 20;      // ns - width of bins in PDF
  const float lowAngleLim = -1.0;   // ns - lower end of PDF
  const float upAngleLim = 1.0;     // ns - upper end of PDF
  const float lowFlatTimeLim = -20; // ns - below this limit, PDF is flat
  const float upFlatTimeLim = 230;  // ns - above this limit, PDF is flat

  TH2D *histogram1 = new TH2D("histo1", "", numTimeBins, lowTimeLim, upTimeLim, numAngleBins, lowAngleLim, upAngleLim);
  TH2D *histogram2 = new TH2D("histo2", "", numTimeBins, lowTimeLim, upTimeLim, numAngleBins, lowAngleLim, upAngleLim);

  std::cout << "running with velocity " << velocity << std::endl;

  // Read in from files
  for (unsigned int irun = 0; irun < numberOfRuns; irun++)
  {
    std::stringstream fileNameStream;
    fileNameStream << inputDirectory << "/Template_Macro_FullVolume_" << irun << ".root";
    FillFullTimeResidualsPlot(fileNameStream.str(), histogram1, tResidPos, velocity);
  }

  // Have tails flat
  const int upFlatBinLim = histogram1->GetXaxis()->FindBin(upFlatTimeLim);
  const int lowFlatBinLim = histogram1->GetXaxis()->FindBin(lowFlatTimeLim);

  for (int i = 1; i <= histogram1->GetXaxis()->GetNbins(); i++)
  {
    for (int j = 1; j <= histogram1->GetYaxis()->GetNbins(); j++)
    {
      if (histogram1->GetXaxis()->GetBinUpEdge(i) > upFlatTimeLim)
        histogram2->SetBinContent(i, j, histogram1->GetBinContent(upFlatBinLim, j));
      else if (histogram1->GetXaxis()->GetBinLowEdge(i) < lowFlatTimeLim)
        histogram2->SetBinContent(i, j, histogram1->GetBinContent(lowFlatBinLim, j));
      else
        histogram2->SetBinContent(i, j, histogram1->GetBinContent(i, j));
    }
  }

  PrintDirection2DPDFRatdb(material, histogram2, numTimeBins, lowTimeLim, upTimeLim, numAngleBins, lowAngleLim, upAngleLim,
                           tResidPos, evePos, energy);

  histogram2->Draw("colz");
  histogram2->GetXaxis()->SetTitle("Time Residual, ns");

  SaveDirection2DPDFRootFile(material, energy, tResidPos, evePos, histogram1, histogram2);
}

void GetMixedRisetimePDF(std::string material = "labppo_scintillator_mixedrisetime", unsigned int numberOfRuns = 10,
                         double velocity = -999, std::string tResidPos = "FitPos",
                         std::string evePos = "fullvolume", double energy = 6)
{
  const std::string inputDirectoryBase = "/data/snoplus3/parkerw/ratSimulations/May21_risetime";

  const int numTimeBins = 400;      // ns - width of bins in PDF
  const float lowTimeLim = -100.0;  // ns - lower end of PDF
  const float upTimeLim = 300.0;    // ns - upper end of PDF
  const int numAngleBins = 20;      // ns - width of bins in PDF
  const float lowAngleLim = -1.0;   // ns - lower end of PDF
  const float upAngleLim = 1.0;     // ns - upper end of PDF
  const float lowFlatTimeLim = -20; // ns - below this limit, PDF is flat
  const float upFlatTimeLim = 230;  // ns - above this limit, PDF is flat

  // Paired proportions are interpreted as the total contribution from the pair,
  // split equally between the lower and upper risetime samples.
  const std::vector<WeightedRisetimeInput> inputs = {
      {"0p70", 0.02}, {"0p71", 0.02}, {"0p72", 0.02}, {"0p73", 0.02}, {"0p74", 0.02},
      {"0p75", 0.02}, {"0p76", 0.02}, {"0p77", 0.02}, {"0p78", 0.02}, {"0p79", 0.02},
      {"0p80", 0.01}, {"0p81", 0.02}, {"0p82", 0.03}, {"0p83", 0.04}, {"0p84", 0.05},
      {"0p85", 0.50},
      {"0p86", 0.05}, {"0p87", 0.04}, {"0p88", 0.03}, {"0p89", 0.02}, {"0p90", 0.01}};

  TH2D *histogram1 = new TH2D("mixed_mc", "", numTimeBins, lowTimeLim, upTimeLim, numAngleBins, lowAngleLim, upAngleLim);
  TH2D *histogram2 = new TH2D("mixed_pdf", "", numTimeBins, lowTimeLim, upTimeLim, numAngleBins, lowAngleLim, upAngleLim);

  std::cout << "running mixed risetime PDF with velocity " << velocity << std::endl;

  double totalWeight = 0.0;
  for (size_t iInput = 0; iInput < inputs.size(); iInput++)
  {
    const WeightedRisetimeInput &input = inputs.at(iInput);
    totalWeight += input.weight;

    TH2D tempHistogram(("risetime_" + input.suffix).c_str(), "", numTimeBins, lowTimeLim, upTimeLim,
                       numAngleBins, lowAngleLim, upAngleLim);

    for (unsigned int irun = 0; irun < numberOfRuns; irun++)
    {
      std::stringstream fileNameStream;
      fileNameStream << inputDirectoryBase << input.suffix << "/Template_Macro_FullVolume_risetime"
                     << input.suffix << "_" << irun << ".root";
      FillFullTimeResidualsPlot(fileNameStream.str(), &tempHistogram, tResidPos, velocity);
    }

    if (tempHistogram.Integral() <= 0.0)
    {
      std::cerr << "Warning: no entries found for risetime " << input.suffix << std::endl;
      continue;
    }

    histogram1->Add(&tempHistogram, input.weight);
  }

  if (totalWeight <= 0.0)
  {
    std::cerr << "No weighted input was configured for the mixed risetime PDF." << std::endl;
    return;
  }

  if (std::abs(totalWeight - 1.0) > 1e-6)
    histogram1->Scale(1.0 / totalWeight);

  // Have tails flat
  const int upFlatBinLim = histogram1->GetXaxis()->FindBin(upFlatTimeLim);
  const int lowFlatBinLim = histogram1->GetXaxis()->FindBin(lowFlatTimeLim);

  for (int i = 1; i <= histogram1->GetXaxis()->GetNbins(); i++)
  {
    for (int j = 1; j <= histogram1->GetYaxis()->GetNbins(); j++)
    {
      if (histogram1->GetXaxis()->GetBinUpEdge(i) > upFlatTimeLim)
        histogram2->SetBinContent(i, j, histogram1->GetBinContent(upFlatBinLim, j));
      else if (histogram1->GetXaxis()->GetBinLowEdge(i) < lowFlatTimeLim)
        histogram2->SetBinContent(i, j, histogram1->GetBinContent(lowFlatBinLim, j));
      else
        histogram2->SetBinContent(i, j, histogram1->GetBinContent(i, j));
    }
  }

  PrintDirection2DPDFRatdb(material, histogram2, numTimeBins, lowTimeLim, upTimeLim, numAngleBins, lowAngleLim, upAngleLim,
                           tResidPos, evePos, energy);

  histogram2->Draw("colz");
  histogram2->GetXaxis()->SetTitle("Time Residual, ns");

  SaveDirection2DPDFRootFile(material, energy, tResidPos, evePos, histogram1, histogram2);
}

void PrintDirection2DPDFRatdb(std::string material, TH2D *histogram, int numTimeBins, float lowTimeLim, float upTimeLim,
                              int numAngleBins, float lowAngleLim, float upAngleLim, std::string tResidPos,
                              std::string evePos, double energy)
{
  // Print coordinated values to screen in RATDB format
  std::cout << std::endl;
  std::cout << "Please place the text below into the database file: FIT_DIRECTION_2DPDF.ratdb located in rat/data, replacing any existing entry with the same index." << std::endl;
  std::cout << std::endl;
  std::cout << "{" << std::endl;
  std::cout << "type : \"FIT_DIRECTION_2DPDF\"," << std::endl;
  std::cout << "version: 1," << std::endl;
  std::cout << "index: \"" << material << "_" << std::fixed << std::setprecision(2) << energy << "MeV_" << tResidPos << "_" << evePos << "\"," << std::endl;
  std::cout << "run_range: [0, 0]," << std::endl;
  std::cout << "pass: 0," << std::endl;
  std::cout << "production: false," << std::endl;
  std::cout << "timestamp: \"\"," << std::endl;
  std::cout << "comment: \"Generated with RAT " << RAT::GetRATVersion() << "\"," << std::endl;
  std::cout << "time_min_max: [ " << lowTimeLim << ", " << upTimeLim << ", ]," << std::endl;
  std::cout << "time_num_bins: " << numTimeBins << "," << std::endl;
  std::cout << "cos_angle_min_max: [ " << lowAngleLim << ", " << upAngleLim << ", ]," << std::endl;
  std::cout << "cos_angle_num_bins: " << numAngleBins << "," << std::endl;
  std::cout << "probability: [";
  for (int i = 1; i <= histogram->GetXaxis()->GetNbins(); i++)
  {
    for (int j = 1; j <= histogram->GetYaxis()->GetNbins(); j++)
    {
      std::cout << std::fixed << std::setprecision(2) << float(histogram->GetBinContent(i, j)) << ", ";
    }
  }
  std::cout << "]," << std::endl;

  std::cout << "}" << std::endl;
  std::cout << std::endl;
}

void SaveDirection2DPDFRootFile(std::string material, double energy, std::string tResidPos, std::string evePos,
                                TH2D *mcHistogram, TH2D *pdfHistogram)
{
  std::stringstream outname;
  outname << material << "_" << energy << "MeV_" << tResidPos << "_" << evePos << ".root";
  TFile *outfile = TFile::Open(outname.str().c_str(), "RECREATE");
  TString hname1 = "MC";
  TString hname2 = "PDF";
  mcHistogram->Write(hname1);
  pdfHistogram->Write(hname2);
  outfile->Close();
}

// Update the Scintillator Time Residual distribution from a single rootfile
void FillFullTimeResidualsPlot(std::string infile, TH2D *histogram, std::string tResidPos, double velocity)
{
  RAT::DU::DSReader dsReader(infile);

  const RAT::DU::EffectiveVelocity &effectiveVelocity = RAT::DU::Utility::Get()->GetEffectiveVelocity();
  RAT::DU::TimeResidualCalculator timeResCalc = RAT::DU::Utility::Get()->GetTimeResidualCalculator(); // To calculate the light's path
  RAT::LP::LightPathStraightScint::BeginOfRun();
  const RAT::DU::PMTInfo &pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
  for (size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++)
  {
    const RAT::DS::Entry &rDS = dsReader.GetEntry(iEntry);
    for (size_t iEV = 0; iEV < rDS.GetEVCount(); iEV++)
    {
      if (iEV > 0) // ignore retriggers
        continue;
      const RAT::DS::EV &rEV = rDS.GetEV(iEV);

      const RAT::DS::CalPMTs &calibratedPMTs = rEV.GetCalPMTs();
      for (size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++)
      {
        const RAT::DS::PMTCal &pmtCal = calibratedPMTs.GetPMT(iPMT);

        // Get fit position
        const RAT::DS::FitVertex &rVertex = rEV.GetFitResult("multiPDFFit").GetVertex(0);

        TVector3 truePos = rDS.GetMC().GetMCParticle(0).GetPosition();
        double trueTime = 390 - rDS.GetMCEV(0).GetGTTime();
        TVector3 trueDirection = rDS.GetMC().GetMCParticle(0).GetMomentum().Unit();

        TVector3 fitPos = rVertex.GetPosition();
        double fitTime = rVertex.GetTime();

        double distInAV = 0.0;
        double distInWater = 0.0;
        double distInTarget = 0.0;

        TVector3 pmtpos = pmtInfo.GetPosition(pmtCal.GetID());

        RAT::LP::LightPathStraightScint::GetPath(pmtpos, fitPos, distInTarget, distInWater);

        float fitTransitTime = RAT::DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance(distInTarget, distInAV, distInWater);
        if (velocity > 0)
          // We are overriding the velocity in EffectiveVelocity for faster processing
          fitTransitTime = (distInTarget / velocity + distInAV / effectiveVelocity.GetAVVelocity() +
                            distInWater / effectiveVelocity.GetWaterVelocity()) +
                           effectiveVelocity.GetOffset();
        float fitCorrectedTime = pmtCal.GetTime() - fitTransitTime - fitTime;

        RAT::LP::LightPathStraightScint::GetPath(pmtpos, truePos, distInTarget, distInWater);

        float trueTransitTime = RAT::DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance(distInTarget, distInAV, distInWater);
        if (velocity > 0)
          // We are overriding the velocity in EffectiveVelocity for faster processing
          fitTransitTime = (distInTarget / velocity + distInAV / effectiveVelocity.GetAVVelocity() +
                            distInWater / effectiveVelocity.GetWaterVelocity()) +
                           effectiveVelocity.GetOffset();

        // Get true quantities
        float trueCorrectedTime = pmtCal.GetTime() - trueTransitTime - trueTime;
        TVector3 truePhotonDir = (pmtpos - truePos).Unit();
        double trueCosAngle = truePhotonDir.Dot(trueDirection);

        // Calculate time residual fromm fit or true position
        double TimeResidual = -999;
        if (tResidPos == "FitPos")
          TimeResidual = pmtCal.GetTime() - fitTransitTime - fitTransitTime;
          //TimeResidual = timeResCalc.CalcTimeResidual(pmtCal, fitPos, fitTime, true);
        else if (tResidPos == "TruePos")
          TimeResidual = pmtCal.GetTime() - trueTransitTime - trueTime;
          //TimeResidual = timeResCalc.CalcTimeResidual(pmtCal, truePos, trueTime, true);

        histogram->Fill(TimeResidual, trueCosAngle);
      }
      DB::Get()->Clear();
    }
  }
}
