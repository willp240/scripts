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


def cpp_array(values):
    return ", ".join("{:.17g}".format(value) for value in values)


def main():
    parser = argparse.ArgumentParser(
        description="Compare Bi210 truth and reconstruction with a slice-based Gaussian smear."
    )
    parser.add_argument("inputs", nargs="*", default=[DEFAULT_INPUT_DIR])
    parser.add_argument("--tree", default="output")
    parser.add_argument("--true-branch", default="mcke1")
    parser.add_argument("--reco-branch", default="energy")
    parser.add_argument("--cut", default="fitValid && evIndex == 0")
    parser.add_argument("--slice-width", type=float, default=0.02)
    parser.add_argument("--response-min", type=float, default=0.0)
    parser.add_argument("--response-max", type=float, default=1.2)
    parser.add_argument("--plot-min", type=float, default=0.0)
    parser.add_argument("--plot-max", type=float, default=0.5)
    parser.add_argument("--plot-bins", type=int, default=100)
    parser.add_argument("--response-reco-bins", type=int, default=480)
    parser.add_argument("--seed", type=int, default=210)
    parser.add_argument("--bias-scale", type=float, default=1.0)
    parser.add_argument("--width-scale", type=float, default=1.0)
    parser.add_argument("--constant-slice", nargs=2, type=float, default=None,
                        metavar=("LOW", "HIGH"),
                        help="Use the bias/RMS from one true-energy slice for every event.")
    parser.add_argument("--replace-bias", nargs=4, type=float, default=None,
                        metavar=("TARGET_LOW", "TARGET_HIGH", "SOURCE_LOW", "SOURCE_HIGH"),
                        help="Replace target-slice bias with source-slice bias.")
    parser.add_argument("-o", "--output", default="bi210_true_reco_gaussian_smear.png")
    args = parser.parse_args()

    files = expand_inputs(args.inputs)
    if not files:
        raise RuntimeError("No input ROOT files found")
    nslices_float = (args.response_max - args.response_min) / args.slice_width
    nslices = int(round(nslices_float))
    if abs(nslices - nslices_float) > 1e-9:
        raise ValueError("Response range must be an integer multiple of slice width")

    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(0)
    chain = ROOT.TChain(args.tree)
    for path in files:
        chain.Add(path)

    # First measure the response mean and RMS in every true-energy slice.
    response = ROOT.TH2D(
        "hSimpleSmearResponse", "",
        nslices, args.response_min, args.response_max,
        args.response_reco_bins, args.response_min, args.response_max,
    )
    selected = chain.Project(
        response.GetName(),
        "{}:{}".format(args.reco_branch, args.true_branch),
        args.cut,
    )
    if selected < 0:
        raise RuntimeError("Could not build response histogram")

    biases = []
    widths = []
    entries = []
    for index in range(nslices):
        xbin = index + 1
        projection = response.ProjectionY("hSimpleSlice_{}".format(index), xbin, xbin)
        centre = response.GetXaxis().GetBinCenter(xbin)
        entries.append(int(projection.GetEntries()))
        biases.append(projection.GetMean() - centre)
        widths.append(projection.GetRMS())

    constant_label = None
    if args.constant_slice is not None:
        low, high = args.constant_slice
        index = int(round(low / args.slice_width))
        if abs(low - index * args.slice_width) > 1e-9 or abs(high - low - args.slice_width) > 1e-9:
            raise ValueError("constant-slice must select one aligned slice")
        constant_bias = biases[index]
        constant_width = widths[index]
        biases = [constant_bias] * nslices
        widths = [constant_width] * nslices
        constant_label = "Constant Smearing"

    replacement_label = None
    if args.replace_bias is not None:
        target_low, target_high, source_low, source_high = args.replace_bias
        target = int(round(target_low / args.slice_width))
        source = int(round(source_low / args.slice_width))
        if (abs(target_low - target * args.slice_width) > 1e-9 or
                abs(source_low - source * args.slice_width) > 1e-9 or
                abs(target_high - target_low - args.slice_width) > 1e-9 or
                abs(source_high - source_low - args.slice_width) > 1e-9):
            raise ValueError("replace-bias ranges must each select one aligned slice")
        biases[target] = biases[source]
        replacement_label = "Bias {:.2f}-{:.2f} -> {:.2f}-{:.2f} MeV".format(
            target_low, target_high, source_low, source_high
        )

    # TChain::Project performs the requested event loop in compiled ROOT code. The
    # Gaussian describes the residual: reco - true ~ N(slice bias, slice RMS).
    ROOT.gInterpreter.Declare(
        """
        #include <TRandom3.h>
        #include <cmath>
        namespace Bi210SimpleSmear {
          static const int n = %(n)d;
          static const double emin = %(emin).17g;
          static const double width = %(width).17g;
          static const double bias[n] = { %(bias)s };
          static const double sigma[n] = { %(sigma)s };
          static TRandom3 random(%(seed)d);
        }
        double SmearBi210TrueEnergy(double trueEnergy) {
          int i = static_cast<int>(std::floor(
              (trueEnergy - Bi210SimpleSmear::emin) / Bi210SimpleSmear::width));
          if (i < 0) i = 0;
          if (i >= Bi210SimpleSmear::n) i = Bi210SimpleSmear::n - 1;
          return trueEnergy + Bi210SimpleSmear::random.Gaus(
              %(bias_scale).17g * Bi210SimpleSmear::bias[i],
              %(width_scale).17g * Bi210SimpleSmear::sigma[i]);
        }
        """ % {
            "n": nslices,
            "emin": args.response_min,
            "width": args.slice_width,
            "bias": cpp_array(biases),
            "sigma": cpp_array(widths),
            "seed": args.seed,
            "bias_scale": args.bias_scale,
            "width_scale": args.width_scale,
        }
    )

    title = "Bi210 energy;Energy [MeV];Events"
    truth = ROOT.TH1D("hSimpleTruth", title, args.plot_bins, args.plot_min, args.plot_max)
    reco = ROOT.TH1D("hSimpleReco", title, args.plot_bins, args.plot_min, args.plot_max)
    smeared = ROOT.TH1D("hSimpleSmeared", title, args.plot_bins, args.plot_min, args.plot_max)
    ntruth = chain.Project(truth.GetName(), args.true_branch, args.cut)
    nreco = chain.Project(reco.GetName(), args.reco_branch, args.cut)
    nsmear = chain.Project(
        smeared.GetName(), "SmearBi210TrueEnergy({})".format(args.true_branch), args.cut
    )
    for hist in (truth, reco, smeared):
        hist.SetDirectory(0)

    truth.SetLineColor(ROOT.kOrange + 7)
    reco.SetLineColor(ROOT.kAzure + 2)
    smeared.SetLineColor(ROOT.kMagenta + 1)
    for hist in (truth, reco, smeared):
        hist.SetLineWidth(2)
    truth.SetMaximum(1.15 * max(truth.GetMaximum(), reco.GetMaximum(), smeared.GetMaximum()))

    canvas = ROOT.TCanvas("cSimpleSmear", "cSimpleSmear", 950, 700)
    canvas.SetGrid()
    truth.Draw("hist")
    reco.Draw("hist same")
    smeared.Draw("hist same")
    legend = ROOT.TLegend(0.30, 0.3, 0.55, 0.47)
    legend.AddEntry(truth, "True Energy", "l")
    legend.AddEntry(reco, "Reconstructed Energy", "l")
    if replacement_label is not None:
        smear_label = replacement_label
    elif constant_label is None:
        smear_label = "True + {:.2g}#times bias, {:.2g}#times resolution".format(
            args.bias_scale, args.width_scale
        )
    else:
        smear_label = constant_label
    legend.AddEntry(smeared, smear_label, "l")
    legend.Draw()
    canvas.SaveAs(args.output)

    print("Read {} files; fit/selection entries: {}".format(len(files), int(selected)))
    print("Slice width: {:.3f} MeV".format(args.slice_width))
    for index, (number, bias, width) in enumerate(zip(entries, biases, widths)):
        low = args.response_min + index * args.slice_width
        high = low + args.slice_width
        if low < args.plot_max:
            print("  {:.3f}-{:.3f}: entries={}, bias={:+.6f}, RMS={:.6f}".format(
                low, high, number, bias, width
            ))
    print("Projected true/reco/smeared entries: {} / {} / {}".format(
        int(ntruth), int(nreco), int(nsmear)
    ))
    print("Wrote {}".format(os.path.abspath(args.output)))


if __name__ == "__main__":
    main()
