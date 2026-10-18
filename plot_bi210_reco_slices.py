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


def slice_ranges(low, high, width):
    ranges = []
    edge = low
    while edge < high - 1e-12:
        ranges.append((edge, min(edge + width, high)))
        edge += width
    return ranges


def default_bias_output(output):
    stem, ext = os.path.splitext(output)
    return "{}_bias{}".format(stem, ext or ".png")


def main():
    parser = argparse.ArgumentParser(
        description="Overlay reconstructed-energy distributions in true-energy slices."
    )
    parser.add_argument(
        "inputs",
        nargs="*",
        default=[DEFAULT_INPUT_DIR],
        help="Input ROOT file(s), glob(s), or directories.",
    )
    parser.add_argument("-o", "--output", default="bi210_reco_slices.png")
    parser.add_argument(
        "--bias-output",
        default=None,
        help="Output image for mean reconstructed-energy bias versus true energy.",
    )
    parser.add_argument("--tree", default="output")
    parser.add_argument("--reco-branch", default="energy")
    parser.add_argument("--true-branch", default="mcke1")
    parser.add_argument("--cut", default="fitValid")
    parser.add_argument("--true-min", type=float, default=0.0)
    parser.add_argument("--true-max", type=float, default=0.24)
    parser.add_argument("--true-width", type=float, default=0.04)
    parser.add_argument("--reco-min", type=float, default=0.0)
    parser.add_argument("--reco-max", type=float, default=0.35)
    parser.add_argument("--bins", type=int, default=140)
    parser.add_argument(
        "--normalise",
        choices=("area", "peak", "none"),
        default="area",
        help="How to scale slices before drawing.",
    )
    parser.add_argument("--no-fit", action="store_true", help="Do not draw Gaussian fits.")
    args = parser.parse_args()

    files = expand_inputs(args.inputs)
    if not files:
        raise RuntimeError("No input ROOT files found")

    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetOptFit(0)

    chain = ROOT.TChain(args.tree)
    for path in files:
        chain.Add(path)
    if chain.GetEntries() == 0:
        raise RuntimeError("No entries found in tree '{}'".format(args.tree))

    colors = [
        ROOT.kBlack,
        ROOT.kAzure + 2,
        ROOT.kRed + 1,
        ROOT.kGreen + 2,
        ROOT.kMagenta + 1,
        ROOT.kOrange + 7,
        ROOT.kCyan + 2,
        ROOT.kViolet + 6,
    ]

    hists = []
    fits = []
    summaries = []
    ranges = slice_ranges(args.true_min, args.true_max, args.true_width)
    response = ROOT.TH2D(
        "hRecoSliceResponse", "", len(ranges), args.true_min, args.true_max,
        args.bins, args.reco_min, args.reco_max
    )
    residual_response = ROOT.TH2D(
        "hResidualSliceResponse", "", len(ranges), args.true_min, args.true_max,
        args.bins * 4, -args.reco_max, args.reco_max
    )
    chain.Project(response.GetName(), "{}:{}".format(args.reco_branch, args.true_branch), args.cut)
    chain.Project(
        residual_response.GetName(),
        "{}-{}:{}".format(args.reco_branch, args.true_branch, args.true_branch),
        args.cut,
    )
    for index, (low, high) in enumerate(ranges):
        color = colors[index % len(colors)]
        hist = ROOT.TH1D(
            "hRecoSlice_{}".format(index),
            "Bi210 Reconstructed Energy in True Energy Slices;Reconstructed Energy [MeV];Normalised Events",
            args.bins,
            args.reco_min,
            args.reco_max,
        )
        hist.SetLineColor(color)
        hist.SetLineWidth(2)
        selection = and_cut(
            args.cut,
            "{} >= {:.12g} && {} < {:.12g}".format(
                args.true_branch, low, args.true_branch, high
            ),
        )
        source = response.ProjectionY("hRecoSource_{}".format(index), index + 1, index + 1)
        hist.Add(source)
        hist.SetDirectory(0)
        entries = hist.GetEntries()
        underflow = hist.GetBinContent(0)
        overflow = hist.GetBinContent(hist.GetNbinsX() + 1)
        reco_mean = hist.GetMean()
        reco_rms = hist.GetRMS()
        residual = ROOT.TH1D(
            "hRecoResidualSlice_{}".format(index), "",
            args.bins * 4, -args.reco_max, args.reco_max,
        )
        residual_source = residual_response.ProjectionY(
            "hResidualSource_{}".format(index), index + 1, index + 1
        )
        residual.Add(residual_source)
        residual.SetDirectory(0)
        # Bias and resolution are the mean and RMS of the event-by-event residual,
        # rather than mean(reco) minus the centre of the true-energy slice.
        raw_mean = residual.GetMean()
        raw_rms = residual.GetRMS()

        if args.normalise == "area" and hist.Integral() > 0:
            hist.Scale(1.0 / hist.Integral())
        elif args.normalise == "peak" and hist.GetMaximum() > 0:
            hist.Scale(1.0 / hist.GetMaximum())

        fit_mean = None
        fit_sigma = None
        fit = None
        if not args.no_fit and entries > 50:
            fit = ROOT.TF1(
                "fRecoSlice_{}".format(index),
                "gaus",
                args.reco_min,
                args.reco_max,
            )
            fit.SetLineColor(color)
            fit.SetLineStyle(2)
            fit.SetLineWidth(2)
            fit.SetParameters(hist.GetMaximum(), reco_mean, max(reco_rms, 1e-3))
            fit_result = hist.Fit(fit, "Q0RS")
            if int(fit_result) == 0:
                fit_mean = fit.GetParameter(1)
                fit_sigma = abs(fit.GetParameter(2))
                fits.append(fit)

        summaries.append(
            (low, high, int(entries), int(underflow), int(overflow),
             raw_mean, raw_rms, fit_mean, fit_sigma)
        )
        hists.append(hist)

    canvas = ROOT.TCanvas("cBi210RecoSlices", "cBi210RecoSlices", 950, 750)
    canvas.SetGrid()

    max_y = max(hist.GetMaximum() for hist in hists)
    first = True
    for hist in hists:
        hist.SetMaximum(max_y * 1.18)
        hist.Draw("hist" if first else "hist same")
        first = False
    centre_lines = []
    for hist, summary in zip(hists, summaries):
        low, high = summary[0], summary[1]
        line = ROOT.TLine(0.5 * (low + high), 0.0, 0.5 * (low + high), max_y * 1.18)
        line.SetLineColor(hist.GetLineColor())
        line.SetLineStyle(3)
        line.SetLineWidth(2)
        line.Draw("same")
        centre_lines.append(line)
    for fit in fits:
        fit.Draw("same")

    legend = ROOT.TLegend(0.48, 0.3, 0.83, 0.83)
    legend.SetBorderSize(1)
    legend.SetFillColor(ROOT.kWhite)
    legend.SetFillStyle(1001)
    for hist, summary in zip(hists, summaries):
        low, high, entries, _underflow, _overflow, _mean, rms, _fit_mean, fit_sigma = summary
        if fit_sigma is None:
            label = "{:.2f}-{:.2f} MeV, RMS={:.3f}".format(low, high, rms)
        else:
            label = "{:.2f}-{:.2f} MeV, #sigma_{{fit}}={:.3f}".format(
                low, high, fit_sigma
            )
        legend.AddEntry(hist, label, "l")
    legend.Draw()

    canvas.SaveAs(args.output)

    bias_graph = ROOT.TGraphErrors(len(summaries))
    bias_graph.SetName("gBi210RecoBias")
    bias_graph.SetTitle(
        "Bi210 Reconstructed Energy Bias;True Energy [MeV];"
        "Mean E Bias (Recon - True) [MeV]"
    )
    bias_graph.SetMarkerStyle(20)
    bias_graph.SetMarkerSize(1.0)
    bias_graph.SetMarkerColor(ROOT.kAzure + 2)
    bias_graph.SetLineColor(ROOT.kAzure + 2)
    bias_graph.SetLineWidth(2)
    bias_graph.GetYaxis().SetTitleOffset(1.2)
    for index, summary in enumerate(summaries):
        low, high, _entries, _underflow, _overflow, mean, rms, _fit_mean, _fit_sigma = summary
        centre = 0.5 * (low + high)
        bias_graph.SetPoint(index, centre, mean)
        # Horizontal bars show the true-energy slice half-width; vertical bars are reco RMS.
        bias_graph.SetPointError(index, 0.5 * (high - low), rms)

    bias_canvas = ROOT.TCanvas("cBi210RecoBias", "cBi210RecoBias", 950, 750)
    bias_canvas.SetGrid()
    bias_graph.Draw("AP")
    zero = ROOT.TLine(args.true_min, 0.0, args.true_max, 0.0)
    zero.SetLineColor(ROOT.kBlack)
    zero.SetLineStyle(2)
    zero.SetLineWidth(2)
    zero.Draw("same")
    bias_output = args.bias_output or default_bias_output(args.output)
    bias_canvas.SaveAs(bias_output)

    print("Read {} file(s)".format(len(files)))
    print("Tree entries: {}".format(chain.GetEntries()))
    print("Cut: {}".format(args.cut if args.cut else "<none>"))
    print("Normalisation: {}".format(args.normalise))
    for low, high, entries, underflow, overflow, mean, rms, fit_mean, fit_sigma in summaries:
        fit_text = ""
        if fit_mean is not None:
            fit_text = ", fit mean={:.5f}, fit sigma={:.5f}".format(
                fit_mean, fit_sigma
            )
        print(
            "{:.3f}-{:.3f} true MeV: entries={}, underflow={}, overflow={}, "
            "mean(reco-true)={:+.5f}, RMS(reco-true)={:.5f}{}".format(
                low, high, entries, underflow, overflow, mean, rms,
                fit_text
            )
        )
    print("Wrote {}".format(os.path.abspath(args.output)))
    print("Wrote {}".format(os.path.abspath(bias_output)))


if __name__ == "__main__":
    main()
