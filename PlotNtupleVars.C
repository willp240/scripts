// make_all_branch_plots.C
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TObjArray.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TString.h"
#include <iostream>

static bool IsNumericLeafType(const TString& t)
{
  // Common ROOT leaf types: Float_t, Double_t, Int_t, UInt_t, Short_t, UShort_t, Char_t, UChar_t, Long64_t, ULong64_t, Bool_t
  // Leaf type names returned by TLeaf::GetTypeName() are usually like "Float_t", "Double_t", "Int_t", ...
  return (t == "Float_t" || t == "Double_t" ||
          t == "Int_t"   || t == "UInt_t"   ||
          t == "Short_t" || t == "UShort_t" ||
          t == "Char_t"  || t == "UChar_t"  ||
          t == "Long64_t"|| t == "ULong64_t"||
          t == "Bool_t"  ||
          t == "Long_t"  || t == "ULong_t");
}

void PlotNtupleVars(const char* infile = "input.root",
                           const char* outpdf = "branch_plots.pdf")
{
  // Style
  gStyle->SetOptStat(0);
  gStyle->SetPalette(51);

  // Open file / tree
  TFile* f = TFile::Open(infile, "READ");
  if (!f || f->IsZombie()) {
    std::cerr << "ERROR: Could not open file: " << infile << "\n";
    return;
  }

  TTree* t = dynamic_cast<TTree*>(f->Get("output"));
  if (!t) {
    std::cerr << "ERROR: Could not find TTree named \"output\" in file.\n";
    f->Close();
    return;
  }

  // Expressions
  const TString xTrueR =
      "sqrt(mcPosx*mcPosx+mcPosy*mcPosy+(mcPosz-184.4)*(mcPosz-184.4))";
  const TString xFitTrueDist =
      "sqrt((posx-mcPosx)*(posx-mcPosx) + (posy-mcPosy)*(posy-mcPosy) + (posz-mcPosz)*(posz-mcPosz))";

  const TString cut =
      "sqrt(posx*posx+posy*posy+(posz-184.4)*(posz-184.4)) < 6000 && "
      "(dcFlagged & 256) == 256 && fitValid == 1 && evIndex == 0";

  // Write to a temporary file first so the requested PDF path is only replaced
  // after ROOT has finalized the document trailer.
  const TString finalPdf(outpdf);
  TString partialPdf = finalPdf;
  if (partialPdf.EndsWith(".pdf")) {
    partialPdf.ReplaceAll(".pdf", ".partial.pdf");
  } else {
    partialPdf += ".partial.pdf";
  }
  gSystem->Unlink(partialPdf);

  // Canvas + open PDF
  TCanvas c("c", "c", 900, 700);
  c.SetRightMargin(0.13);
  c.Print(partialPdf + "["); // open multipage pdf

  // Loop branches
  TObjArray* branches = t->GetListOfBranches();
  int plotCounter = 0;

  for (int ib = 0; ib < branches->GetEntries(); ++ib) {
    TBranch* br = dynamic_cast<TBranch*>(branches->At(ib));
    if (!br) continue;

    // We only handle simple scalar numeric leaves (no arrays/vectors/objects)
    TObjArray* leaves = br->GetListOfLeaves();
    if (!leaves || leaves->GetEntries() < 1) continue;

    // If branch has multiple leaves (e.g. split objects), skip to avoid chaos
    if (leaves->GetEntries() != 1) continue;

    TLeaf* leaf = dynamic_cast<TLeaf*>(leaves->At(0));
    if (!leaf) continue;

    // Skip arrays
    if (leaf->GetLenStatic() > 1) continue;

    // Skip non-numeric types
    TString typeName = leaf->GetTypeName();
    if (!IsNumericLeafType(typeName)) continue;

    // Use leaf name as the expression (usually same as branch for simple cases)
    TString yExpr = leaf->GetName();

    // Exclude sentinel default values from plotting/ranging.
    const TString validY = TString::Format("(%s < 20000 && %s > -20000)", yExpr.Data(), yExpr.Data());

    // Auto y-range from valid entries only.
    double yMin = 0.0;
    double yMax = 0.0;
    {
      const Long64_t nSel = t->Draw(yExpr, validY, "goff");
      if (nSel > 0) {
        const double* vals = t->GetV1();
        yMin = vals[0];
        yMax = vals[0];
        for (Long64_t i = 1; i < nSel; ++i) {
          if (vals[i] < yMin) yMin = vals[i];
          if (vals[i] > yMax) yMax = vals[i];
        }
      } else {
        // No valid entries for this branch, skip plotting it.
        continue;
      }
    }
    if (!(yMin < yMax)) { // handle constant / weird
      yMin -= 1.0;
      yMax += 1.0;
    } else {
      // Add a small padding so points aren’t glued to the edges
      const double pad = 0.02 * (yMax - yMin);
      yMin -= pad;
      yMax += pad;
    }
    std::cout << yExpr << " " << yMin << " " << yMax << std::endl;

    // Histogram binning (tweak if you like)
    const int xBins = 100;
    const int yBins = 100;
    const double xMin = 0.0;
    const double xMax = 14000.0;

    auto draw2D = [&](const TString& xExpr,
                      const TString& xLabel,
                      const TString& selection,
                      int whichPlot)
    {
      ++plotCounter;
      TString hname;
      hname.Form("h2_%d_%s_p%d", plotCounter, br->GetName(), whichPlot);

      // Always apply valid-y filter so sentinel defaults are excluded.
      TString totalSelection = validY;
      if (selection.Length()) {
        totalSelection = TString::Format("(%s) && (%s)", validY.Data(), selection.Data());
      }

      // Recalculate y-range for cut plots using the post-cut sample.
      double yMinPlot = yMin;
      double yMaxPlot = yMax;
      if (selection.Length()) {
        const Long64_t nSel = t->Draw(yExpr, totalSelection, "goff");
        if (nSel <= 0) return;
        const double* vals = t->GetV1();
        yMinPlot = vals[0];
        yMaxPlot = vals[0];
        for (Long64_t i = 1; i < nSel; ++i) {
          if (vals[i] < yMinPlot) yMinPlot = vals[i];
          if (vals[i] > yMaxPlot) yMaxPlot = vals[i];
        }
        if (!(yMinPlot < yMaxPlot)) {
          yMinPlot -= 1.0;
          yMaxPlot += 1.0;
        } else {
          const double pad = 0.02 * (yMaxPlot - yMinPlot);
          yMinPlot -= pad;
          yMaxPlot += pad;
        }
      }

      TH2F* h = new TH2F(hname, "", xBins, xMin, xMax, yBins, yMinPlot, yMaxPlot);

      // Axis labels (title is set after filling so we can include integral)
      h->GetXaxis()->SetTitle(xLabel);
      h->GetYaxis()->SetTitle(yExpr);

      // Draw into hist
      TString drawCmd = TString::Format("%s:%s>>%s", yExpr.Data(), xExpr.Data(), hname.Data());
      t->Draw(drawCmd, totalSelection, "goff");

      // Title with integral from filled histogram.
      const double integral = h->Integral();
      TString title = TString::Format("%s vs %s%s (Integral=%.0f)",
                                      yExpr.Data(),
                                      xLabel.Data(),
                                      selection.Length() ? " (cut)" : "",
                                      integral);
      h->SetTitle(title);

      // Draw & save page
      c.Clear();
      h->Draw("colz");
      c.Print(partialPdf);

      delete h;
    };

    // Plot 1: y vs True Radius (no cut)
    draw2D(xTrueR, "True Radius, mm", "", 1);

    // Plot 2: y vs True Radius (with cut)
    draw2D(xTrueR, "True Radius, mm", cut, 2);

    // Plot 3: y vs Fit-True Distance (no cut)
    draw2D(xFitTrueDist, "Fit-True Distance, mm", "", 3);

    // Plot 4: y vs Fit-True Distance (with cut)
    draw2D(xFitTrueDist, "Fit-True Distance, mm", cut, 4);
  }

  // Close PDF
  c.Print(partialPdf + "]");

  gSystem->Unlink(finalPdf);
  if (gSystem->Rename(partialPdf, finalPdf) != 0) {
    std::cerr << "ERROR: Could not rename temporary PDF " << partialPdf
              << " to final output " << finalPdf << "\n";
    f->Close();
    return;
  }

  f->Close();
  std::cout << "Done. Wrote: " << outpdf << "\n";
}
