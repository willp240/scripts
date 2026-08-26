#!/usr/bin/env python3
"""Benchmark and compare the old and ntuple-aware time-residual methods.

The reference and candidate calculations intentionally run as separate
commands so RAT database/utility state cannot leak between measurements.
"""

import argparse
from array import array
import json
import math
import os
import time

import ROOT
import rat
from ROOT import RAT


def _input_tree(path):
    root_file = ROOT.TFile.Open(path, "READ")
    if not root_file or root_file.IsZombie():
        raise RuntimeError(f"could not open {path}")
    tree = root_file.Get("output")
    if not tree:
        raise RuntimeError(f"{path} does not contain an 'output' tree")
    if tree.GetEntries() == 0:
        raise RuntimeError(f"{path} has an empty 'output' tree")
    return root_file, tree


def _branch_names(tree):
    return {branch.GetName() for branch in tree.GetListOfBranches()}


def _event_av_offset(event, names):
    if "av_offset" in names:
        return float(event.av_offset)
    if "avOffset" in names:
        return float(event.avOffset)
    if "posz" in names and "posz_av" in names:
        return float(event.posz - event.posz_av)
    raise RuntimeError(
        "ntuple needs av_offset (or avOffset), or both posz and posz_av"
    )


def _initialise_rat(run_number, airplane):
    db = RAT.DB.Get()
    db.SetAirplaneModeStatus(airplane)

    start = time.perf_counter()
    db.BeginOfRun(run_number)
    begin_db_seconds = time.perf_counter() - start

    start = time.perf_counter()
    db.LoadDefaults()
    # These are the standard local SNO+ geometry/PMT tables used by the
    # PyROOT ntuple examples. The remote DB provides run conditions, not the
    # complete base PMT geometry required by Utility::BeginOfRun().
    ratroot = os.environ["RATROOT"]
    db.Load(os.path.join(ratroot, "data/geo/snoplusnative.geo"))
    db.Load(os.path.join(ratroot, "data/pmt/PMTINFO_sept2018.ratdb"))
    load_defaults_seconds = time.perf_counter() - start

    start = time.perf_counter()
    rat.utility().Get().BeginOfRun()
    utility_seconds = time.perf_counter() - start

    db.EndOfBeginOfRun()
    return {
        "database_run_number": run_number,
        "db_begin_seconds": begin_db_seconds,
        "load_defaults_seconds": load_defaults_seconds,
        "utility_begin_seconds": utility_seconds,
        "initialisation_seconds": (
            begin_db_seconds + load_defaults_seconds + utility_seconds
        ),
    }


def calculate(input_path, output_path, method):
    airplane = method == "new"
    input_file, input_tree = _input_tree(input_path)
    names = _branch_names(input_tree)
    required = {"runID", "hitids", "hittimes", "posx", "posy", "posz", "time"}
    missing = sorted(required - names)
    if missing:
        raise RuntimeError(f"ntuple is missing branches: {', '.join(missing)}")

    input_tree.GetEntry(0)
    run_number = int(input_tree.runID)
    # A non-zero DB run locks out the local default plane even in airplane
    # mode.  The candidate deliberately uses run-0 local detector defaults;
    # its run-dependent AV displacement comes from the ntuple instead.
    database_run_number = run_number if method == "old" else 0
    timings = _initialise_rat(database_run_number, airplane)
    calculator = rat.utility().GetTimeResidualCalculator()
    psup_system_id = RAT.DU.Point3D.GetSystemId("innerPMT")

    output_file = ROOT.TFile.Open(output_path, "RECREATE")
    if not output_file or output_file.IsZombie():
        raise RuntimeError(f"could not create {output_path}")

    residual_tree = ROOT.TTree("residuals", "One time residual per PMT hit")
    entry_value = array("q", [0])
    hit_value = array("I", [0])
    pmt_value = array("I", [0])
    residual_value = array("d", [0.0])
    residual_tree.Branch("entry", entry_value, "entry/L")
    residual_tree.Branch("hit", hit_value, "hit/i")
    residual_tree.Branch("pmtid", pmt_value, "pmtid/i")
    residual_tree.Branch("residual", residual_value, "residual/D")
    histogram = ROOT.TH1D(
        "time_residuals", f"{method} time residuals;Time residual (ns);Hits",
        2000, -500.0, 500.0,
    )

    offsets = []
    hit_count = 0
    start = time.perf_counter()
    for entry_index, event in enumerate(input_tree):
        if int(event.runID) != run_number:
            raise RuntimeError(
                "input contains multiple run numbers; split it by run before benchmarking"
            )
        position = ROOT.TVector3(event.posx, event.posy, event.posz)
        if method == "old":
            calculation_position = RAT.DU.Point3D(psup_system_id, position)
        else:
            av_offset = _event_av_offset(event, names)
            offsets.append(av_offset)

        if len(event.hitids) != len(event.hittimes):
            raise RuntimeError(f"entry {entry_index} has mismatched hit vectors")
        for hit_index, (pmtid, pmt_time) in enumerate(
            zip(event.hitids, event.hittimes)
        ):
            if method == "old":
                residual = calculator.CalcTimeResidual(
                    pmtid, pmt_time, calculation_position, event.time
                )
            else:
                residual = calculator.CalcTimeResidual(
                    pmtid, pmt_time, position, event.time, av_offset
                )
            if not math.isfinite(residual):
                raise RuntimeError(
                    f"non-finite residual at entry {entry_index}, hit {hit_index}"
                )
            entry_value[0] = entry_index
            hit_value[0] = hit_index
            pmt_value[0] = pmtid
            residual_value[0] = residual
            residual_tree.Fill()
            histogram.Fill(residual)
            hit_count += 1
    calculation_seconds = time.perf_counter() - start

    metadata = {
        "input": input_path,
        "method": method,
        "airplane": airplane,
        "run_number": run_number,
        "entries": int(input_tree.GetEntries()),
        "hits": hit_count,
        "calculation_seconds": calculation_seconds,
        "microseconds_per_hit": 1e6 * calculation_seconds / hit_count,
        **timings,
    }
    if offsets:
        metadata["av_offset_min_mm"] = min(offsets)
        metadata["av_offset_max_mm"] = max(offsets)

    output_file.cd()
    residual_tree.Write()
    histogram.Write()
    ROOT.TNamed("benchmark_metadata", json.dumps(metadata, sort_keys=True)).Write()
    output_file.Close()
    input_file.Close()
    print(json.dumps(metadata, indent=2, sort_keys=True))


