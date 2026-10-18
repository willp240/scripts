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


def and_cut(*cuts):
    cuts = [cut for cut in cuts if cut]
    if not cuts:
        return ""
    return " && ".join("({})".format(cut) for cut in cuts)


def window_cut(branch, low, high):
    return "{} >= {} && {} < {}".format(branch, low, branch, high)


def make_hist(name, title, bins, low, high, color):
    hist = ROOT.TH1D(name, title, bins, low, high)
    hist.SetLineColor(color)
    hist.SetLineWidth(2)
    return hist


def project(chain, hist, branch, cut):
    selected = chain.Project(hist.GetName(), branch, cut)
    if selected < 0:
        raise RuntimeError("Could not draw '{}' with cut '{}'".format(branch, cut))
    hist.SetDirectory(0)
    return selected


def print_window_summary(label, hist, low, high):
    axis = hist.GetXaxis()
    first = axis.FindBin(low)
    last = axis.FindBin(high)
    total = hist.Integral(0, hist.GetNbinsX() + 1)
    below = hist.Integral(0, first - 1)
    inside = hist.Integral(first, last)
    above = hist.Integral(last + 1, hist.GetNbinsX() + 1)
    print(label)
    print("  total:  {}".format(int(total)))
    if total > 0:
        print("  below window:  {:8d} ({:.3%})".format(int(below), below / total))
        print("  inside window: {:8d} ({:.3%})".format(int(inside), inside / total))
        print("  above window:  {:8d} ({:.3%})".format(int(above), above / total))
        print("  mean: {:.5f} MeV".format(hist.GetMean()))
        print("  rms:  {:.5f} MeV".format(hist.GetRMS()))


def count(chain, *cuts):
    """Count entries with exact cut boundaries, independent of histogram bins."""
    return int(chain.GetEntries(and_cut(*cuts)))


def print_exact_accounting(chain, base_cut, true_branch, reco_branch,
                           true_low, true_high, reco_low, reco_high,
                           plot_low, plot_high):
    true_window = window_cut(true_branch, true_low, true_high)
    reco_window = window_cut(reco_branch, reco_low, reco_high)

    true_total = count(chain, base_cut, true_window)
    reco_total = count(chain, base_cut, reco_window)
    true_to_below = count(chain, base_cut, true_window, "{} < {}".format(reco_branch, reco_low))
    true_to_inside = count(chain, base_cut, true_window, reco_window)
    true_to_above = count(chain, base_cut, true_window, "{} >= {}".format(reco_branch, reco_high))
    below_to_reco = count(chain, base_cut, reco_window, "{} < {}".format(true_branch, true_low))
    inside_to_reco = true_to_inside
    above_to_reco = count(chain, base_cut, reco_window, "{} >= {}".format(true_branch, true_high))

    print("Exact migration accounting (half-open windows [low, high)):")
    print("  true-window total: {}".format(true_total))
    print("    reco below / inside / above: {} / {} / {}".format(
        true_to_below, true_to_inside, true_to_above))
    print("  reco-window total: {}".format(reco_total))
    print("    true below / inside / above: {} / {} / {}".format(
        below_to_reco, inside_to_reco, above_to_reco))
    print("  migrated out / migrated in / net: {} / {} / {:+d}".format(
        true_to_below + true_to_above,
        below_to_reco + above_to_reco,
        reco_total - true_total))
    print("  true-window reco outside plot [{}, {}): below={} above={}".format(
        plot_low, plot_high,
        count(chain, base_cut, true_window, "{} < {}".format(reco_branch, plot_low)),
        count(chain, base_cut, true_window, "{} >= {}".format(reco_branch, plot_high))))
    print("  true-window reco non-finite: {}".format(
        count(chain, base_cut, true_window, "!TMath::Finite({})".format(reco_branch))))
    print("  true-window fit/base-cut failures: {}".format(
        count(chain, true_window) - true_total))


