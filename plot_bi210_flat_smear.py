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
        matches = glob.glob(os.path.join(path, "*.root")) if os.path.isdir(path) else glob.glob(path)
        files.extend(matches if matches else [path])
    return sorted(set(path for path in files if os.path.isfile(path)))


def main():
    parser = argparse.ArgumentParser(description="Smear a flat true-energy spectrum with Bi210 slice response.")
    parser.add_argument("inputs", nargs="*", default=[DEFAULT_INPUT_DIR])
    parser.add_argument("--tree", default="output")
    parser.add_argument("--true-branch", default="mcke1")
    parser.add_argument("--reco-branch", default="energy")
    parser.add_argument("--cut", default="fitValid")
    parser.add_argument("--slice-width", type=float, default=0.02)
    parser.add_argument("--flat-min", type=float, default=0.0)
    parser.add_argument("--flat-max", type=float, default=0.5)
    parser.add_argument("--response-max", type=float, default=1.2)
    parser.add_argument("--events", type=int, default=2000000)
    parser.add_argument("--bins", type=int, default=100)
    parser.add_argument("--seed", type=int, default=210)
    parser.add_argument("-o", "--output", default="bi210_flat_true_reco_smear.png")
    args = parser.parse_args()

    files = expand_inputs(args.inputs)
    nslices = int(round(args.response_max / args.slice_width))
    if abs(nslices * args.slice_width - args.response_max) > 1e-9:
        raise ValueError("response-max must be a multiple of slice-width")
    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(0)
    chain = ROOT.TChain(args.tree)
    for path in files:
        chain.Add(path)

    response = ROOT.TH2D("hFlatResponse", "", nslices, 0, args.response_max,
                         600, 0, args.response_max)
    chain.Project(response.GetName(), "{}:{}".format(args.reco_branch, args.true_branch), args.cut)
    biases, widths = [], []
    for i in range(nslices):
        p = response.ProjectionY("hFlatSlice_{}".format(i), i + 1, i + 1)
        centre = (i + 0.5) * args.slice_width
        biases.append(p.GetMean() - centre)
        widths.append(p.GetRMS())

    flat = ROOT.TH1D("hFlatTrue", "Flat True Energy;Energy [MeV];Events",
                     args.bins, args.flat_min, args.flat_max)
    smeared = flat.Clone("hFlatSmeared")
    smeared.SetTitle("Flat True Energy;Energy [MeV];Events")
    random = ROOT.TRandom3(args.seed)
    for _ in range(args.events):
        true_energy = random.Uniform(args.flat_min, args.flat_max)
        index = min(nslices - 1, max(0, int(true_energy / args.slice_width)))
        reco_energy = true_energy + random.Gaus(biases[index], widths[index])
        flat.Fill(true_energy)
        smeared.Fill(reco_energy)
    flat.SetDirectory(0)
    smeared.SetDirectory(0)
    flat.SetLineColor(ROOT.kOrange + 7)
    smeared.SetLineColor(ROOT.kMagenta + 1)
    flat.SetLineWidth(2)
    smeared.SetLineWidth(2)
    flat.SetMaximum(1.15 * max(flat.GetMaximum(), smeared.GetMaximum()))

    canvas = ROOT.TCanvas("cFlatSmear", "cFlatSmear", 950, 700)
    canvas.SetGrid()
    flat.Draw("hist")
    smeared.Draw("hist same")
    legend = ROOT.TLegend(0.53, 0.73, 0.88, 0.86)
    legend.AddEntry(flat, "Flat True Energy", "l")
    legend.AddEntry(smeared, "Bias Smeared Flat Truth", "l")
    legend.Draw()
    canvas.SaveAs(args.output)
    print("Measured response from {} files".format(len(files)))
    print("Generated {} flat events over [{:.3f}, {:.3f}) MeV".format(
        args.events, args.flat_min, args.flat_max))
    print("Wrote {}".format(os.path.abspath(args.output)))


if __name__ == "__main__":
    main()
