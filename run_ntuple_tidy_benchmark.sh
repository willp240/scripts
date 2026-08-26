#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 4 ]]; then
  echo "usage: $0 <compression> <start-step> <stop-step> <output-base>" >&2
  exit 2
fi

compression="$1"
start_step="$2"
stop_step="$3"
output_base="$4"

repo="/home/parkerw/Software/rat_cpp17"
safe_compression="${compression/:/}"
output_dir="${output_base}/${safe_compression}_steps${start_step}-${stop_step}"

cd "${repo}"

python3 python/rat/ntuple_tidy_benchmark.py \
  --default-sample \
  --compression "${compression}" \
  --start-step "${start_step}" \
  --stop-step "${stop_step}" \
  -o "${output_dir}"
