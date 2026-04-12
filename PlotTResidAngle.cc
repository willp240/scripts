#include <glob.h>

#include <RAT/DU/DSReader.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/DU/Point3D.hh>
#include <RAT/DU/TimeResidualCalculator.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DS/EV.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/MC.hh>
#include <RAT/DS/PMT.hh>

#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TString.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace
{

const int kNumTimeBins = 400;
const double kTimeMin = -100.0;
const double kTimeMax = 300.0;
const int kNumCosBins = 20;
const double kCosMin = -1.0;
const double kCosMax = 1.0;

std::vector<std::string> ExpandInputFiles(const std::string &input)
{
  glob_t globResult;
  std::vector<std::string> files;

  const int globStatus = glob(input.c_str(), 0, 0, &globResult);
  if (globStatus == 0)
  {
    for (size_t i = 0; i < globResult.gl_pathc; ++i)
      files.push_back(globResult.gl_pathv[i]);
  }
  else if (globStatus == GLOB_NOMATCH)
  {
    files.push_back(input);
  }
  else
  {
    std::cerr << "Failed to expand input pattern: " << input << std::endl;
  }

  globfree(&globResult);
  std::sort(files.begin(), files.end());
  return files;
}

std::string ToString(const char *text)
{
  return text ? std::string(text) : std::string("");
}

std::string BaseNameNoRoot(const std::string &path)
{
  std::string name = path;
  const std::string::size_type slash = name.find_last_of('/');
  if (slash != std::string::npos)
    name = name.substr(slash + 1);
  if (name.size() > 5 && name.substr(name.size() - 5) == ".root")
    name = name.substr(0, name.size() - 5);
  return name;
}

TH1D *MakeTimeResidualHist(const std::string &name, const std::string &title)
{
  return new TH1D(name.c_str(), title.c_str(), kNumTimeBins, kTimeMin, kTimeMax);
}

TH2D *MakeAngleHist(const std::string &name, const std::string &title)
{
  return new TH2D(name.c_str(), title.c_str(),
                  kNumTimeBins, kTimeMin, kTimeMax,
                  kNumCosBins, kCosMin, kCosMax);
}

void NormaliseHist(TH1D *hist)
{
  if (!hist)
    return;
  const double integral = hist->Integral();
  if (integral > 0.0)
    hist->Scale(1.0 / integral);
}

void FillHistogramsFromPattern(const std::string &inputPattern,
                               TH1D *hTResid,
                               TH2D *hAngle)
{
  RAT::DB::Get()->SetAirplaneModeStatus(true);

  const std::vector<std::string> inputFiles = ExpandInputFiles(inputPattern);
  for (size_t iFile = 0; iFile < inputFiles.size(); iFile++)
  {
    std::cout << "Processing " << inputFiles[iFile] << std::endl;
    RAT::DU::DSReader dsReader(inputFiles[iFile]);
    RAT::DU::TimeResidualCalculator timeResCalc = RAT::DU::Utility::Get()->GetTimeResidualCalculator();
    const RAT::DU::PMTInfo &pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo();
    const size_t pmtSystemId = RAT::DU::Point3D::GetSystemId("innerPMT");

    for (size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++)
    {
      const RAT::DS::Entry &rDS = dsReader.GetEntry(iEntry);
      if (!rDS.MCExists() || rDS.GetMCEVCount() == 0 || rDS.GetEVCount() == 0 ||
          rDS.GetMC().GetMCParticleCount() == 0)
      {
        RAT::DB::Get()->Clear();
        continue;
      }

      const TVector3 truePos = rDS.GetMC().GetMCParticle(0).GetPosition();
      const TVector3 trueDirection = rDS.GetMC().GetMCParticle(0).GetMomentum().Unit();
      const RAT::DU::Point3D truePoint(pmtSystemId, truePos);
      const double trueTime = 390.0 - rDS.GetMCEV(0).GetGTTime();

      for (size_t iEV = 0; iEV < rDS.GetEVCount(); iEV++)
      {
        if (iEV > 0)
          continue;

        const RAT::DS::EV &rEV = rDS.GetEV(iEV);
        const RAT::DS::CalPMTs &calibratedPMTs = rEV.GetCalPMTs();
        for (size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++)
        {
          const RAT::DS::PMTCal &pmtCal = calibratedPMTs.GetPMT(iPMT);
          const TVector3 pmtPos = pmtInfo.GetPosition(pmtCal.GetID());
          const double timeResidual =
              timeResCalc.CalcTimeResidual(pmtCal, truePoint, trueTime, true);
          const double trueCosAngle = (pmtPos - truePos).Unit().Dot(trueDirection);

          if (hTResid)
            hTResid->Fill(timeResidual);
          if (hAngle)
            hAngle->Fill(timeResidual, trueCosAngle);
        }
      }

      RAT::DB::Get()->Clear();
    }
  }
}

TH1D *LoadTimeResidualHist(TFile *file)
{
  if (!file)
    return 0;
  TH1D *hist = dynamic_cast<TH1D *>(file->Get("tResidNorm"));
  if (!hist)
    hist = dynamic_cast<TH1D *>(file->Get("tResid"));
  return hist;
}

TH2D *LoadAngleHist(TFile *file)
{
  if (!file)
    return 0;
  TH2D *hist = dynamic_cast<TH2D *>(file->Get("tResidAngle"));
  if (!hist)
    hist = dynamic_cast<TH2D *>(file->Get("newPDF"));
  return hist;
}

} // namespace

