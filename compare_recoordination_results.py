#!/usr/bin/env python3
"""Compare final effective velocities and MultiPDFs from recoordination runs."""

import argparse
import re
import sys
from pathlib import Path

try:
    import ROOT
except ImportError:
    sys.exit("error: PyROOT is unavailable; source the RAT environment first")

from make_fit_performance_report import DEFAULT_COLOURS, root_colour


ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetGridStyle(3)
ROOT.gStyle.SetPaperSize(26.0, 14.625)


def parse_args():
    parser = argparse.ArgumentParser(
        description="Compare final SEVs and radial-shell MultiPDFs from 2--N runs."
    )
    parser.add_argument("runs", nargs="+", type=Path,
                        help="AutoRecoordinating output directories")
    parser.add_argument("--labels", nargs="+",
                        help="Display labels, in the same order as runs")
    parser.add_argument("--colours", nargs="+",
                        help="ROOT colours, in the same order as runs")
    parser.add_argument("--output-directory", "-o", type=Path,
                        default=Path("recoordination_comparison"))
    return parser.parse_args()


def final_multipdf(run):
    candidates = []
    for path in (run / "plots").glob("MultiPDF_Plots_Round*.root"):
        match = re.fullmatch(r"MultiPDF_Plots_Round(\d+)\.root", path.name)
        if match:
            candidates.append((int(match.group(1)), path))
    if not candidates:
        sys.exit("error: no MultiPDF_Plots_Round*.root found in {}".format(run / "plots"))
    return max(candidates)


def clone_object(path, name, clone_name):
    root_file = ROOT.TFile.Open(str(path), "READ")
    source = root_file.Get(name) if root_file and not root_file.IsZombie() else None
    if not source:
        sys.exit("error: {} is missing or unreadable in {}".format(name, path))
    clone = source.Clone(clone_name)
    if hasattr(clone, "SetDirectory"):
        clone.SetDirectory(0)
    root_file.Close()
    return clone


def velocity_canvas(cases):
    canvas = ROOT.TCanvas("final_effective_velocities", "final_effective_velocities", 1600, 900)
    canvas.SetGrid()
    canvas.SetLeftMargin(0.11)
    canvas.SetBottomMargin(0.12)
    graphs = []
    for number, case in enumerate(cases):
        path = case["run"] / "plots/scint_effective_speeds.root"
        for energy, object_name, line_style in (
                ("2.5 MeV", "low_e_velocities", 1),
                ("10 MeV", "high_e_velocities", 2)):
            graph = clone_object(path, object_name,
                                 "{}_case_{}".format(object_name, number))
            graph.SetLineColor(case["colour"])
            graph.SetMarkerColor(case["colour"])
            graph.SetLineStyle(line_style)
            graph.SetLineWidth(3)
            graph.SetMarkerStyle(20 + number)
            graph.SetMarkerSize(1.2)
            graphs.append((case, energy, graph))

    xs = [graph.GetPointX(i) for _, _, graph in graphs for i in range(graph.GetN())]
    ys = [graph.GetPointY(i) for _, _, graph in graphs for i in range(graph.GetN())]
    xpad = 0.04 * (max(xs) - min(xs))
    ypad = max(0.05, 0.15 * (max(ys) - min(ys)))
    frame = canvas.DrawFrame(min(xs) - xpad, min(ys) - ypad,
                             max(xs) + xpad, max(ys) + ypad)
    frame.SetTitle("Final scintillator effective velocities")
    frame.GetXaxis().SetTitle("Radius [mm]")
    frame.GetYaxis().SetTitle("Effective velocity [mm/ns]")
    frame.GetYaxis().SetTitleOffset(1.25)
    for _, _, graph in graphs:
        graph.Draw("LP SAME")
    legend = ROOT.TLegend(0.58, 0.64, 0.89, 0.88)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    for case, energy, graph in graphs:
        legend.AddEntry(graph, "{} -- {}".format(case["label"], energy), "lp")
    legend.Draw()
    canvas._keepalive = [frame, legend] + [graph for _, _, graph in graphs]
    return canvas, graphs


