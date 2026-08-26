#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TPad.h"
#include "TTree.h"

void DiagnosticCosAlpha()
{
  const char *expr =
    "(xdirFit*xdirTrue + ydirFit*ydirTrue + zdirFit*zdirTrue)"
    "/(sqrt(xdirFit*xdirFit + ydirFit*ydirFit + zdirFit*zdirFit)"
    "*sqrt(xdirTrue*xdirTrue + ydirTrue*ydirTrue + zdirTrue*zdirTrue))";

  TFile fMulti("May26_seedposition_default_multipdfseed_Tree.root");
  TFile fDefault("May26_seedposition_default_defaultseed_Tree.root");
  TTree *tMulti = (TTree *)fMulti.Get("eveTree");
  TTree *tDefault = (TTree *)fDefault.Get("eveTree");

  TH1D hMulti("hMulti", ";cos (#alpha);Probability", 40, -1.0, 1.0);
  TH1D hDefault("hDefault", ";cos (#alpha);Probability", 40, -1.0, 1.0);

  tMulti->Draw(Form("%s>>hMulti", expr), "", "goff");
  tDefault->Draw(Form("%s>>hDefault", expr), "", "goff");

  hMulti.Scale(1.0 / hMulti.Integral());
  hDefault.Scale(1.0 / hDefault.Integral());

  hMulti.SetLineColor(kBlack);
  hMulti.SetLineStyle(2);
  hMulti.SetLineWidth(2);
  hDefault.SetLineColor(kRed + 1);
  hDefault.SetLineWidth(2);

  TH1D hDiff("hDiff", ";cos (#alpha);Default - MultiPDF", 40, -1.0, 1.0);
  hDiff.Add(&hDefault, &hMulti, 1.0, -1.0);
  hDiff.SetLineColor(kBlue + 1);
  hDiff.SetLineWidth(2);

  TCanvas c("cDiagnosticCosAlpha", "cDiagnosticCosAlpha", 1000, 800);
  TPad pTop("pTop", "pTop", 0.0, 0.28, 1.0, 1.0);
  TPad pBottom("pBottom", "pBottom", 0.0, 0.0, 1.0, 0.28);
  pTop.SetBottomMargin(0.02);
  pBottom.SetTopMargin(0.04);
  pBottom.SetBottomMargin(0.28);
  pTop.Draw();
  pBottom.Draw();

  pTop.cd();
  hMulti.GetYaxis()->SetRangeUser(0.0, 1.12 * std::max(hMulti.GetMaximum(), hDefault.GetMaximum()));
  hMulti.Draw("hist");
  hDefault.Draw("hist same");
  TLegend legend(0.18, 0.68, 0.55, 0.86);
  legend.SetLineWidth(2);
  legend.AddEntry(&hMulti, "Recon. Pos. Seed", "l");
  legend.AddEntry(&hDefault, "True Pos. Seed", "l");
  legend.Draw("same");

  pBottom.cd();
  hDiff.Draw("hist");
  c.SaveAs("May26SeedPosition_CosAlpha_diagnostic.pdf");
  c.SaveAs("May26SeedPosition_CosAlpha_diagnostic.png");
}