void PlotTResidOverlay(const std::vector<std::string> &rootFiles,
                       const std::vector<std::string> &labels,
                       const char *outputPrefix = "tresid_overlay");

void MakeTResidAngleFile(const char *inputPattern, const char *outputPrefix)
{
  const std::string pattern = ToString(inputPattern);
  std::string prefix = ToString(outputPrefix);
  if (prefix.empty())
    prefix = "tresid_angle";

  gStyle->SetOptStat(0);

  TH1D *hTResid = MakeTimeResidualHist("tResid", "Time Residual;Time Residual, ns;Counts");
  TH2D *hAngle = MakeAngleHist("tResidAngle", "cos(#theta) vs Time Residual;Time Residual, ns;cos(#theta)");

  FillHistogramsFromPattern(pattern, hTResid, hAngle);

  TH1D *hTResidNorm = static_cast<TH1D *>(hTResid->Clone("tResidNorm"));
  hTResidNorm->SetTitle("Normalised Time Residual;Time Residual, ns;Normalised counts");
  NormaliseHist(hTResidNorm);

  TCanvas *c1 = new TCanvas((prefix + "_time_canvas").c_str(), "", 900, 700);
  hTResidNorm->SetLineWidth(2);
  hTResidNorm->Draw("hist");

  TCanvas *c2 = new TCanvas((prefix + "_angle_canvas").c_str(), "", 1000, 800);
  hAngle->Draw("lego");

  TFile *file = new TFile((prefix + ".root").c_str(), "RECREATE");
  hTResid->Write();
  hTResidNorm->Write();
  hAngle->Write();
  c1->Write("timeResidualCanvas");
  c2->Write("angleCanvas");
  file->Close();
}

void PlotTResid(const char *inputPattern)
{
  MakeTResidAngleFile(inputPattern, "timeResid2");
}

void PlotAngles(const char *inputPattern)
{
  MakeTResidAngleFile(inputPattern, "timeResidAngle");
}

void CompareTResid(const char *fileName1, const char *fileName2, const char *label1, const char *label2)
{
  std::vector<std::string> files;
  files.push_back(ToString(fileName1));
  files.push_back(ToString(fileName2));
  std::vector<std::string> labels;
  labels.push_back(ToString(label1));
  labels.push_back(ToString(label2));

  PlotTResidOverlay(files, labels, "timeResidComp");
}

