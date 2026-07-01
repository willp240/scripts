#!/usr/bin/env bash
set -euo pipefail

SUBMISSION_DIR="/home/parkerw/Software/rat_submission"
RUN_DIR="/home/parkerw/Software/rat_1"
ENV_FILE="/home/parkerw/Software/env-dev_cpp17.sh"

INPUT_DIR="/data/snoplus3/parkerw/ratSimulations/May17_defrisetime_defPDF"
INPUT_BASENAME="Template_Macro_FullVolume"
OUTPUT_BASE="/data/snoplus3/parkerw/ratSimulations"

MACRO_DIR="time_window_direction_macros"
N_JOBS=10

declare -a TAGS=(
  "m100_p300"
  "m10_p30"
  "m5_p15"
  "m5_p10"
  "m5_p5"
  "m5_p2"
  "m3_p2"
)

for tag in "${TAGS[@]}"; do
  macro="${MACRO_DIR}/Template_Macro_DirectionTimeWindow_${tag}.mac"
  out_dir="${OUTPUT_BASE}/May26_direction_timewindow_${tag}"

  python ../rat_submission/submitInputJobs.py \
    "$macro" \
    "$out_dir" \
    -e "$ENV_FILE" \
    -s "$SUBMISSION_DIR" \
    -d "$RUN_DIR" \
    -n "$N_JOBS" \
    -i "$INPUT_DIR" \
    -b "$INPUT_BASENAME"
done
