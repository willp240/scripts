#!/usr/bin/env bash
set -euo pipefail

BASE_DIR="/data/snoplus3/parkerw/ratSimulations"

directories=(
  "May17_cerenk0p06"
  "May17_cerenk0p06_defPDF"
  "May17_cerenk0p10"
  "May17_cerenk0p10_defPDF"
  "May17_defMC_cerenk0p06PDF"
  "May17_defMC_cerenk0p10PDF"
  "May17_defrisetime_defPDF"
  "May17_risetime0p70"
  "May17_risetime0p70_defaultPDF"
  "May17_risetime0p90"
  "May17_risetime0p90_defaultPDF"
  "May17_risetime1p10"
  "May17_risetime1p10_defaultPDF"
)

for directory in "${directories[@]}"; do
  input_glob="${BASE_DIR}/${directory}/*.root"
  output_file="${directory}_Tree.root"

  echo "[$(date '+%Y-%m-%d %H:%M:%S')] Starting ${directory}"
  python saveTree.py "${input_glob}" -o "${output_file}"
  echo "[$(date '+%Y-%m-%d %H:%M:%S')] Finished ${directory} -> ${output_file}"
done

echo "[$(date '+%Y-%m-%d %H:%M:%S')] All May17 saveTree jobs finished."