def _metadata(root_file):
    value = root_file.Get("benchmark_metadata")
    if not value:
        raise RuntimeError(f"{root_file.GetName()} has no benchmark metadata")
    return json.loads(value.GetTitle())


def compare(reference_path, candidate_path, output_path):
    reference_file = ROOT.TFile.Open(reference_path, "READ")
    candidate_file = ROOT.TFile.Open(candidate_path, "READ")
    reference = reference_file.Get("residuals")
    candidate = candidate_file.Get("residuals")
    if not reference or not candidate:
        raise RuntimeError("both inputs must contain a residuals tree")
    if reference.GetEntries() != candidate.GetEntries():
        raise RuntimeError(
            f"hit counts differ: {reference.GetEntries()} vs {candidate.GetEntries()}"
        )

    output_file = ROOT.TFile.Open(output_path, "RECREATE")
    difference_tree = ROOT.TTree("differences", "Candidate minus reference")
    entry_value = array("q", [0])
    hit_value = array("I", [0])
    pmt_value = array("I", [0])
    reference_value = array("d", [0.0])
    candidate_value = array("d", [0.0])
    difference_value = array("d", [0.0])
    for name, value, leaf in (
        ("entry", entry_value, "entry/L"),
        ("hit", hit_value, "hit/i"),
        ("pmtid", pmt_value, "pmtid/i"),
        ("reference", reference_value, "reference/D"),
        ("candidate", candidate_value, "candidate/D"),
        ("difference", difference_value, "difference/D"),
    ):
        difference_tree.Branch(name, value, leaf)
    difference_histogram = ROOT.TH1D(
        "residual_differences",
        "New airplane - old online;Residual difference (ns);Hits",
        2000, -1.0, 1.0,
    )

    count = 0
    total = 0.0
    total_squared = 0.0
    max_absolute = 0.0
    above_tolerance = 0
    tolerance = 1e-9
    for index in range(reference.GetEntries()):
        reference.GetEntry(index)
        candidate.GetEntry(index)
        reference_key = (int(reference.entry), int(reference.hit), int(reference.pmtid))
        candidate_key = (int(candidate.entry), int(candidate.hit), int(candidate.pmtid))
        if reference_key != candidate_key:
            raise RuntimeError(
                f"hit alignment differs at row {index}: {reference_key} vs {candidate_key}"
            )
        difference = float(candidate.residual - reference.residual)
        entry_value[0], hit_value[0], pmt_value[0] = reference_key
        reference_value[0] = reference.residual
        candidate_value[0] = candidate.residual
        difference_value[0] = difference
        difference_tree.Fill()
        difference_histogram.Fill(difference)
        count += 1
        total += difference
        total_squared += difference * difference
        max_absolute = max(max_absolute, abs(difference))
        above_tolerance += abs(difference) > tolerance

    mean = total / count
    rms = math.sqrt(total_squared / count)
    summary = {
        "hits": count,
        "mean_difference_ns": mean,
        "rms_difference_ns": rms,
        "max_absolute_difference_ns": max_absolute,
        "tolerance_ns": tolerance,
        "hits_above_tolerance": above_tolerance,
        "reference": _metadata(reference_file),
        "candidate": _metadata(candidate_file),
    }
    output_file.cd()
    difference_tree.Write()
    difference_histogram.Write()
    ROOT.TNamed("comparison_metadata", json.dumps(summary, sort_keys=True)).Write()
    output_file.Close()
    reference_file.Close()
    candidate_file.Close()
    print(json.dumps(summary, indent=2, sort_keys=True))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)
    for command, help_text in (
        ("reference", "old Point3D method with the online database"),
        ("candidate", "new explicit-offset method in airplane mode"),
    ):
        subparser = subparsers.add_parser(command, help=help_text)
        subparser.add_argument("input", help="input extended ntuple")
        subparser.add_argument("output", help="output ROOT file")
    compare_parser = subparsers.add_parser("compare", help="compare saved hits")
    compare_parser.add_argument("reference", help="reference ROOT file")
    compare_parser.add_argument("candidate", help="candidate ROOT file")
    compare_parser.add_argument("output", help="comparison ROOT file")
    args = parser.parse_args()

    if args.command == "reference":
        calculate(args.input, args.output, "old")
    elif args.command == "candidate":
        calculate(args.input, args.output, "new")
    else:
        compare(args.reference, args.candidate, args.output)


if __name__ == "__main__":
    main()
