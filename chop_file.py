import csv
import io
import argparse
from pathlib import Path

MAX_BYTES = 1_000_000_000

columns_inp = [
    "group", "capture", "flagall",
    "flaga1", "flagb1", "flagc1", "flagd1",
    "timea1", "timeb1", "timec1", "timed1",
    "dtimea1", "dtimeb1", "dtimec1", "dtimed1",
    "peaka1", "peakb1", "peakc1", "peakd1",
    "flagb3", "flagb4", "flagb5", "flagb6",
    "timeb3", "timeb4", "timeb5", "timeb6",
    "dtimeb3", "dtimeb4", "dtimeb5", "dtimeb6",
    "peakb3", "peakb4", "peakb5", "peakb6",
    "baseb3", "baseb4", "baseb5", "baseb6",
    "endtimeb3", "endtimeb4", "endtimeb5", "endtimeb6",
]

columns_to_keep = [
    "flaga1", "dtimea1", "peaka1",
    "flagb1", "dtimeb1", "peakb1",
    "flagc1", "dtimec1", "peakc1",
    "flagb3", "dtimeb3", "peakb3",
    "flagd1", "dtimed1", "peakd1",
]

positions = [columns_inp.index(name) for name in columns_to_keep]

def csv_bytes(values):
    buffer = io.StringIO(newline="")
    csv.writer(buffer).writerow(values)
    return buffer.getvalue().encode("utf-8")

parser = argparse.ArgumentParser(description="Prune and split a headerless CSV file.")
parser.add_argument("input_file", type=Path, help="CSV file to process")
args = parser.parse_args()
source_path = args.input_file

part = 0
output = None
size = 0

try:
    with source_path.open("r", encoding="utf-8-sig", newline="") as source:
        for line_number, row in enumerate(csv.reader(source), start=1):
            if len(row) != len(columns_inp):
                raise ValueError(
                    f"Record {line_number} has {len(row)} columns; "
                    f"expected {len(columns_inp)}"
                )

            record = csv_bytes([row[position] for position in positions])
            if len(record) > MAX_BYTES:
                raise ValueError(f"Record {line_number} exceeds the size limit")

            if output is None or size + len(record) > MAX_BYTES:
                if output is not None:
                    output.close()
                part += 1
                output_path = source_path.with_name(f"{source_path.stem}_part{part}.csv")
                output = output_path.open("wb")
                size = 0

            output.write(record)
            size += len(record)
finally:
    if output is not None:
        output.close()
