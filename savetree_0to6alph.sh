#!/usr/bin/sh
source /home/parkerw/Software/env-dev_cpp17.sh
cd /home/parkerw/Scripts
python saveTree_tresidpmtz.py "/data/snoplus/parkerw/Feb5_alpha_0to6m/*.root" -o "./Feb5_alpha_pmtratio_0to6m_pmtratio.root"
