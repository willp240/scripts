#!/usr/bin/env python3
"""Benchmark proposed storage-only standard-ntuple tidy changes.

This script rewrites existing flat ntuple ROOT files into a sequence of
cumulative schema/compression variants.  It is intended for storage studies
only; it does not run RAT reconstruction.
"""

from __future__ import annotations

import argparse
import array
import ctypes
import glob
import json
import math
import os
from pathlib import Path
from typing import Any, Callable

import ROOT


TREE_NAME = "output"


DEFAULT_PPOR = (
    "/data/snoplus3/SNOplusData/processing/fullFill/rat-8.2.0/"
    "Analysis50_PPOR/ntuples/*.root"
)
DEFAULT_BMR = (
    "/data/snoplus3/SNOplusData/processing/fullFill/rat-8.2.0/"
    "Analysis50_bMR/ntuples/*.root"
)


def expand_inputs(inputs: list[str], max_files: int | None) -> list[str]:
    files: list[str] = []
    for item in inputs:
        path = Path(item)
        if path.is_dir():
            files.extend(str(p) for p in sorted(path.glob("*.root")))
        else:
            matches = sorted(glob.glob(item))
            files.extend(matches if matches else [item])
    files = [f for f in files if Path(f).is_file()]
    if max_files is not None:
        files = files[:max_files]
    return files


def compression_settings(spec: str | None) -> int | None:
    if not spec:
        return None
    if spec.isdigit():
        return int(spec)
    algo_name, level_text = spec.replace(" ", "").upper().split(":", 1)
    level = int(level_text)
    algos = {
        "ZLIB": ROOT.RCompressionSetting.EAlgorithm.kZLIB,
        "LZMA": ROOT.RCompressionSetting.EAlgorithm.kLZMA,
        "LZ4": ROOT.RCompressionSetting.EAlgorithm.kLZ4,
        "ZSTD": ROOT.RCompressionSetting.EAlgorithm.kZSTD,
    }
    return ROOT.CompressionSettings(algos[algo_name], level)


def branch_type(branch: Any) -> str:
    leaves = branch.GetListOfLeaves()
    if leaves.GetEntries() != 1:
        raise RuntimeError(f"Unhandled multi-leaf branch {branch.GetName()}")
    leaf = leaves[0]
    if leaf.GetLen() != 1 or leaf.GetLeafCount():
        raise RuntimeError(f"Unhandled counted/array branch {branch.GetName()}")
    return leaf.GetTypeName()


def make_scalar_holder(type_name: str) -> tuple[Any, str | None]:
    if type_name == "Double_t":
        return array.array("d", [0.0]), "/D"
    if type_name == "Float_t":
        return array.array("f", [0.0]), "/F"
    if type_name == "Int_t":
        return array.array("i", [0]), "/I"
    if type_name == "UInt_t":
        return array.array("I", [0]), "/i"
    if type_name == "ULong64_t":
        return array.array("Q", [0]), "/l"
    if type_name == "Bool_t":
        return ctypes.c_bool(False), "/O"
    if type_name == "TString":
        return ROOT.TString(), None
    if type_name == "vector<float>":
        return ROOT.std.vector("float")(), None
    if type_name == "vector<unsigned short>":
        return ROOT.std.vector("unsigned short")(), None
    raise RuntimeError(f"Unhandled branch type {type_name}")


def holder_value(holder: Any) -> Any:
    if isinstance(holder, ctypes.c_bool):
        return bool(holder.value)
    if isinstance(holder, ROOT.TString):
        return holder
    if hasattr(holder, "__getitem__"):
        return holder[0]
    return holder


def set_holder(holder: Any, value: Any) -> None:
    if isinstance(holder, ctypes.c_bool):
        holder.value = bool(value)
    elif hasattr(holder, "__setitem__"):
        holder[0] = value
    else:
        raise RuntimeError(f"Cannot set holder {holder!r}")


