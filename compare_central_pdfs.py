#!/usr/bin/env python3
"""Compare central MultiPDF curves from run outputs and/or a RATDB table."""

import argparse
import json
import re
import sys
from array import array
from pathlib import Path

try:
    import ROOT
except ImportError:
    sys.exit("error: PyROOT is unavailable; source the RAT environment first")

from make_fit_performance_report import DEFAULT_COLOURS, DEFAULT_STYLES, final_multipdf_path, root_colour


ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetGridStyle(3)
ROOT.gStyle.SetPaperSize(26.0, 14.625)


def parse_args():
    parser = argparse.ArgumentParser(
        description="Compare central PDFs, including curves read directly from RATDB."
    )
    parser.add_argument("config", type=Path, help="JSON central-PDF comparison configuration")
    parser.add_argument("--output-directory", "-o", type=Path,
                        help="Override output_directory from the configuration")
    return parser.parse_args()


def number_array(table, field):
    match = re.search(r"\b{}\s*:\s*\[(.*?)\]".format(re.escape(field)),
                      table, re.DOTALL)
    if not match:
        sys.exit("error: {} is missing from the selected RATDB table".format(field))
    return [float(value) for value in re.findall(
        r"[-+]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][-+]?\d+)?", match.group(1))]


def ratdb_curve(case, number):
    path = Path(case["ratdb_file"]).expanduser().resolve()
    text = path.read_text()
    index_pattern = r'\bindex\s*:\s*"{}"\s*,'.format(re.escape(case["ratdb_index"]))
    index_match = re.search(index_pattern, text)
    if not index_match:
        sys.exit("error: RATDB index {!r} is absent from {}".format(
            case["ratdb_index"], path))
    start = text.rfind("{", 0, index_match.start())
    end = text.find("}", index_match.end())
    if start < 0 or end < 0:
        sys.exit("error: could not isolate RATDB table for {}".format(case["ratdb_index"]))
    table = text[start:end + 1]
    times = number_array(table, "time")
    probabilities = number_array(table, "prob_posdep_0")
    if len(times) != len(probabilities) or not times:
        sys.exit("error: incompatible time and prob_posdep_0 arrays in {}".format(path))
    total = sum(probabilities)
    if total <= 0:
        sys.exit("error: prob_posdep_0 has zero integral in {}".format(path))
    graph = ROOT.TGraph(len(times), array("d", times),
                       array("d", [value / total for value in probabilities]))
    graph.SetName("central_pdf_ratdb_{}".format(number))
    return graph


def run_curve(case, number):
    path = final_multipdf_path(case)
    if path is None:
        sys.exit("error: no MultiPDF_Plots_Round*.root found for {}".format(case["label"]))
    root_file = ROOT.TFile.Open(str(path), "READ")
    source = root_file.Get("PDF_0") if root_file and not root_file.IsZombie() else None
    if not source:
        sys.exit("error: PDF_0 is missing or unreadable in {}".format(path))
    total = source.Integral()
    if total <= 0:
        sys.exit("error: PDF_0 has zero integral in {}".format(path))
    points_x = []
    points_y = []
    for bin_number in range(1, source.GetNbinsX() + 1):
        points_x.append(source.GetBinCenter(bin_number))
        points_y.append(source.GetBinContent(bin_number) / total)
    root_file.Close()
    graph = ROOT.TGraph(len(points_x), array("d", points_x), array("d", points_y))
    graph.SetName("central_pdf_run_{}".format(number))
    return graph


def main():
    args = parse_args()
    config = json.loads(args.config.expanduser().read_text())
    cases = config.get("cases", [])
    if len(cases) < 2:
        sys.exit("error: configuration must contain at least two cases")

    curves = []
    for number, case in enumerate(cases):
        if not case.get("label"):
            sys.exit("error: case {} requires a label".format(number + 1))
        if "ratdb_file" in case:
            if not case.get("ratdb_index"):
                sys.exit("error: RATDB case {} requires ratdb_index".format(number + 1))
            curve = ratdb_curve(case, number)
        elif "directory" in case:
            case["directory"] = str(Path(case["directory"]).expanduser().resolve())
            curve = run_curve(case, number)
        else:
            sys.exit("error: case {} requires directory or ratdb_file".format(number + 1))
        colour = root_colour(case.get("colour"), DEFAULT_COLOURS[number % len(DEFAULT_COLOURS)])
        curve.SetLineColor(colour)
        curve.SetLineStyle(int(case.get("line_style", DEFAULT_STYLES[number % len(DEFAULT_STYLES)])))
        curve.SetLineWidth(3)
        curves.append((case, curve))

    output_dir = args.output_directory or config.get("output_directory", "central_pdf_comparison")
    output_dir = Path(output_dir).expanduser().resolve()
    output_dir.mkdir(parents=True, exist_ok=True)

    canvas = ROOT.TCanvas("central_multipdf_comparison", "central_multipdf_comparison", 1600, 900)
    canvas.SetGrid()
    canvas.SetLeftMargin(0.11)
    canvas.SetBottomMargin(0.12)
    frame = canvas.DrawFrame(-5.0, 0.0, 100.0,
                             1.12 * max(curve.GetY()[point]
                                        for _, curve in curves
                                        for point in range(curve.GetN())))
    frame.SetTitle(config.get("title", "Central time-residual PDFs"))
    frame.GetXaxis().SetTitle("Hit time residual [ns]")
    frame.GetYaxis().SetTitle("Normalised counts")
    frame.GetYaxis().SetTitleOffset(1.25)
    for _, curve in curves:
        curve.Draw("L SAME")

    legend = ROOT.TLegend(0.58, 0.64, 0.88, 0.86)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    for case, curve in curves:
        legend.AddEntry(curve, case["label"], "l")
    legend.Draw()
    canvas._keepalive = [frame, legend] + [curve for _, curve in curves]

    for extension in ("pdf", "png"):
        canvas.SaveAs(str(output_dir / ("central_multipdf_comparison." + extension)))
    root_output = ROOT.TFile.Open(str(output_dir / "central_multipdf_comparison.root"), "RECREATE")
    canvas.Write()
    root_output.Close()
    print("Compared {} central PDFs".format(len(curves)))
    print("Wrote outputs to {}".format(output_dir))


if __name__ == "__main__":
    main()
