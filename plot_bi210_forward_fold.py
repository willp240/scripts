#!/usr/bin/env python

from __future__ import absolute_import, division, print_function

import argparse
import glob
import math
import os

import ROOT


DEFAULT_INPUT_DIR = "/data/snoplus3/SNOplusData/production/rat-8.0.1/ntuple/Bi210"


def expand_inputs(inputs):
    files = []
    for path in inputs:
        matches = glob.glob(os.path.join(path, "*.root")) if os.path.isdir(path) else glob.glob(path)
        files.extend(matches if matches else [path])
    return sorted(set(path for path in files if os.path.isfile(path)))


def gaussian_bin_probability(low, high, mean, sigma):
    if sigma <= 0.0:
        return 1.0 if low <= mean < high else 0.0
    scale = math.sqrt(2.0) * sigma
    return 0.5 * (math.erf((high - mean) / scale) - math.erf((low - mean) / scale))


def add_at_bin_centres(hist, value, weight):
    """Add weight at value, linearly shared between neighbouring bin centres."""
    axis = hist.GetXaxis()
    if value < axis.GetXmin() or value >= axis.GetXmax():
        return
    target = axis.FindFixBin(value)
    centre = axis.GetBinCenter(target)
    if value >= centre and target < hist.GetNbinsX():
        neighbour = target + 1
    elif value < centre and target > 1:
        neighbour = target - 1
    else:
        hist.AddBinContent(target, weight)
        return
    neighbour_centre = axis.GetBinCenter(neighbour)
    neighbour_fraction = abs(value - centre) / abs(neighbour_centre - centre)
    hist.AddBinContent(target, weight * (1.0 - neighbour_fraction))
    hist.AddBinContent(neighbour, weight * neighbour_fraction)