def max_abs_error(pos: Any, neg: Any) -> float:
    p = float(holder_value(pos))
    n = float(holder_value(neg))
    if p <= -99990 and n <= -99990:
        return -99999.0
    return max(abs(p), abs(n))


def remove_exact(names: set[str]) -> Callable[[str], bool]:
    return lambda name: name in names


def remove_prefix(prefixes: tuple[str, ...]) -> Callable[[str], bool]:
    return lambda name: name.startswith(prefixes)


def remove_regex_like(name: str) -> bool:
    return (
        name.startswith("penergy")
        or name in {
            "annR",
            "posx_MultiPath",
            "posy_MultiPath",
            "posz_MultiPath",
            "posr_MultiPath",
            "time_MultiPath",
            "fitValid_MultiPath",
            "posFOM_Multipath",
            "posFOM2_Multipath",
            "posz_fastZ",
            "time_fastZ",
            "fitValid_fastZ",
        }
    )


STEPS = [
    {
        "name": "00_baseline_rewrite",
        "drop": lambda _name: False,
        "convert_doubles": False,
        "consolidate_position_errors": False,
        "replace_av": False,
    },
    {
        "name": "01_remove_bipo_xsite",
        "drop": remove_exact(
            {"xsite", "biPoCumul", "biPoLikelihood212", "biPoLikelihood214"}
        ),
        "convert_doubles": False,
        "consolidate_position_errors": False,
        "replace_av": False,
    },
    {
        "name": "02_remove_alacarte_fitters",
        "drop": remove_regex_like,
        "convert_doubles": False,
        "consolidate_position_errors": False,
        "replace_av": False,
    },
    {
        "name": "03_consolidate_position_errors",
        "drop": remove_exact(
            {
                "posxPosError",
                "posxNegError",
                "posyPosError",
                "posyNegError",
                "poszPosError",
                "poszNegError",
            }
        ),
        "convert_doubles": False,
        "consolidate_position_errors": True,
        "replace_av": False,
    },
    {
        "name": "04_remove_position_seed",
        "drop": remove_prefix(("seed",)),
        "convert_doubles": False,
        "consolidate_position_errors": True,
        "replace_av": False,
    },
    {
        "name": "05_remove_intimehits",
        "drop": remove_exact({"inTimeHits20", "inTimeHits100"}),
        "convert_doubles": False,
        "consolidate_position_errors": True,
        "replace_av": False,
    },
    {
        "name": "06_replace_av_coordinates",
        "drop": remove_exact({"posr_av", "posz_av", "mcPosr_AV", "mcPosz_AV"}),
        "convert_doubles": False,
        "consolidate_position_errors": True,
        "replace_av": True,
    },
    {
        "name": "07_convert_doubles_to_floats",
        "drop": lambda _name: False,
        "convert_doubles": True,
        "consolidate_position_errors": True,
        "replace_av": True,
    },
    {
        "name": "08_compression",
        "drop": lambda _name: False,
        "convert_doubles": True,
        "consolidate_position_errors": True,
        "replace_av": True,
    },
]


def cumulative_step(index: int) -> dict[str, Any]:
    drop_functions = [step["drop"] for step in STEPS[1 : index + 1]]

    def drop(name: str) -> bool:
        return any(func(name) for func in drop_functions)

    return {
        "name": STEPS[index]["name"],
        "drop": drop,
        "convert_doubles": any(step["convert_doubles"] for step in STEPS[: index + 1]),
        "consolidate_position_errors": any(
            step["consolidate_position_errors"] for step in STEPS[: index + 1]
        ),
        "replace_av": any(step["replace_av"] for step in STEPS[: index + 1]),
    }


