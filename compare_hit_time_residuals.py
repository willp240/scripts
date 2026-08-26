"""Overlay saved online and airplane time-residual histograms."""

import sys

import ROOT

ROOT.gROOT.SetBatch(True)


def read_histogram(file_name):
    input_file = ROOT.TFile.Open(file_name)
    if not input_file or input_file.IsZombie():
        raise RuntimeError(f"could not open {file_name}")
    histogram = input_file.Get("time_residuals")
    if not histogram:
        raise RuntimeError(f"{file_name} does not contain 'time_residuals'")
    histogram.SetDirectory(0)
    input_file.Close()
    return histogram


def compare_hit_time_residuals(online_name, airplane_name, image_name):
    online = read_histogram(online_name)
    airplane = read_histogram(airplane_name)
    online.SetName("time_residuals_online")
    airplane.SetName("time_residuals_airplane")

    if online.GetNbinsX() != airplane.GetNbinsX():
        raise RuntimeError("histograms have different binning")

    canvas = ROOT.TCanvas("comparison", "comparison", 900, 800)
    upper = ROOT.TPad("upper", "upper", 0.0, 0.25, 1.0, 1.0)
    lower = ROOT.TPad("lower", "lower", 0.0, 0.0, 1.0, 0.25)
    upper.SetBottomMargin(0.02)
    lower.SetTopMargin(0.03)
    lower.SetBottomMargin(0.30)
    upper.SetGrid()
    lower.SetGrid()
    upper.Draw()
    lower.Draw()

    upper.cd()
    online.SetStats(0)
    online.SetTitle("")
    online.SetLineColor(ROOT.kBlue + 1)
    online.SetLineWidth(2)
    airplane.SetLineColor(ROOT.kRed + 1)
    airplane.SetLineStyle(2)
    airplane.SetLineWidth(2)
    online.GetYaxis().SetTitle("Counts")
    online.GetYaxis().SetTitleOffset(1.25)
    online.GetXaxis().SetTitle("")
    online.GetXaxis().SetLabelSize(0.0)
    online.GetXaxis().SetRangeUser(-100, 300)
    online.Draw("HIST")
    airplane.Draw("HIST SAME")
    legend = ROOT.TLegend(0.58, 0.72, 0.88, 0.88)
    legend.AddEntry(online, "Online", "l")
    legend.AddEntry(airplane, "Airplane Mode", "l")
    legend.Draw()

    lower.cd()
    difference = airplane.Clone("time_residual_difference")
    difference.Add(online, -1.0)
    difference.SetStats(0)
    difference.SetTitle("")
    difference.SetLineColor(ROOT.kBlack)
    difference.SetLineWidth(2)
    difference.GetXaxis().SetTitle("Time Residuals (ns)")
    difference.GetXaxis().SetRangeUser(-100, 300)
    difference.GetYaxis().SetRangeUser(-0.12, 0.12)
    difference.GetYaxis().SetNdivisions(505)
    difference.GetYaxis().SetTitle("Airplane - Online")
    difference.GetYaxis().SetTitleSize(0.09)
    difference.GetYaxis().SetTitleOffset(0.5)
    difference.GetYaxis().SetLabelSize(0.08)
    difference.GetXaxis().SetTitleSize(0.11)
    difference.GetXaxis().SetLabelSize(0.09)
    difference.Draw("HIST")

    canvas.Update()
    canvas.SaveAs(image_name)
    print(f"Online entries: {int(online.GetEntries())}")
    print(f"Airplane entries: {int(airplane.GetEntries())}")
    print(f"Largest absolute bin difference: {max(abs(difference.GetBinContent(i)) for i in range(difference.GetNbinsX() + 2)):.0f}")
    print(f"Saved comparison to {image_name}")
    return canvas, online, airplane, difference


if __name__ == "__main__":
    if len(sys.argv) < 3:
        raise RuntimeError(
            "usage: compare_hit_time_residuals.py ONLINE.root AIRPLANE.root [OUTPUT.png]"
        )
    image_name = sys.argv[3] if len(sys.argv) > 3 else "time_residual_comparison.png"
    result = compare_hit_time_residuals(sys.argv[1], sys.argv[2], image_name)