def save_hist(hist, output, marker_window=None):
    canvas = ROOT.TCanvas("c_" + hist.GetName(), "c_" + hist.GetName(), 900, 650)
    canvas.SetGrid()
    hist.Draw("hist")
    lines = []
    if marker_window:
        ymax = hist.GetMaximum() * 1.05
        for x in marker_window:
            line = ROOT.TLine(x, 0.0, x, ymax)
            line.SetLineColor(ROOT.kRed + 1)
            line.SetLineStyle(2)
            line.SetLineWidth(2)
            line.Draw("same")
            lines.append(line)
    canvas.SaveAs(output)


def main():
    parser = argparse.ArgumentParser(
        description="Diagnose where Bi210 events migrate in reconstructed/true energy."
    )
    parser.add_argument(
        "inputs",
        nargs="*",
        default=[DEFAULT_INPUT_DIR],
        help="Input ROOT file(s), glob(s), or directories.",
    )
    parser.add_argument("--tree", default="output")
    parser.add_argument("--reco-branch", default="energy")
    parser.add_argument("--true-branch", default="mcke1")
    parser.add_argument("--cut", default="fitValid")
    parser.add_argument("--true-window", nargs=2, type=float, default=(0.09, 0.11))
    parser.add_argument("--reco-window", nargs=2, type=float, default=(0.09, 0.11))
    parser.add_argument("--bins", type=int, default=160)
    parser.add_argument("--emin", type=float, default=0.0)
    parser.add_argument("--emax", type=float, default=1.2)
    parser.add_argument("--out-prefix", default="bi210_migration")
    args = parser.parse_args()

    files = expand_inputs(args.inputs)
    if not files:
        raise RuntimeError("No input ROOT files found")

    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(1110)

    chain = ROOT.TChain(args.tree)
    for path in files:
        chain.Add(path)
    if chain.GetEntries() == 0:
        raise RuntimeError("No entries found in tree '{}'".format(args.tree))

    true_low, true_high = args.true_window
    reco_low, reco_high = args.reco_window

    true_window_selection = and_cut(
        args.cut, window_cut(args.true_branch, true_low, true_high)
    )
    reco_window_selection = and_cut(
        args.cut, window_cut(args.reco_branch, reco_low, reco_high)
    )

    reco_from_true = make_hist(
        "hRecoFromTrueWindow",
        "Reco E for {:.3f} < true E < {:.3f} MeV;Reconstructed Energy [MeV];Events".format(
            true_low, true_high
        ),
        args.bins,
        args.emin,
        args.emax,
        ROOT.kAzure + 2,
    )
    true_from_reco = make_hist(
        "hTrueFromRecoWindow",
        "True E for {:.3f} < reco E < {:.3f} MeV;True Energy [MeV];Events".format(
            reco_low, reco_high
        ),
        args.bins,
        args.emin,
        args.emax,
        ROOT.kOrange + 7,
    )

    n_reco_from_true = project(
        chain, reco_from_true, args.reco_branch, true_window_selection
    )
    n_true_from_reco = project(
        chain, true_from_reco, args.true_branch, reco_window_selection
    )

    reco_from_true_out = "{}_reco_from_true_window.png".format(args.out_prefix)
    true_from_reco_out = "{}_true_from_reco_window.png".format(args.out_prefix)
    save_hist(reco_from_true, reco_from_true_out, marker_window=(reco_low, reco_high))
    save_hist(true_from_reco, true_from_reco_out, marker_window=(true_low, true_high))

    print("Read {} file(s)".format(len(files)))
    print("Tree entries: {}".format(chain.GetEntries()))
    print("Base cut: {}".format(args.cut if args.cut else "<none>"))
    print("True-window selected entries: {}".format(int(n_reco_from_true)))
    print("Reco-window selected entries: {}".format(int(n_true_from_reco)))
    print_window_summary(
        "Where true-window events reconstruct:", reco_from_true, reco_low, reco_high
    )
    print_window_summary(
        "Where reco-window events came from:", true_from_reco, true_low, true_high
    )
    print_exact_accounting(
        chain, args.cut, args.true_branch, args.reco_branch,
        true_low, true_high, reco_low, reco_high, args.emin, args.emax
    )
    print("Wrote {}".format(os.path.abspath(reco_from_true_out)))
    print("Wrote {}".format(os.path.abspath(true_from_reco_out)))


if __name__ == "__main__":
    main()
