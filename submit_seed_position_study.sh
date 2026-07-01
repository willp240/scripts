#!/usr/bin/env bash
set -euo pipefail

SUBMISSION_DIR="/home/parkerw/Software/rat_submission"
RUN_DIR="/home/parkerw/Software/rat_1"
ENV_FILE="/home/parkerw/Software/env-dev_cpp17.sh"
N_JOBS=10
OUT_BASE="/data/snoplus3/parkerw/ratSimulations"
MACRO_DIR="seed_position_direction_macros"

submit_sample() {
    local tag="$1"
    local seed="$2"
    local macro="${MACRO_DIR}/Template_Macro_SeedPosition_${tag}_${seed}.mac"
    local out_dir="${OUT_BASE}/May26_seedposition_${tag}_${seed}"
    local batch="seedpos_${tag}_${seed}"

    python ../rat_submission/submitJobs.py "$macro" "$out_dir" \
        -e "$ENV_FILE" \
        -s "$SUBMISSION_DIR" \
        -d "$RUN_DIR" \
        -b "$batch" \
        -n "$N_JOBS"
}

for tag in default cerenk0p06 cerenk0p10 risetime0p70 risetime0p90 risetime1p10; do
    submit_sample "$tag" multipdfseed
    #submit_sample "$tag" defaultseed
done
