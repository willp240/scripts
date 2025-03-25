#!/usr/bin/sh
source /home/parkerw/Software/env-dev_cpp17.sh
cd /home/parkerw/Scripts
python saveTree_tresidpmtz.py "/data/snoplus/parkerw/Feb5_alpha_6to14m/*.root" -o "./Feb5_alpha_pmtratio_6to14m_pmt_tresid_ratio.root"
