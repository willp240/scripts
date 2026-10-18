#!/usr/bin/env python

from __future__ import absolute_import, division, print_function

import argparse
import glob
import os

import ROOT
import rat


def iter_files(patterns):
    files = []
    for pattern in patterns:
        matches = sorted(glob.glob(pattern))
        files.extend(matches if matches else [pattern])
    return files


def fill_hist(hist, file_name, fit_name):
    stats = {
        "seen": 0,
        "filled": 0,
        "missing_fit": 0,
        "missing_energy": 0,
        "invalid_energy": 0,
        "underflow": 0,
        "overflow": 0,
    }
    for ds, _ in rat.dsreader(file_name):
        for iev in range(ds.GetEVCount()):
            stats["seen"] += 1
            ev = ds.GetEV(iev)
            if not ev.FitResultExists(fit_name):
                stats["missing_fit"] += 1
                continue
            vertex = ev.GetFitResult(fit_name).GetVertex(0)
            if not vertex.ContainsEnergy():
                stats["missing_energy"] += 1
                continue
            if not vertex.ValidEnergy():
                stats["invalid_energy"] += 1
                continue
            energy = vertex.GetEnergy()
            hist.Fill(energy)
            stats["filled"] += 1
            if energy < hist.GetXaxis().GetXmin():
                stats["underflow"] += 1
            elif energy >= hist.GetXaxis().GetXmax():
                stats["overflow"] += 1
    return stats


def main():
    parser = argparse.ArgumentParser(
        description="Plot reconstructed energy from RAT DS files."
    )
    parser.add_argument("files", nargs="+", help="Input ROOT file(s), globs allowed")
    parser.add_argument("-o", "--output", default="reco_energy.png", help="Output image")
    parser.add_argument("--fit", default="scintFitter", help="Fit result name")
    parser.add_argument("--bins", type=int, default=100, help="Number of bins")
    parser.add_argument("--emin", type=float, default=0.0, help="Minimum energy [MeV]")
    parser.add_argument("--emax", type=float, default=1.0, help="Maximum energy [MeV]")
    args = parser.parse_args()

    files = iter_files(args.files)
    if not files:
        raise RuntimeError("No input files found")

    ROOT.gROOT.SetBatch(True)
    hist = ROOT.TH1D(
        "hRecoE",
        "Reconstructed energy;Reconstructed E [MeV];Number of events",
        args.bins,
        args.emin,
        args.emax,
    )
    hist.SetDirectory(0)

    total = {
        "seen": 0,
        "filled": 0,
        "missing_fit": 0,
        "missing_energy": 0,
        "invalid_energy": 0,
        "underflow": 0,
        "overflow": 0,
    }
    for file_name in files:
        stats = fill_hist(hist, file_name, args.fit)
        for key, value in stats.items():
            total[key] += value
        print(
            "{}: filled {filled} / {seen} triggered events "
            "(missing fit: {missing_fit}, missing E: {missing_energy}, "
            "invalid E: {invalid_energy}, underflow: {underflow}, overflow: {overflow})"
            .format(file_name, **stats)
        )

    canvas = ROOT.TCanvas("cRecoE", "cRecoE", 900, 650)
    hist.SetLineWidth(2)
    hist.Draw("hist")
    canvas.SetGrid()
    canvas.SaveAs(args.output)

    print("Wrote {}".format(os.path.abspath(args.output)))
    print(
        "Total filled {filled} / {seen} triggered events "
        "(missing fit: {missing_fit}, missing E: {missing_energy}, "
        "invalid E: {invalid_energy}, underflow: {underflow}, overflow: {overflow})"
        .format(**total)
    )


if __name__ == "__main__":
    main()
