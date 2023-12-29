#!/usr/bin/sh

#################################################################
### Template bash script to FitPerformanceTool jobs on a cluster
### Options are written in by the scripts in job_sub
#################################################################

source ${env_file}
sleep ${sleep}
cd ${submission_dir}

python fit_performance_tool.py ${input_files} ${coord} -f quad -o ${output_file} -c plotting_1D.cfg --autoview  --debug
