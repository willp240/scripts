#!/usr/bin/env bash
set -euo pipefail

work_dir="/home/parkerw/Software/rat-tools_master/LivetimeTools"
rat_env="/home/parkerw/Software/rat_master/env.sh"

condor_dir="/data/snoplus3/parkerw/solarAntinu/livetimeCalculatorCondor"
script_dir="${condor_dir}/sh"
out_dir="${condor_dir}/output"
err_dir="${condor_dir}/error"
log_dir="${condor_dir}/log"

mkdir -p "$script_dir" "$out_dir" "$err_dir" "$log_dir"

write_job_script() {
  local tag="$1"
  local nhit="$2"
  local input="$3"
  local data_dir="$4"
  local veto_tag="$5"
  local extra_range_args="$6"

  local base="/data/snoplus3/parkerw/solarAntinu/May12_Tagging_${nhit}NhitVeto"
  local script="${script_dir}/${tag}.sh"

  cat > "$script" <<EOF
#!/usr/bin/env bash
set -euo pipefail

cd "$work_dir"
source "$rat_env"

mkdir -p "${base}/livetimeCalculatorOutput"

python scintillator_livetime_calculate.py \\
  --input "$input" \\
  $extra_range_args \\
  --data_dir "$data_dir" \\
  --veto_dir "${base}/livetimeVetoCondor/vetooutput/" \\
  --veto_tag "$veto_tag" \\
  --tag "$tag" \\
  --output_dir "${base}/livetimeCalculatorOutput"
EOF

  chmod +x "$script"
}

write_job_script \
  "PPO_3000NhitVeto_1s_run300600plus" \
  "3000" \
  "runlist_livetime_PPO.txt" \
  "/data/snoplus3/SNOplusData/processing/fullFill/rat-8.0.1/Analysis20_PPOR/ntuple/" \
  "PPOPhase" \
  "--lower_range 300600"

write_job_script \
  "bisMSB_3000NhitVeto_1s_published_cuts" \
  "3000" \
  "runlist_livetime_bisMSB_published_cuts.txt" \
  "/data/snoplus3/SNOplusData/processing/fullFill/rat-8.0.1/Analysis20_bMR/ntuple/,/data/snoplus3/SNOplusData/processing/fullFill/rat-8.0.1/ntuples_bMR/,/data/snoplus3/SNOplusData/processing/fullFill/rat-8.0.1/ntuples/" \
  "BisMSBPhase" \
  ""

write_job_script \
  "PPO_5000NhitVeto_1s_run300600plus" \
  "5000" \
  "runlist_livetime_PPO.txt" \
  "/data/snoplus3/SNOplusData/processing/fullFill/rat-8.0.1/Analysis20_PPOR/ntuple/" \
  "PPOPhase" \
  "--lower_range 300600"

write_job_script \
  "bisMSB_5000NhitVeto_1s_published_cuts" \
  "5000" \
  "runlist_livetime_bisMSB_published_cuts.txt" \
  "/data/snoplus3/SNOplusData/processing/fullFill/rat-8.0.1/Analysis20_bMR/ntuple/,/data/snoplus3/SNOplusData/processing/fullFill/rat-8.0.1/ntuples_bMR/,/data/snoplus3/SNOplusData/processing/fullFill/rat-8.0.1/ntuples/" \
  "BisMSBPhase" \
  ""

job_list="${condor_dir}/livetime_calculation_jobs.txt"
submit_file="${condor_dir}/livetime_calculations.submit"

: > "$job_list"
for script in "$script_dir"/*.sh; do
  tag="$(basename "$script" .sh)"
  printf '%s %s\n' "$script" "$tag" >> "$job_list"
done

cat > "$submit_file" <<EOF
universe = vanilla
executable = \$(script)
getenv = true
should_transfer_files = NO
request_memory = 4 GB

output = ${out_dir}/\$(tag).out
error = ${err_dir}/\$(tag).err
log = ${log_dir}/\$(tag).log

queue script, tag from $job_list
EOF

echo "Wrote job scripts to: $script_dir"
echo "Wrote submit file to: $submit_file"
echo "Wrote job list to: $job_list"
condor_submit "$submit_file"