void PlotTResidOverlay(const std::vector<std::string> &rootFiles,
                       const std::vector<std::string> &labels,
                       const char *outputPrefix)
{
  gStyle->SetOptStat(0);

  std::string prefix = ToString(outputPrefix);
  if (prefix.empty())
    prefix = "tresid_overlay";

  TCanvas *canvas = new TCanvas((prefix + "_canvas").c_str(), "", 900, 700);
  //TLegend *legend = new TLegend(0.68, 0.68, 0.88, 0.88);
  TLegend *legend = new TLegend(0.48, 0.38, 0.88, 0.58);
  bool first = true;

  const int colors[] = {kBlack, kRed + 1, kBlue + 1, kGreen + 2, kOrange + 7, kMagenta + 2, kCyan + 2};

  TFile *outFile = new TFile((prefix + ".root").c_str(), "RECREATE");

  for (size_t i = 0; i < rootFiles.size(); i++)
  {
    TFile input(rootFiles[i].c_str(), "READ");
    TH1D *source = LoadTimeResidualHist(&input);
    if (!source)
    {
      std::cerr << "Could not find time residual histogram in " << rootFiles[i] << std::endl;
      continue;
    }

    TH1D *hist = static_cast<TH1D *>(source->Clone(TString::Format("tResidOverlay_%zu", i)));
    hist->SetDirectory(0);
    NormaliseHist(hist);
    hist->SetLineWidth(2);
    hist->SetLineStyle(1);
    hist->SetLineColor(colors[i % (sizeof(colors) / sizeof(colors[0]))]);
    hist->SetTitle("Normalised Time Residual Comparison;Time Residual, ns;Normalised Counts");
    hist->GetXaxis()->SetRangeUser(-30,30);
    hist->GetYaxis()->SetTitleOffset(1.2);
    canvas->SetLogy();

    if (first)
    {
      hist->Draw("hist");
      first = false;
    }
    else
    {
      hist->Draw("hist same");
    }

    const std::string label = i < labels.size() ? labels[i] : BaseNameNoRoot(rootFiles[i]);
    legend->AddEntry(hist, label.c_str(), "l");
    outFile->cd();
    hist->Write();
  }

  outFile->cd();
  legend->Draw();
  canvas->Write("overlayCanvas");
  outFile->Close();
  canvas->SaveAs((prefix+".pdf").c_str());
}

void CombineTResidOutputs(const std::vector<std::string> &rootFiles,
                          const char *outputPrefix)
{
  std::string prefix = ToString(outputPrefix);
  if (prefix.empty())
    prefix = "tresid_combined";

  TH1D *combined1D = 0;
  TH2D *combined2D = 0;

  for (size_t i = 0; i < rootFiles.size(); i++)
  {
    TFile input(rootFiles[i].c_str(), "READ");
    TH1D *source1D = LoadTimeResidualHist(&input);
    TH2D *source2D = LoadAngleHist(&input);

    if (!source1D || !source2D)
    {
      std::cerr << "Could not find expected histograms in " << rootFiles[i] << std::endl;
      continue;
    }

    if (!combined1D)
    {
      combined1D = static_cast<TH1D *>(source1D->Clone("tResid"));
      combined1D->SetDirectory(0);
      combined2D = static_cast<TH2D *>(source2D->Clone("tResidAngle"));
      combined2D->SetDirectory(0);
    }
    else
    {
      combined1D->Add(source1D);
      combined2D->Add(source2D);
    }
  }

  if (!combined1D || !combined2D)
    return;

  TH1D *combinedNorm = static_cast<TH1D *>(combined1D->Clone("tResidNorm"));
  combinedNorm->SetDirectory(0);
  combinedNorm->SetTitle("Normalised Time Residual;Time Residual, ns;Normalised counts");
  NormaliseHist(combinedNorm);

  TCanvas *c1 = new TCanvas((prefix + "_time_canvas").c_str(), "", 900, 700);
  combinedNorm->SetLineWidth(2);
  combinedNorm->Draw("hist");

  TCanvas *c2 = new TCanvas((prefix + "_angle_canvas").c_str(), "", 1000, 800);
  combined2D->SetTitle("cos(#theta) vs Time Residual;Time Residual, ns;cos(#theta)");
  combined2D->Draw("lego");

  TFile *outFile = new TFile((prefix + ".root").c_str(), "RECREATE");
  combined1D->Write();
  combinedNorm->Write();
  combined2D->Write();
  c1->Write("timeResidualCanvas");
  c2->Write("angleCanvas");
  outFile->Close();
}

void SaveAngleLegoPdf(const char *inputFileName, const char *outputName)
{
  const std::string inputFile = ToString(inputFileName);
  std::string outputBase = ToString(outputName);
  if (outputBase.empty())
    outputBase = BaseNameNoRoot(inputFile);

  TFile input(inputFile.c_str(), "READ");
  TH2D *source = LoadAngleHist(&input);
  if (!source)
  {
    std::cerr << "Could not find tResidAngle in " << inputFile << std::endl;
    return;
  }

  TH2D *hist = static_cast<TH2D *>(source->Clone("tResidAngleLego"));
  hist->SetDirectory(0);
  hist->GetXaxis()->SetRangeUser(-25,-5);
  hist->GetXaxis()->SetTitleOffset(1.4);
  hist->GetYaxis()->SetTitleOffset(1.6);

  TCanvas *canvas = new TCanvas((outputBase + "_lego_canvas").c_str(), "", 1500, 800);
  hist->Draw("lego");
  canvas->SaveAs((outputBase + ".pdf").c_str());
}
