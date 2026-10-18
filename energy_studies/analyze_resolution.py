#!/usr/bin/env python3
"""Plot reconstructed-energy and cleaned-NHit resolutions from RAT ntuples.

Usage:
  python3 analyze_resolution.py INPUT_DIRECTORY OUTPUT_PREFIX

The script reads all *.root files below INPUT_DIRECTORY.  It expects the
standard RAT outntuple tree and branches: output/energy, output/nhitsCleaned,
and output/fitValid.
"""

import argparse
import glob
import math
import os
import sys

import ROOT


def values_from_directory(directory):
    files = sorted(glob.glob(os.path.join(directory, "**", "*.root"), recursive=True))
    if not files:
        raise RuntimeError("No ROOT files found below {}".format(directory))

    chain = ROOT.TChain("output")
    for filename in files:
        if chain.Add(filename) == 0:
            print("Warning: could not add {}".format(filename), file=sys.stderr)

    energies = []
    nhits = []
    for event in chain:
        # fitValid is the validity flag selected by OutNtupleProc.  Require
        # finite, positive values as an additional guard against sentinels.
        if hasattr(event, "fitValid") and not bool(event.fitValid):
            continue
        energy = float(event.energy)
        hit_count = float(event.nhitsCleaned)
        if math.isfinite(energy) and math.isfinite(hit_count) and energy > 0 and hit_count > 0:
            energies.append(energy)
            nhits.append(hit_count)

    if not energies:
        raise RuntimeError("No valid events found in the input files")
    return energies, nhits, files


def histogram_and_fit(data, name, title, xlabel, bins=80):
    mean = sum(data) / len(data)
    variance = sum((x - mean) ** 2 for x in data) / max(1, len(data) - 1)
    rms = math.sqrt(variance)
    low = max(0.0, mean - 5.0 * rms)
    high = mean + 5.0 * rms
    if high <= low:
        high = low + 1.0

    hist = ROOT.TH1D(name, title, bins, low, high)
    for value in data:
        hist.Fill(value)

    # Fit the central region to reduce sensitivity to pathological tails.
    fit_low = max(low, mean - 2.5 * rms)
    fit_high = min(high, mean + 2.5 * rms)
    fit = ROOT.TF1(name + "_gaus", "gaus", fit_low, fit_high)
    hist.Fit(fit, "Q0", "", fit_low, fit_high)
    fitted_mean = fit.GetParameter(1)
    fitted_sigma = abs(fit.GetParameter(2))

    hist.SetStats(False)
    hist.SetLineColor(ROOT.kBlue + 1)
    hist.SetFillColorAlpha(ROOT.kAzure - 9, 0.35)
    hist.GetXaxis().SetTitle(xlabel)
    hist.GetYaxis().SetTitle("Events")
    hist.SetTitle(title)
    hist.Draw("HIST")
    fit.SetLineColor(ROOT.kRed + 1)
    fit.SetLineWidth(2)
    fit.Draw("SAME")
    return hist, fit, fitted_mean, fitted_sigma


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input_directory")
    parser.add_argument("output_prefix", help="prefix for the two output PNG files")
    args = parser.parse_args()

    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetOptFit(0)

    energies, nhits, files = values_from_directory(args.input_directory)
    os.makedirs(os.path.dirname(os.path.abspath(args.output_prefix)), exist_ok=True)

    energy_canvas = ROOT.TCanvas("energy_canvas", "Energy resolution", 900, 700)
    _, energy_fit, energy_mean, energy_width = histogram_and_fit(
        energies, "energy", "Reconstructed energy", "Reconstructed energy [MeV]"
    )
    energy_fraction = energy_width / energy_mean
    energy_text = ROOT.TLatex()
    energy_text.SetNDC(True)
    energy_text.SetTextSize(0.035)
    energy_text.DrawLatex(0.60, 0.82, "#mu = {:.5g} MeV".format(energy_mean))
    energy_text.DrawLatex(0.60, 0.77, "#sigma = {:.5g} MeV".format(energy_width))
    energy_text.DrawLatex(0.60, 0.72, "#sigma_{{E}}/#mu_{{E}} = {:.5g}".format(energy_fraction))
    energy_canvas.SaveAs(args.output_prefix + "_energy.png")

    nhit_canvas = ROOT.TCanvas("nhit_canvas", "NHit resolution", 900, 700)
    _, nhit_fit, nhit_mean, nhit_width = histogram_and_fit(
        nhits, "nhits", "Cleaned PMT hit-count resolution", "Cleaned PMT hits"
    )
    nhit_fraction = nhit_width / nhit_mean
    poisson_fraction = 1.0 / math.sqrt(nhit_mean)
    nhit_text = ROOT.TLatex()
    nhit_text.SetNDC(True)
    nhit_text.SetTextSize(0.035)
    nhit_text.DrawLatex(0.57, 0.84, "#mu_{{N}} = {:.5g}".format(nhit_mean))
    nhit_text.DrawLatex(0.57, 0.79, "#sigma_{{N}} = {:.5g}".format(nhit_width))
    nhit_text.DrawLatex(0.57, 0.74, "#sigma_{{N}}/#mu_{{N}} = {:.5g}".format(nhit_fraction))
    nhit_text.DrawLatex(0.57, 0.69, "1/#sqrt{{#mu_N}} = {:.5g}".format(poisson_fraction))
    nhit_canvas.SaveAs(args.output_prefix + "_nhits.png")

    print("Read {} files and {} valid events".format(len(files), len(energies)))
    print("Energy: mean = {:.6g}, width = {:.6g}, width/mean = {:.6g}".format(
        energy_mean, energy_width, energy_fraction))
    print("NHit:   mean = {:.6g}, width = {:.6g}, width/mean = {:.6g}, Poisson = {:.6g}".format(
        nhit_mean, nhit_width, nhit_fraction, poisson_fraction))
    print("Wrote {} and {}".format(args.output_prefix + "_energy.png",
                                  args.output_prefix + "_nhits.png"))


if __name__ == "__main__":
    main()
