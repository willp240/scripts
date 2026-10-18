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


def project_hist(chain, name, branch, bins, emin, emax, cut, color, title):
    hist = ROOT.TH1D(
        name,
        "{};Energy [MeV];Events".format(title),
        bins,
        emin,
        emax,
    )
    hist.SetLineWidth(2)
    hist.SetLineColor(color)

    selected = chain.Project(hist.GetName(), branch, cut)
    if selected < 0:
        raise RuntimeError(
            "Could not draw branch '{}' from tree '{}'".format(
                branch, chain.GetName()
            )
        )
    hist.SetDirectory(0)
    return hist


def main():
    parser = argparse.ArgumentParser(
        description="Plot true and reconstructed energy from RAT ntuple ROOT files."
    )
    parser.add_argument(
        "inputs",
        nargs="*",
        default=[DEFAULT_INPUT_DIR],
        help="Input ROOT file(s), glob(s), or directory/directories of ROOT files.",
    )
    parser.add_argument("-o", "--output", default="ntuple_energy.png", help="Output image")
    parser.add_argument(
        "--title",
        default="Energy",
        help="Plot title",
    )
    parser.add_argument("--tree", default="output", help="TTree name")
    parser.add_argument(
        "--reco-branch",
        default="energy",
        help="Reconstructed energy branch to plot",
    )
    parser.add_argument(
        "--true-branch",
        default="mcke1",
        help="True energy branch to plot",
    )
    parser.add_argument("--bins", type=int, default=100, help="Number of histogram bins")
    parser.add_argument("--emin", type=float, default=0.0, help="Minimum energy [MeV]")
    parser.add_argument("--emax", type=float, default=0.5, help="Maximum energy [MeV]")
    parser.add_argument(
        "--cut",
        default="",
        help='Optional TTree selection, for example "fitValid && posr < 4000".',
    )
    parser.add_argument(
        "--logy",
        action="store_true",
        help="Draw the y axis on a logarithmic scale.",
    )
    args = parser.parse_args()

    files = existing_files(expand_inputs(args.inputs))
    if not files:
        raise RuntimeError("No input ROOT files found")

    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(0)
    ROOT.TH1.SetDefaultSumw2()

    chain = build_chain(files, args.tree)
    if chain.GetEntries() == 0:
        raise RuntimeError(
            "No entries found in tree '{}' for {} file(s)".format(args.tree, len(files))
        )

    reco_hist = project_hist(
        chain,
        "hRecoEnergy",
        args.reco_branch,
        args.bins,
        args.emin,
        args.emax,
        args.cut,
        ROOT.kAzure + 2,
        args.title,
    )
    true_hist = project_hist(
        chain,
        "hTrueEnergy",
        args.true_branch,
        args.bins,
        args.emin,
        args.emax,
        args.cut,
        ROOT.kOrange + 7,
        args.title,
    )

    canvas = ROOT.TCanvas("cEnergy", "cEnergy", 900, 650)
    canvas.SetGrid()
    canvas.SetLogy(args.logy)

    max_y = max(reco_hist.GetMaximum(), true_hist.GetMaximum())
    reco_hist.SetMaximum(max_y * (10.0 if args.logy else 1.15))
    reco_hist.Draw("hist")
    true_hist.Draw("hist same")

    legend = ROOT.TLegend(0.4, 0.4, 0.65, 0.55)
    legend.SetBorderSize(1)
    legend.SetFillColor(ROOT.kWhite)
    legend.SetFillStyle(1001)
    legend.SetTextColor(ROOT.kBlack)
    legend.AddEntry(true_hist, "True", "l")
    legend.AddEntry(reco_hist, "Reconstructed", "l")
    legend.Draw()

    canvas.SaveAs(args.output)

    print("Read {} file(s)".format(len(files)))
    print("Tree entries: {}".format(chain.GetEntries()))
    print("True entries: {}".format(int(true_hist.GetEntries())))
    print("Reconstructed entries: {}".format(int(reco_hist.GetEntries())))
    print("True underflow: {}".format(int(true_hist.GetBinContent(0))))
    print("True overflow: {}".format(int(true_hist.GetBinContent(true_hist.GetNbinsX() + 1))))
    print("Reconstructed underflow: {}".format(int(reco_hist.GetBinContent(0))))
    print(
        "Reconstructed overflow: {}".format(
            int(reco_hist.GetBinContent(reco_hist.GetNbinsX() + 1))
        )
    )
    print("Wrote {}".format(os.path.abspath(args.output)))


if __name__ == "__main__":
    main()