def copy_one_file(
    input_file: str,
    output_file: Path,
    step: dict[str, Any],
    compression: int | None,
) -> dict[str, Any]:
    source = ROOT.TFile.Open(input_file)
    if not source or source.IsZombie():
        raise RuntimeError(f"Could not open {input_file}")
    source_tree = source.Get(TREE_NAME)
    if not source_tree:
        raise RuntimeError(f"Could not find tree {TREE_NAME} in {input_file}")

    output_file.parent.mkdir(parents=True, exist_ok=True)
    if output_file.exists():
        output_file.unlink()

    output = ROOT.TFile.Open(str(output_file), "RECREATE")
    if compression is None:
        output.SetCompressionSettings(source.GetCompressionSettings())
    else:
        output.SetCompressionSettings(compression)
    target_tree = ROOT.TTree(TREE_NAME, TREE_NAME)

    input_holders: dict[str, Any] = {}
    output_holders: list[Any] = []
    double_copies: list[tuple[Any, Any]] = []
    computed: list[tuple[Any, Callable[[], float]]] = []
    kept_branches: list[str] = []
    dropped_branches: list[str] = []
    converted_doubles = 0

    source_names = {br.GetName() for br in source_tree.GetListOfBranches()}

    for branch in source_tree.GetListOfBranches():
        name = branch.GetName()
        type_name = branch_type(branch)
        holder, leaf_suffix = make_scalar_holder(type_name)
        source_tree.SetBranchAddress(name, holder)
        input_holders[name] = holder

        if step["drop"](name):
            dropped_branches.append(name)
            continue

        output_type = type_name
        output_holder = holder
        if step["convert_doubles"] and type_name == "Double_t":
            output_type = "Float_t"
            output_holder = array.array("f", [0.0])
            double_copies.append((holder, output_holder))
            converted_doubles += 1

        if output_type != type_name:
            _unused, output_suffix = make_scalar_holder(output_type)
        else:
            output_suffix = leaf_suffix

        if output_suffix:
            target_tree.Branch(name, output_holder, name + output_suffix)
        else:
            target_tree.Branch(name, output_holder)
        output_holders.append(output_holder)
        kept_branches.append(name)

    if step["consolidate_position_errors"]:
        for coord in ("x", "y", "z"):
            pos_name = f"pos{coord}PosError"
            neg_name = f"pos{coord}NegError"
            if pos_name in source_names and neg_name in source_names:
                holder = array.array("f" if step["convert_doubles"] else "d", [0.0])
                suffix = "/F" if step["convert_doubles"] else "/D"
                target_tree.Branch(f"pos{coord}Error", holder, f"pos{coord}Error{suffix}")
                computed.append(
                    (
                        holder,
                        lambda p=input_holders[pos_name], n=input_holders[neg_name]: max_abs_error(
                            p, n
                        ),
                    )
                )
                output_holders.append(holder)

    if step["replace_av"]:
        holder = array.array("f" if step["convert_doubles"] else "d", [0.0])
        suffix = "/F" if step["convert_doubles"] else "/D"
        target_tree.Branch("av_offset", holder, "av_offset" + suffix)

        def get_offset() -> float:
            for psup_name, av_name in (("posz", "posz_av"), ("mcPosz", "mcPosz_AV")):
                if psup_name in input_holders and av_name in input_holders:
                    psup = float(holder_value(input_holders[psup_name]))
                    av = float(holder_value(input_holders[av_name]))
                    if psup > -99990 and av > -99990:
                        return psup - av
            return -99999.0

        computed.append((holder, get_offset))
        output_holders.append(holder)

    entries = source_tree.GetEntries()
    for entry in range(entries):
        source_tree.GetEntry(entry)
        for in_holder, out_holder in double_copies:
            set_holder(out_holder, holder_value(in_holder))
        for holder, func in computed:
            set_holder(holder, func())
        target_tree.Fill()

    target_tree.Write()
    output.Close()
    source.Close()

    check = ROOT.TFile.Open(str(output_file))
    check_tree = check.Get(TREE_NAME)
    result = {
        "input": input_file,
        "output": str(output_file),
        "entries": int(entries),
        "input_size": os.path.getsize(input_file),
        "output_size": os.path.getsize(output_file),
        "tree_zip_bytes": int(check_tree.GetZipBytes()),
        "tree_total_bytes": int(check_tree.GetTotBytes()),
        "compression_settings": int(check.GetCompressionSettings()),
        "kept_branches": len(kept_branches) + len(computed),
        "dropped_branches": dropped_branches,
        "converted_doubles": converted_doubles,
    }
    check.Close()
    return result


