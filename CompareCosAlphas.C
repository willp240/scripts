#include <algorithm>
#include <iostream>
#include <string>

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TTree.h"

namespace {

const int kNFiles = 2;
const int kNBins = 40;
const double kXMin = -1.0;
const double kXMax = 1.0;

const char *kCosAlpha =
  "(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)"
  "/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)"
  "*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue))";

void DrawCosAlphaSet(const char *canvasName,
                     const char *plotTitle,
                     const char *outputStem,
                     const char *filenames[kNFiles],
                     const char *labels[kNFiles])
{
  gStyle->SetOptStat(0);
  gStyle->SetFrameLineWidth(2);

  //int colours[kNFiles] = {kBlack, kBlack, kRed + 1, kRed + 1, kBlue + 1, kBlue + 1};
  //int colours[kNFiles] = {kBlack, kBlue + 2, kMagenta + 2, kRed + 2, kYellow + 2, kGreen + 2, kCyan + 2};
  int colours[kNFiles] = {kBlack, kRed + 1};
  //'int lineStyles[kNFiles] = {2, 1, 2, 1, 2, 1};
  //int lineStyles[kNFiles] = {2, 1, 1, 1, 1, 1, 1};
  int lineStyles[kNFiles] = {2, 1};

  TCanvas *canvas = new TCanvas(canvasName, canvasName, 1000, 600);
  //TLegend *legend = new TLegend(0.18, 0.68, 0.55, 0.86);
  TLegend *legend = new TLegend(0.18, 0.38, 0.55, 0.86);
  legend->SetLineWidth(2);

  TH1D *hists[kNFiles] = {nullptr};
  double maxY = 0.0;

  for (int i = 0; i < kNFiles; ++i) {
    TFile *file = TFile::Open(filenames[i], "READ");
    if (!file || file->IsZombie()) {
      std::cerr << "Could not open " << filenames[i] << std::endl;
      continue;
    }

    TTree *tree = dynamic_cast<TTree *>(file->Get("eveTree"));
    if (!tree) {
      std::cerr << "Could not find eveTree in " << filenames[i] << std::endl;
      continue;
    }

    std::string histName = std::string(canvasName) + "_h" + std::to_string(i);
    hists[i] = new TH1D(histName.c_str(), plotTitle, kNBins, kXMin, kXMax);
    hists[i]->Sumw2();

    std::string drawCommand = std::string(kCosAlpha) + ">>" + histName;
    tree->Draw(drawCommand.c_str(), "", "goff");
    hists[i]->SetDirectory(nullptr);
    std::cout << filenames[i] << ": " << hists[i]->Integral()
              << " entries in cos alpha histogram" << std::endl;
    const double integral = hists[i]->Integral();
    if (integral > 0.0) {
      hists[i]->Scale(1.0 / integral);
    }

    hists[i]->GetXaxis()->SetTitle("cos (#alpha)");
    hists[i]->GetYaxis()->SetTitle("Probability");
    hists[i]->GetYaxis()->SetTitleOffset(1.3);
    hists[i]->SetLineColor(colours[i]);
    hists[i]->SetLineWidth(2);
    hists[i]->SetLineStyle(lineStyles[i]);

    maxY = std::max(maxY, hists[i]->GetMaximum());
    legend->AddEntry(hists[i], labels[i], "l");
  }

  bool firstDrawn = false;
  for (int i = 0; i < kNFiles; ++i) {
    if (!hists[i]) continue;
    //hists[i]->GetYaxis()->SetRangeUser(0.0, 1.15 * maxY);
    hists[i]->GetYaxis()->SetRangeUser(0.0, 0.3);
    hists[i]->Draw(firstDrawn ? "E1 same" : "E1");
    firstDrawn = true;
  }

  legend->Draw("same");
  canvas->SaveAs((std::string(outputStem) + ".pdf").c_str());
  canvas->SaveAs((std::string(outputStem) + ".png").c_str());
}

}