def main():
    parser = argparse.ArgumentParser(
        description="Forward-fold Bi210 truth through empirical and zero-bias resolution models."
    )
    parser.add_argument("inputs", nargs="*", default=[DEFAULT_INPUT_DIR])
    parser.add_argument("--tree", default="output")
    parser.add_argument("--true-branch", default="mcke1")
    parser.add_argument("--reco-branch", default="energy")
    parser.add_argument("--cut", default="fitValid")
    parser.add_argument("--emin", type=float, default=0.0)
    parser.add_argument("--emax", type=float, default=1.2)
    parser.add_argument("--bins", type=int, default=240, help="True and reconstructed bins.")
    parser.add_argument("--display-max", type=float, default=0.5)
    parser.add_argument("--target-window", nargs=2, type=float, default=(0.08, 0.12))
    parser.add_argument("--contribution-width", type=float, default=0.02)
    parser.add_argument("-o", "--output", default="bi210_forward_fold.png")
    parser.add_argument("--ratio-output", default="bi210_forward_fold_ratio.png")
    args = parser.parse_args()

    files = expand_inputs(args.inputs)
    if not files:
        raise RuntimeError("No input ROOT files found")

    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(0)
    chain = ROOT.TChain(args.tree)
    for path in files:
        chain.Add(path)

    response = ROOT.TH2D(
        "hResponse", "Bi210 response;True Energy [MeV];Reconstructed Energy [MeV]",
        args.bins, args.emin, args.emax, args.bins, args.emin, args.emax,
    )
    expression = "{}:{}".format(args.reco_branch, args.true_branch)
    selected = chain.Project(response.GetName(), expression, args.cut)
    if selected < 0:
        raise RuntimeError("Projection failed")
    response.SetDirectory(0)

    # These projections are the measured truth and reconstruction spectra. Folding each
    # truth bin through its empirical conditional response is algebraically ProjectionY.
    truth = response.ProjectionX("hTruth", 0, response.GetNbinsY() + 1)
    actual = response.ProjectionY("hActual", 0, response.GetNbinsX() + 1)
    empirical = actual.Clone("hEmpiricalFold")
    gaussian = actual.Clone("hZeroBiasGaussianFold")
    biased_gaussian = actual.Clone("hMeasuredBiasGaussianFold")
    shifted_empirical = actual.Clone("hZeroBiasEmpiricalFold")
    gaussian.Reset("ICES")
    biased_gaussian.Reset("ICES")
    shifted_empirical.Reset("ICES")
    for hist in (truth, actual, empirical, gaussian, biased_gaussian, shifted_empirical):
        hist.SetDirectory(0)

    sigmas = []
    for ix in range(1, response.GetNbinsX() + 1):
        ntrue = truth.GetBinContent(ix)
        conditional = response.ProjectionY("hConditional_{}".format(ix), ix, ix)
        measured_mean = conditional.GetMean()
        sigma = conditional.GetRMS()
        sigmas.append(sigma)
        if ntrue <= 0.0 or conditional.GetEntries() < 2:
            continue
        # Null hypothesis: response is Gaussian, centred on true E, with measured RMS.
        mean = response.GetXaxis().GetBinCenter(ix)
        for iy in range(1, gaussian.GetNbinsX() + 1):
            low = gaussian.GetXaxis().GetBinLowEdge(iy)
            high = gaussian.GetXaxis().GetBinUpEdge(iy)
            gaussian.AddBinContent(iy, ntrue * gaussian_bin_probability(low, high, mean, sigma))
            biased_gaussian.AddBinContent(
                iy, ntrue * gaussian_bin_probability(low, high, measured_mean, sigma)
            )
        # Preserve the full conditional shape but translate it to have zero mean bias.
        shift = mean - measured_mean
        for iy in range(1, conditional.GetNbinsX() + 1):
            weight = conditional.GetBinContent(iy)
            if weight:
                add_at_bin_centres(
                    shifted_empirical,
                    conditional.GetXaxis().GetBinCenter(iy) + shift,
                    weight,
                )
    # AddBinContent does not describe statistical uncertainties for this deterministic model.
    for model in (gaussian, biased_gaussian, shifted_empirical):
        for iy in range(0, model.GetNbinsX() + 2):
            model.SetBinError(iy, 0.0)

    actual.SetTitle("Bi210 forward fold;Energy [MeV];Events")
    actual.SetLineColor(ROOT.kBlack)
    actual.SetLineWidth(3)
    empirical.SetLineColor(ROOT.kGreen + 2)
    empirical.SetLineStyle(2)
    empirical.SetLineWidth(2)
    gaussian.SetLineColor(ROOT.kRed + 1)
    gaussian.SetLineWidth(2)
    biased_gaussian.SetLineColor(ROOT.kMagenta + 1)
    biased_gaussian.SetLineWidth(2)
    shifted_empirical.SetLineColor(ROOT.kOrange + 7)
    shifted_empirical.SetLineWidth(2)
    shifted_empirical.SetLineStyle(7)
    actual.GetXaxis().SetRangeUser(args.emin, args.display_max)
    canvas = ROOT.TCanvas("cForwardFold", "cForwardFold", 950, 750)
    canvas.SetGrid()
    actual.Draw("hist")
    empirical.Draw("hist same")
    gaussian.Draw("hist same")
    biased_gaussian.Draw("hist same")
    shifted_empirical.Draw("hist same")
    legend = ROOT.TLegend(0.43, 0.59, 0.89, 0.87)
    legend.AddEntry(actual, "Actual reconstructed", "l")
    legend.AddEntry(empirical, "Empirical response closure", "l")
    legend.AddEntry(gaussian, "Zero-bias Gaussian + measured RMS", "l")
    legend.AddEntry(biased_gaussian, "Measured-bias Gaussian + measured RMS", "l")
    legend.AddEntry(shifted_empirical, "Zero-bias shifted empirical response", "l")
    legend.Draw()
    canvas.SaveAs(args.output)

    ratio = actual.Clone("hActualOverGaussian")
    ratio.Divide(gaussian)
    biased_ratio = actual.Clone("hActualOverBiasedGaussian")
    biased_ratio.Divide(biased_gaussian)
    shifted_ratio = actual.Clone("hActualOverShiftedEmpirical")
    shifted_ratio.Divide(shifted_empirical)
    ratio.SetTitle("Bi210 response-model comparison;Reconstructed Energy [MeV];Actual / model")
    ratio.GetXaxis().SetRangeUser(args.emin, args.display_max)
    ratio.SetMinimum(0.8)
    ratio.SetMaximum(1.2)
    ratio.SetLineColor(ROOT.kAzure + 2)
    ratio.SetMarkerColor(ROOT.kAzure + 2)
    ratio.SetMarkerStyle(20)
    biased_ratio.SetLineColor(ROOT.kMagenta + 1)
    biased_ratio.SetMarkerColor(ROOT.kMagenta + 1)
    biased_ratio.SetMarkerStyle(21)
    shifted_ratio.SetLineColor(ROOT.kOrange + 7)
    shifted_ratio.SetMarkerColor(ROOT.kOrange + 7)
    shifted_ratio.SetMarkerStyle(22)
    ratio_canvas = ROOT.TCanvas("cForwardFoldRatio", "cForwardFoldRatio", 950, 650)
    ratio_canvas.SetGrid()
    ratio.Draw("E1")
    biased_ratio.Draw("E1 same")
    shifted_ratio.Draw("E1 same")
    unity = ROOT.TLine(args.emin, 1.0, args.display_max, 1.0)
    unity.SetLineStyle(2)
    unity.Draw("same")
    ratio_legend = ROOT.TLegend(0.48, 0.70, 0.88, 0.87)
    ratio_legend.AddEntry(ratio, "Zero-bias Gaussian", "lp")
    ratio_legend.AddEntry(biased_ratio, "Measured-bias Gaussian", "lp")
    ratio_legend.AddEntry(shifted_ratio, "Zero-bias empirical", "lp")
    ratio_legend.Draw()
    ratio_canvas.SaveAs(args.ratio_output)

    target_low, target_high = args.target_window
    epsilon = 1e-10
    ylow = response.GetYaxis().FindFixBin(target_low + epsilon)
    yhigh = response.GetYaxis().FindFixBin(target_high - epsilon)
    print("Read {} files; selected {} events".format(len(files), int(selected)))
    print("Contributions to {:.3f} <= reco E < {:.3f} MeV:".format(target_low, target_high))
    edge = args.emin
    target_total = actual.Integral(ylow, yhigh)
    while edge < args.emax - 1e-12:
        high = min(edge + args.contribution_width, args.emax)
        xlow = response.GetXaxis().FindFixBin(edge + epsilon)
        xhigh = response.GetXaxis().FindFixBin(high - epsilon)
        contribution = response.Integral(xlow, xhigh, ylow, yhigh)
        if contribution > 0:
            print("  true {:.3f}-{:.3f}: {:9.0f} ({:6.2%})".format(
                edge, high, contribution, contribution / target_total))
        edge = high
    print("Measured RMS by true-energy bin is used without a Gaussian fit.")
    print("Wrote {}".format(os.path.abspath(args.output)))
    print("Wrote {}".format(os.path.abspath(args.ratio_output)))


if __name__ == "__main__":
    main()
