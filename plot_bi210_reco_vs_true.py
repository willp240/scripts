#!/usr/bin/env python

from __future__ import absolute_import, division, print_function

import argparse
import glob
import os

import ROOT


DEFAULT_INPUT_DIR = "/data/snoplus3/SNOplusData/production/rat-8.0.1/ntuple/Bi210"


def expand_inputs(inputs):
    files = []
    for path in inputs:
        if os.path.isdir(path):
            matches = glob.glob(os.path.join(path, "*.root"))
        else:
            matches = glob.glob(path)
        files.extend(matches if matches else [path])
    return sorted(set(path for path in files if os.path.isfile(path)))


def draw_vertical_line(x, ymin, ymax, color):
    line = ROOT.TLine(x, ymin, x, ymax)
    line.SetLineColor(color)
    line.SetLineWidth(2)
    line.SetLineStyle(2)
    line.Draw("same")
    return line


def default_slice_output(output):
    stem, ext = os.path.splitext(output)
    return "{}_true_slice{}".format(stem, ext or ".png")


def default_overlay_output(output):
    stem, ext = os.path.splitext(output)
    return "{}_true_reco_overlay{}".format(stem, ext or ".png")


def main():
    parser = argparse.ArgumentParser(
        description="Plot reconstructed vs true energy for Bi210 production ntuples."
    )
    parser.add_argument(
        "inputs",
        nargs="*",
        default=[DEFAULT_INPUT_DIR],
        help="Input ROOT file(s), glob(s), or directories.",
    )
    parser.add_argument("-o", "--output", default="bi210_reco_vs_true.png")
    parser.add_argument("--tree", default="output")
    parser.add_argument("--reco-branch", default="energy")
    parser.add_argument("--true-branch", default="mcke1")
    parser.add_argument("--cut", default="fitValid")
    parser.add_argument("--xbins", type=int, default=120)
    parser.add_argument("--ybins", type=int, default=140)
    parser.add_argument("--xmin", type=float, default=0.0)
    parser.add_argument("--xmax", type=float, default=1.2)
    parser.add_argument("--ymin", type=float, default=0.0)
    parser.add_argument("--ymax", type=float, default=1.2)
    parser.add_argument("--logz", action="store_true")
    parser.add_argument(
        "--true-window",
        nargs=2,
        type=float,
        metavar=("LOW", "HIGH"),
        default=(0.09, 0.11),
        help="Draw vertical guide lines around this true-energy window.",
    )
    parser.add_argument(
        "--slice-output",
        default=None,
        help="Output image for the reconstructed-energy projection in --true-window.",
    )
    parser.add_argument(
        "--overlay-output",
        default=None,
        help="Output image for the 1D true/reconstructed energy overlay.",
    )
    parser.add_argument(
        "--overlay-bins",
        type=int,
        default=120,
        help="Number of bins for the 1D true/reconstructed energy overlay.",
    )
    parser.add_argument("--logy", action="store_true", help="Use log y scale for the 1D overlay.")
    args = parser.parse_args()

    files = expand_inputs(args.inputs)
    if not files:
        raise RuntimeError("No input ROOT files found")

    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetPalette(ROOT.kViridis)

    chain = ROOT.TChain(args.tree)
    for path in files:
        chain.Add(path)

    if chain.GetEntries() == 0:
        raise RuntimeError("No entries found in tree '{}'".format(args.tree))

    hist = ROOT.TH2D(
        "hBi210RecoVsTrue",
        "Bi210 MC;True Energy [MeV];Reconstructed Energy [MeV];Events",
        args.xbins,
        args.xmin,
        args.xmax,
        args.ybins,
        args.ymin,
        args.ymax,
    )

    # ROOT draw syntax is y:x, so this fills x=true energy and y=reconstructed energy.
    expression = "{}:{}".format(args.reco_branch, args.true_branch,)
    selected = chain.Project(hist.GetName(), expression, args.cut)
    if selected < 0:
        raise RuntimeError("Could not draw '{}'".format(expression))
    hist.SetDirectory(0)

    canvas = ROOT.TCanvas("cBi210RecoVsTrue", "cBi210RecoVsTrue", 950, 750)
    canvas.SetGrid()
    canvas.SetRightMargin(0.15)
    canvas.SetLogz(args.logz)
    hist.Draw("colz")

    guides = []
    if args.true_window:
        guides.append(draw_vertical_line(args.true_window[0], args.ymin, args.ymax, ROOT.kRed + 1))
        guides.append(draw_vertical_line(args.true_window[1], args.ymin, args.ymax, ROOT.kRed + 1))

    canvas.SaveAs(args.output)

    slice_output = args.slice_output or default_slice_output(args.output)
    xbin_low = hist.GetXaxis().FindBin(args.true_window[0])
    xbin_high = hist.GetXaxis().FindBin(args.true_window[1])
    reco_slice = hist.ProjectionY("hRecoForTrueWindow", xbin_low, xbin_high)
    reco_slice.SetDirectory(0)
    reco_slice.SetTitle(
        "Bi210 MC, {:.3f} < true E < {:.3f} MeV;Reconstructed Energy [MeV];Events".format(
            args.true_window[0], args.true_window[1]
        )
    )
    slice_canvas = ROOT.TCanvas("cRecoForTrueWindow", "cRecoForTrueWindow", 900, 650)
    slice_canvas.SetGrid()
    reco_slice.SetLineWidth(2)
    reco_slice.SetLineColor(ROOT.kAzure + 2)
    reco_slice.Draw("hist")
    slice_canvas.SaveAs(slice_output)

    overlay_output = args.overlay_output or default_overlay_output(args.output)
    overlay_min = min(args.xmin, args.ymin)
    overlay_max = max(args.xmax, args.ymax)
    true_hist = ROOT.TH1D(
        "hBi210TrueEnergy",
        "Bi210 MC;Energy [MeV];Events",
        args.overlay_bins,
        overlay_min,
        overlay_max,
    )
    reco_hist = ROOT.TH1D(
        "hBi210RecoEnergy",
        "Bi210 MC;Energy [MeV];Events",
        args.overlay_bins,
        overlay_min,
        overlay_max,
    )
    true_selected = chain.Project(true_hist.GetName(), args.true_branch, args.cut)
    reco_selected = chain.Project(reco_hist.GetName(), args.reco_branch, args.cut)
    true_hist.SetDirectory(0)
    reco_hist.SetDirectory(0)
    true_hist.SetLineWidth(2)
    reco_hist.SetLineWidth(2)
    true_hist.SetLineColor(ROOT.kOrange + 7)
    reco_hist.SetLineColor(ROOT.kAzure + 2)

    overlay_canvas = ROOT.TCanvas("cBi210TrueRecoOverlay", "cBi210TrueRecoOverlay", 900, 650)
    overlay_canvas.SetGrid()
    overlay_canvas.SetLogy(args.logy)
    max_y = max(true_hist.GetMaximum(), reco_hist.GetMaximum())
    true_hist.SetMaximum(max_y * (10.0 if args.logy else 1.15))
    true_hist.Draw("hist")
    reco_hist.Draw("hist same")
    legend = ROOT.TLegend(0.55, 0.70, 0.82, 0.84)
    legend.SetBorderSize(1)
    legend.SetFillColor(ROOT.kWhite)
    legend.SetFillStyle(1001)
    legend.AddEntry(true_hist, "True", "l")
    legend.AddEntry(reco_hist, "Reconstructed", "l")
    legend.Draw()
    overlay_canvas.SaveAs(overlay_output)

    print("Read {} file(s)".format(len(files)))
    print("Tree entries: {}".format(chain.GetEntries()))
    print("Selected entries: {}".format(int(selected)))
    print("Histogram entries: {}".format(int(hist.GetEntries())))
    print("Cut: {}".format(args.cut if args.cut else "<none>"))
    print("Wrote {}".format(os.path.abspath(args.output)))
    print(
        "True-window projection entries ({:.3f}, {:.3f}): {}".format(
            args.true_window[0], args.true_window[1], int(reco_slice.GetEntries())
        )
    )
    print("Wrote {}".format(os.path.abspath(slice_output)))
    print("True overlay entries: {}".format(int(true_selected)))
    print("Reco overlay entries: {}".format(int(reco_selected)))
    print("True overlay underflow: {}".format(int(true_hist.GetBinContent(0))))
    print("True overlay overflow: {}".format(int(true_hist.GetBinContent(true_hist.GetNbinsX() + 1))))
    print("Reco overlay underflow: {}".format(int(reco_hist.GetBinContent(0))))
    print("Reco overlay overflow: {}".format(int(reco_hist.GetBinContent(reco_hist.GetNbinsX() + 1))))
    print("Wrote {}".format(os.path.abspath(overlay_output)))


if __name__ == "__main__":
    main()
