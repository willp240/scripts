#!/usr/bin/env python

from __future__ import print_function

import argparse
import os
import subprocess
import sys


def parse_total_file(path):
    values = {}
    if not os.path.exists(path):
        return values
    with open(path) as handle:
        for line in handle:
            if ":" not in line:
                continue
            key, value = line.split(":", 1)
            try:
                values[key.strip()] = float(value.strip())
            except ValueError:
                values[key.strip()] = value.strip()
    return values


def build_command(args, cut_tag, output_tag):
    script = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                          "scintillator_livetime_calculate.py")
    cmd = [
        sys.executable,
        script,
        "--input", args.input,
        "--data_dir", args.data_dir,
        "--veto_dir", args.veto_dir,
        "--veto_tag", cut_tag,
        "--tag", output_tag,
        "--output_dir", args.output_dir,
        "--lower_range", str(args.lower_range),
        "--upper_range", str(args.upper_range),
        "--run_table_pass", str(args.run_table_pass),
        "--muon_table_pass", str(args.muon_table_pass),
    ]
    if args.ratdb_tag is not None:
        cmd.extend(["--ratdb_tag", args.ratdb_tag])
    if args.turn_off_file_checks:
        cmd.append("--turn_off_file_checks")
    if args.turn_off_muon_veto:
        cmd.append("--turn_off_muon_veto")
    if args.turn_off_pot_muon_veto:
        cmd.append("--turn_off_pot_muon_veto")
    if args.turn_off_bespoke_veto:
        cmd.append("--turn_off_bespoke_veto")
    return cmd


def main():
    parser = argparse.ArgumentParser(
        description="Run scintillator livetime calculations for multiple nhit follower veto tags."
    )
    parser.add_argument("--input", "-i", required=True,
                        help="File containing line-separated run numbers.")
    parser.add_argument("--cut", action="append", required=True,
                        help="Veto tag for one nhit follower cut. Repeat this option for each cut.")
    parser.add_argument("--tag_prefix", default="livetime",
                        help="Prefix for output tags. The cut tag is appended to this prefix.")
    parser.add_argument("--data_dir", "-d", default=os.getcwd(),
                        help="Directory containing Analysis*.ntuple.root files.")
    parser.add_argument("--veto_dir", default=os.getcwd(),
                        help="Directory containing *vetooutput.txt files.")
    parser.add_argument("--output_dir", "-o", default=os.getcwd(),
                        help="Directory where per-cut output directories will be written.")
    parser.add_argument("--lower_range", "-l", type=int, default=0)
    parser.add_argument("--upper_range", "-u", type=int, default=9999999)
    parser.add_argument("--run_table_pass", type=int, default=-1)
    parser.add_argument("--muon_table_pass", type=int, default=-1)
    parser.add_argument("--ratdb_tag", default=None)
    parser.add_argument("--turn_off_file_checks", action="store_true")
    parser.add_argument("--turn_off_muon_veto", action="store_true")
    parser.add_argument("--turn_off_pot_muon_veto", action="store_true")
    parser.add_argument("--turn_off_bespoke_veto", action="store_true")
    parser.add_argument("--dry_run", action="store_true",
                        help="Print the commands without running them.")
    args = parser.parse_args()

    summary_rows = []
    for cut_tag in args.cut:
        output_tag = "{}_{}".format(args.tag_prefix, cut_tag)
        cmd = build_command(args, cut_tag, output_tag)
        print("Running cut tag: {}".format(cut_tag))
        print(" ".join(cmd))
        if not args.dry_run:
            subprocess.check_call(cmd)
            total_file = os.path.join(args.output_dir, output_tag,
                                      "livetime_total_{}.txt".format(output_tag))
            totals = parse_total_file(total_file)
            summary_rows.append((
                cut_tag,
                totals.get("raw_livetime", ""),
                totals.get("livetime", ""),
            ))

    if summary_rows:
        summary_path = os.path.join(args.output_dir,
                                    "{}_cut_summary.txt".format(args.tag_prefix))
        with open(summary_path, "w") as handle:
            handle.write("cut_tag raw_livetime livetime\n")
            for row in summary_rows:
                handle.write("{} {} {}\n".format(*row))
        print("Wrote summary: {}".format(summary_path))


if __name__ == "__main__":
    main()
