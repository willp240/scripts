#!/usr/bin/env bash
# Regenerate the October 8 plots; reuse successfully extracted event summaries.
set -euo pipefail
cd -- "$(dirname -- "${BASH_SOURCE[0]}")"
# The caller should first source /home/parkerw/Software/env-dev_cpp17.sh.
export ROOTSYS=/data/snoplus/software/snocave_Alma9/root-6.30.06/obj
export PYTHONPATH="$ROOTSYS/lib:${PYTHONPATH:-}"
export LD_LIBRARY_PATH="$ROOTSYS/lib:${LD_LIBRARY_PATH:-}"
energies=(1 2 4 6 10 15 20 25 30 40)
for sample in centre 004; do
    if [[ $sample == centre ]]; then
        campaign=Oct8_mono_e_1000
        z=0
        label=Centre
        ymax=400
    else
        campaign=Oct8_mono_e_004_1000
        z=4000
        label='(0, 0, 4 m)'
        ymax=400
    fi
    summary="plots/Oct8/${sample}_events.root"
    if [[ ! -f ${summary%.root}.json || ! -f $summary ]]; then
        python3 extract_mono_hits.py "/data/snoplus3/parkerw/ratSimulations/$campaign" \
            --z "$z" -o "$summary"
    fi
    python3 plot_nhits_rms.py "$summary" --monoenergies "${energies[@]}" \
        --clip-sigma 3 --label "$label" --ymax "$ymax" \
        -o "plots/Oct8/${sample}_nhits_rms_3sigma"
    if [[ $sample == 004 ]]; then
        python3 plot_nhits_rms.py "$summary" --monoenergies "${energies[@]}" \
            --clip-sigma 3 --label "$label" --ymax 350 \
            -o "plots/Oct8/${sample}_nhits_rms_3sigma_ymax350"
    fi
    python3 plot_nhits_rms.py "$summary" --monoenergies "${energies[@]}" \
        --label "$label" -o "plots/Oct8/${sample}_nhits_rms_uncut"
    python3 plot_mono_hit_distributions.py "$summary" --clip-sigma 3 --label "$label" \
        -o "plots/Oct8/${sample}_distributions_3sigma"
    python3 plot_mono_hit_distributions.py "$summary" --label "$label" \
        -o "plots/Oct8/${sample}_distributions_uncut"
    python3 plot_mono_gaussian_fits.py "$summary" --label "$label" --ymax 200 --xmax 100 \
        --style-from "plots/Oct8/${sample}_nhits_rms_3sigma.root" \
        --comparison-csv "plots/Oct8/${sample}_nhits_rms_3sigma.csv" \
        -o "plots/Oct8/${sample}_gaussian"
done
