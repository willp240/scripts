"""Plot a single saved hit-time-residual histogram."""

import sys

import ROOT


ROOT.gROOT.SetBatch(True)


def plot_hit_time_residuals(input_name, image_name):
    input_file = ROOT.TFile.Open(input_name)
    if not input_file or input_file.IsZombie():
        raise RuntimeError(f"could not open {input_name}")

    residuals = input_file.Get("time_residuals")
    if not residuals:
        raise RuntimeError(f"{input_name} does not contain 'time_residuals'")
    residuals.SetDirectory(0)
    input_file.Close()

    canvas = ROOT.TCanvas("time_residual_canvas", "time_residual_canvas", 900, 650)
    canvas.SetLeftMargin(0.13)
    canvas.SetGrid()

    residuals.SetStats(0)
    residuals.SetTitle("Hit Time Residuals")
    residuals.SetLineColor(ROOT.kBlue + 1)
    residuals.SetLineWidth(2)
    residuals.GetXaxis().SetTitle("Time Residual (ns)")
    residuals.GetXaxis().SetRangeUser(-15, 100)
    residuals.GetYaxis().SetTitle("Counts")
    residuals.GetYaxis().SetTitleOffset(1.25)
    residuals.Draw("HIST")

    canvas.SaveAs(image_name)
    print(f"Entries: {int(residuals.GetEntries())}")
    print(f"Saved plot to {image_name}")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        raise RuntimeError(
            "usage: plot_hit_time_residuals.py INPUT.root [OUTPUT.png]"
        )
    output_name = sys.argv[2] if len(sys.argv) > 2 else "hit_time_residuals.png"
    plot_hit_time_residuals(sys.argv[1], output_name)