def multipdf_canvas(cases, shell):
    canvas = ROOT.TCanvas("final_multipdf_shell_{}".format(shell),
                          "final_multipdf_shell_{}".format(shell), 1600, 900)
    canvas.SetGrid()
    canvas.SetLeftMargin(0.11)
    canvas.SetBottomMargin(0.12)
    histograms = []
    for number, case in enumerate(cases):
        histogram = clone_object(case["multipdf_path"], "PDF_{}".format(shell),
                                 "final_pdf_shell_{}_case_{}".format(shell, number))
        integral = histogram.Integral()
        if integral <= 0:
            sys.exit("error: PDF_{} has zero integral in {}".format(
                shell, case["multipdf_path"]))
        histogram.Scale(1.0 / integral)
        histogram.SetLineColor(case["colour"])
        histogram.SetLineWidth(3)
        histogram.SetLineStyle(number + 1)
        histograms.append((case, histogram))

    ymax = 1.12 * max(histogram.GetMaximum() for _, histogram in histograms)
    for number, (_, histogram) in enumerate(histograms):
        histogram.SetTitle("Final time-residual PDFs -- radial shell {}".format(shell))
        histogram.GetXaxis().SetTitle("Hit time residual [ns]")
        histogram.GetYaxis().SetTitle("Normalised counts")
        histogram.GetYaxis().SetTitleOffset(1.25)
        histogram.GetXaxis().SetRangeUser(-5.0, 100.0)
        histogram.GetYaxis().SetRangeUser(0.0, ymax)
        histogram.Draw("HIST" if number == 0 else "HIST SAME")
    legend = ROOT.TLegend(0.58, 0.70, 0.89, 0.88)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    for case, histogram in histograms:
        legend.AddEntry(histogram, "{} (round {})".format(
            case["label"], case["multipdf_round"]), "l")
    legend.Draw()
    canvas._keepalive = [legend] + [histogram for _, histogram in histograms]
    return canvas, histograms


def save_canvas(canvas, stem, output_dir):
    canvas.SaveAs(str(output_dir / (stem + ".pdf")))
    canvas.SaveAs(str(output_dir / (stem + ".png")))


def main():
    args = parse_args()
    if len(args.runs) < 2:
        sys.exit("error: provide at least two run directories")
    if args.labels and len(args.labels) != len(args.runs):
        sys.exit("error: --labels must contain one label per run")
    if args.colours and len(args.colours) != len(args.runs):
        sys.exit("error: --colours must contain one colour per run")

    cases = []
    for number, supplied_run in enumerate(args.runs):
        run = supplied_run.expanduser().resolve()
        if not run.is_dir():
            sys.exit("error: run directory does not exist: {}".format(run))
        round_number, multipdf_path = final_multipdf(run)
        colour_name = args.colours[number] if args.colours else None
        cases.append({
            "run": run,
            "label": args.labels[number] if args.labels else run.name,
            "colour": root_colour(colour_name,
                                  DEFAULT_COLOURS[number % len(DEFAULT_COLOURS)]),
            "multipdf_round": round_number,
            "multipdf_path": multipdf_path,
        })

    output_dir = args.output_directory.expanduser().resolve()
    output_dir.mkdir(parents=True, exist_ok=True)
    root_output = ROOT.TFile.Open(str(output_dir / "recoordination_comparison.root"), "RECREATE")
    canvases = []
    velocity, velocity_graphs = velocity_canvas(cases)
    canvases.append(("final_effective_velocities", velocity))
    root_output.cd()
    velocity.Write()
    for _, _, graph in velocity_graphs:
        root_output.cd()
        graph.Write()
    for shell in range(5):
        canvas, histograms = multipdf_canvas(cases, shell)
        canvases.append(("final_multipdf_shell_{}".format(shell), canvas))
        root_output.cd()
        canvas.Write()
        for _, histogram in histograms:
            root_output.cd()
            histogram.Write()
    root_output.Close()

    combined = output_dir / "recoordination_comparison.pdf"
    for stem, canvas in canvases:
        save_canvas(canvas, stem, output_dir)
    for number, (stem, canvas) in enumerate(canvases):
        suffix = "(" if number == 0 else ")" if number == len(canvases) - 1 else ""
        canvas.Print(str(combined) + suffix, "Title:{}".format(stem))

    print("Compared {} runs".format(len(cases)))
    for case in cases:
        print("  {}: final MultiPDF round {}".format(
            case["label"], case["multipdf_round"]))
    print("Wrote 1 velocity and 5 radial-shell PDF comparisons to {}".format(output_dir))


if __name__ == "__main__":
    main()
