#!/usr/bin/sh
source /home/parkerw/Software/env-dev_cpp17.sh
cd /home/parkerw/Scripts
python saveTree_tresidpmtz.py "/data/snoplus/parkerw/Jan8_NearAV_0to6m/*.root" -o "./Jan8_NearAV__0to6m_pmt_tresid_ratio.root"
