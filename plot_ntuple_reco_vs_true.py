#!/usr/bin/env python

from __future__ import absolute_import, division, print_function

import argparse
import glob
import os

import ROOT


DEFAULT_INPUT_DIR = (
    "/data/snoplus3/parkerw/ratSimulations/Jul8_0to0p5MeVe_ntup_bisMSB_4m"
)


def expand_inputs(inputs):
    files = []
    for path in inputs:
        if os.path.isdir(path):
            matches = glob.glob(os.path.join(path, "*.root"))
        else:
            matches = glob.glob(path)
        files.extend(matches if matches else [path])
    return sorted(set(files))


def existing_files(files):
    return [path for path in files if os.path.isfile(path)]


def build_chain(files, tree_name):
    chain = ROOT.TChain(tree_name)
    for path in files:
        chain.Add(path)
    return chain


def draw_diagonal(xmin, xmax, ymin, ymax):
    lo = max(xmin, ymin)
    hi = min(xmax, ymax)
    line = ROOT.TLine(lo, lo, hi, hi)
    line.SetLineColor(ROOT.kRed + 1)
    line.SetLineWidth(2)
    line.SetLineStyle(2)
    line.Draw("same")
    return line


def main():
    parser = argparse.ArgumentParser(
        description="Plot reconstructed energy vs true energy from RAT ntuple ROOT files."
    )
    parser.add_argument(
        "inputs",
        nargs="*",
        default=[DEFAULT_INPUT_DIR],
        help="Input ROOT file(s), glob(s), or directory/directories of ROOT files.",
    )
    parser.add_argument(
        "-o",
        "--output",
        default="ntuple_reco_vs_true_energy.png",
        help="Output image",
    )
    parser.add_argument("--title", default="Reconstructed vs True Energy", help="Plot title")
    parser.add_argument("--tree", default="output", help="TTree name")
    parser.add_argument(
        "--reco-branch",
        default="energy",
        help="Reconstructed energy branch for y axis",
    )
    parser.add_argument(
        "--true-branch",
        default="mcke1",
        help="True energy branch for x axis",
    )
    parser.add_argument("--xbins", type=int, default=100, help="Number of true-E bins")
    parser.add_argument("--ybins", type=int, default=100, help="Number of reco-E bins")
    parser.add_argument("--xmin", type=float, default=0.0, help="Minimum true energy [MeV]")
    parser.add_argument("--xmax", type=float, default=0.5, help="Maximum true energy [MeV]")
    parser.add_argument("--ymin", type=float, default=0.0, help="Minimum reco energy [MeV]")
    parser.add_argument("--ymax", type=float, default=0.6, help="Maximum reco energy [MeV]")
    parser.add_argument(
        "--cut",
        default="fitValid",
        help='Optional TTree selection, for example "fitValid && posr < 4000".',
    )
    parser.add_argument(
        "--logz",
        action="store_true",
        help="Draw the z axis on a logarithmic scale.",
    )
    parser.add_argument(
        "--no-diagonal",
        action="store_true",
        help="Do not draw the E_reco = E_true guide line.",
    )
    args = parser.parse_args()

    files = existing_files(expand_inputs(args.inputs))
    if not files:
        raise RuntimeError("No input ROOT files found")

    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetPalette(ROOT.kViridis)

    chain = build_chain(files, args.tree)
    if chain.GetEntries() == 0:
        raise RuntimeError(
            "No entries found in tree '{}' for {} file(s)".format(args.tree, len(files))
        )

    hist = ROOT.TH2D(
        "hRecoVsTrueEnergy",
        "{};True Energy [MeV];Reconstructed Energy [MeV];Events".format(args.title),
        args.xbins,
        args.xmin,
        args.xmax,
        args.ybins,
        args.ymin,
        args.ymax,
    )

    expression = "{}:{}".format(args.reco_branch, args.true_branch)
    selected = chain.Project(hist.GetName(), expression, args.cut)
    if selected < 0:
        raise RuntimeError(
            "Could not draw '{}' from tree '{}'".format(expression, chain.GetName())
        )
    hist.SetDirectory(0)

    canvas = ROOT.TCanvas("cRecoVsTrueEnergy", "cRecoVsTrueEnergy", 950, 750)
    canvas.SetGrid()
    canvas.SetRightMargin(0.15)
    canvas.SetLogz(args.logz)
    hist.Draw("colz")

    diagonal = None
    if not args.no_diagonal:
        diagonal = draw_diagonal(args.xmin, args.xmax, args.ymin, args.ymax)

    canvas.SaveAs(args.output)

    print("Read {} file(s)".format(len(files)))
    print("Tree entries: {}".format(chain.GetEntries()))
    print("Selected entries: {}".format(int(selected)))
    print("Histogram entries: {}".format(int(hist.GetEntries())))
    print("X underflow: {}".format(int(hist.GetBinContent(0, 1))))
    print("X overflow: {}".format(int(hist.GetBinContent(hist.GetNbinsX() + 1, 1))))
    print("Wrote {}".format(os.path.abspath(args.output)))


if __name__ == "__main__":
    main()