def summarize(stage_results: list[dict[str, Any]], baseline_size: int) -> dict[str, Any]:
    output_size = sum(item["output_size"] for item in stage_results)
    tree_zip = sum(item["tree_zip_bytes"] for item in stage_results)
    entries = sum(item["entries"] for item in stage_results)
    return {
        "files": len(stage_results),
        "entries": entries,
        "output_size": output_size,
        "tree_zip_bytes": tree_zip,
        "size_ratio_to_input": output_size / baseline_size if baseline_size else math.nan,
        "bytes_per_entry": output_size / entries if entries else math.nan,
    }


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Rewrite ntuples with cumulative storage tidy changes."
    )
    parser.add_argument("inputs", nargs="*", help="Input ROOT files, globs, or directories.")
    parser.add_argument(
        "-o",
        "--output-dir",
        default="/tmp/ntuple_tidy_benchmark",
        help="Directory for rewritten files and summary JSON.",
    )
    parser.add_argument(
        "--max-files",
        type=int,
        default=None,
        help="Maximum files to use from the expanded input list.",
    )
    parser.add_argument(
        "--default-sample",
        action="store_true",
        help="Use 10 PPOR and 10 bMR files from the official rat-8.2.0 directories.",
    )
    parser.add_argument(
        "--compression",
        default="ZLIB:6",
        help="Compression for the final compression stage, e.g. ZLIB:6, ZSTD:3, or 106.",
    )
    parser.add_argument(
        "--start-step",
        type=int,
        default=0,
        help="First step index to write.",
    )
    parser.add_argument(
        "--stop-step",
        type=int,
        default=len(STEPS) - 1,
        help="Last step index to write.",
    )
    args = parser.parse_args()

    inputs = list(args.inputs)
    if args.default_sample:
        inputs = expand_inputs([DEFAULT_PPOR], 100) + expand_inputs([DEFAULT_BMR], 100)
    files = expand_inputs(inputs, args.max_files)
    if not files:
        raise SystemExit("No input files found.")

    out_dir = Path(args.output_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    final_compression = compression_settings(args.compression)
    input_total = sum(os.path.getsize(path) for path in files)

    all_results: dict[str, Any] = {
        "inputs": files,
        "input_total_size": input_total,
        "final_compression": args.compression,
        "stages": {},
    }

    print(f"Input files: {len(files)}")
    print(f"Input total: {input_total / 1024**2:.2f} MiB")

    for index in range(args.start_step, args.stop_step + 1):
        step = cumulative_step(index)
        compression = final_compression if index == len(STEPS) - 1 else None
        stage_dir = out_dir / step["name"]
        stage_results = []
        print(f"\n[{index}] {step['name']}")
        for file_index, input_file in enumerate(files):
            output_file = stage_dir / Path(input_file).name
            result = copy_one_file(input_file, output_file, step, compression)
            stage_results.append(result)
            print(
                f"  {file_index + 1:02d}/{len(files):02d} "
                f"{Path(input_file).name}: "
                f"{result['output_size'] / 1024**2:.2f} MiB"
            )
        summary = summarize(stage_results, input_total)
        all_results["stages"][step["name"]] = {
            "summary": summary,
            "files": stage_results,
        }
        print(
            f"  total {summary['output_size'] / 1024**2:.2f} MiB "
            f"({100 * summary['size_ratio_to_input']:.2f}% of input), "
            f"{summary['bytes_per_entry']:.1f} bytes/entry"
        )

    summary_path = out_dir / "summary.json"
    summary_path.write_text(json.dumps(all_results, indent=2, sort_keys=True))
    print(f"\nWrote {summary_path}")


if __name__ == "__main__":
    main()
