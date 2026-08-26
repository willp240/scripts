cd /home/parkerw/Software/rat-tools_master/FitCoordination/Direction2DPDF

for d in /data/snoplus3/parkerw/ratSimulations/May26_seed*multipdfseed; do
  name=$(basename "$d")
  python saveTree.py "$d/*.root" -o "${name}_Tree.root"
done
