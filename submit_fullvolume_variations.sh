#!/usr/bin/env bash
set -euo pipefail

SUBMISSION_DIR="/home/parkerw/Software/rat_submission"
RUN_DIR="/home/parkerw/Software/rat_1"
ENV_FILE="/home/parkerw/Software/env-dev_cpp17.sh"
N_JOBS=10

#python ../rat_submission/submitJobs.py May14_risetime0p85/Template_Macro_FullVolume.mac /data/snoplus3/parkerw/ratSimulations/May17_defrisetime_defPDF -e "$ENV_FILE" -s "$SUBMISSION_DIR" -d "$RUN_DIR" -b risetime0p85 -n "$N_JOBS"
#python ../rat_submission/submitJobs.py May14_risetime0p90/Template_Macro_FullVolume.mac /data/snoplus3/parkerw/ratSimulations/May17_risetime0p90 -e "$ENV_FILE" -s "$SUBMISSION_DIR" -d "$RUN_DIR" -b risetime0p90 -n "$N_JOBS"
#python ../rat_submission/submitJobs.py May14_risetime1p10/Template_Macro_FullVolume.mac /data/snoplus3/parkerw/ratSimulations/May17_risetime1p10 -e "$ENV_FILE" -s "$SUBMISSION_DIR" -d "$RUN_DIR" -b risetime1p10 -n "$N_JOBS"
#python ../rat_submission/submitJobs.py May14_cerenk0p06/Template_Macro_FullVolume.mac /data/snoplus3/parkerw/ratSimulations/May17_defMC_cerenk0p06PDF -e "$ENV_FILE" -s "$SUBMISSION_DIR" -d "$RUN_DIR" -b defMC_cerenk0p06PDF -n "$N_JOBS"
#python ../rat_submission/submitJobs.py May14_cerenk0p10/Template_Macro_FullVolume.mac /data/snoplus3/parkerw/ratSimulations/May17_defMC_cerenk0p10PDF -e "$ENV_FILE" -s "$SUBMISSION_DIR" -d "$RUN_DIR" -b defMC_cerenk0p10PDF -n "$N_JOBS"

python ../rat_submission/submitJobs.py mixedrisetime_direction_macros/Template_Macro_MixedRiseTimeDirection_risetime0p70.mac /data/snoplus3/parkerw/ratSimulations/May26_mixedrisetimePDF0p70 -e "$ENV_FILE" -s "$SUBMISSION_DIR" -d "$RUN_DIR" -b risetime0p70 -n "$N_JOBS"
python ../rat_submission/submitJobs.py mixedrisetime_direction_macros/Template_Macro_MixedRiseTimeDirection_risetime0p85.mac /data/snoplus3/parkerw/ratSimulations/May26_mixedrisetimePDF0p85 -e "$ENV_FILE" -s "$SUBMISSION_DIR" -d "$RUN_DIR" -b risetime0p85 -n "$N_JOBS"
python ../rat_submission/submitJobs.py mixedrisetime_direction_macros/Template_Macro_MixedRiseTimeDirection_risetime0p90.mac /data/snoplus3/parkerw/ratSimulations/May26_mixedrisetimePDF0p90 -e "$ENV_FILE" -s "$SUBMISSION_DIR" -d "$RUN_DIR" -b risetime0p90 -n "$N_JOBS"
python ../rat_submission/submitJobs.py mixedrisetime_direction_macros/Template_Macro_MixedRiseTimeDirection_risetime1p10.mac /data/snoplus3/parkerw/ratSimulations/May26_mixedrisetimePDF1p10 -e "$ENV_FILE" -s "$SUBMISSION_DIR" -d "$RUN_DIR" -b risetime1p10 -n "$N_JOBS"
