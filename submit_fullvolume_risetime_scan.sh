#!/usr/bin/env bash
set -euo pipefail

SUBMISSION_DIR="/home/parkerw/Software/rat_submission"
RUN_DIR="/home/parkerw/Software/rat_1"
ENV_FILE="/home/parkerw/Software/env-dev_cpp17.sh"

TEMPLATE="Template_Macro_FullVolume.mac"
MACRO_DIR="fullvolume_risetime_macros"
OUTPUT_BASE="/data/snoplus3/parkerw/ratSimulations"

N_JOBS=10
EVENTS_PER_JOB=1000

mkdir -p "$MACRO_DIR"

for centi in {70..90}; do
    rise_time="$(printf "0.%02d" "$centi")"
    tag="risetime${rise_time/./p}"
    macro="${MACRO_DIR}/Template_Macro_FullVolume_${tag}.mac"
    out_dir="${OUTPUT_BASE}/May21_${tag}"

    sed -E \
        -e "s#^/rat/db/set OPTICS\\[labppo_2p2_scintillator\\] SCINT_RISE_TIME .*#/rat/db/set OPTICS[labppo_2p2_scintillator] SCINT_RISE_TIME ${rise_time}#" \
        -e "s#^/rat/run/start .*#/rat/run/start ${EVENTS_PER_JOB}#" \
        "$TEMPLATE" > "$macro"

    python ../rat_submission/submitJobs.py \
        "$macro" \
        "$out_dir" \
        -e "$ENV_FILE" \
        -s "$SUBMISSION_DIR" \
        -d "$RUN_DIR" \
        -b "$tag" \
        -n "$N_JOBS"
done
