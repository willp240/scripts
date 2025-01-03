#!/bin/bash
## get linux version
export RELEASE_VERSION_NUMBER=$(lsb_release -r  | sed -r 's/[^0-9]*([0-9]+)\..*$/\1/')
if [ $RELEASE_VERSION_NUMBER == 7 ]; then
  export MYBASE="SL7"
fi
if [ $RELEASE_VERSION_NUMBER == 6 ]; then
  export MYBASE="SL6"
fi
## set variables for linux version
if [ $MYBASE == "SL7" ]; then
  # modify these to point to the SL7 installations:
  export RATROOT=/home/parkerw/Software/rat_master
  export SNOCAVE=/data/snoplus/software/snocave_CentOS7
fi
if [ $MYBASE == "SL6" ]; then
  # modify these to point to the SL6 installations:
  export RATROOT=/home/parkerw/Software/rat_master
  export SNOCAVE=/data/snoplus/software/snocave_SL6
fi

# rat environment
export ROOTSYS=$SNOCAVE/root-5.34.38
source /data/snoplus/software/snocave_CentOS7/root-5.34.38/bin/thisroot.sh

export CLHEP_BASE_DIR=$SNOCAVE/clhep/x86_64-cc7-gcc48-opt/
G4BASE=$SNOCAVE/geant4.10.0.p02
GSLBASE=$SNOCAVE/gsl-1.16
source $G4BASE/bin/geant4.sh
export PATH=$ROOTSYS/bin:$G4BASE/bin:$GSLBASE/bin:$PATH:/network/software/el7/compilers/gcc/8.1.0/bin/
export PYTHONPATH=$ROOTSYS/lib:$PYTHONPATH
export LD_LIBRARY_PATH=$ROOTSYS/lib:$GSLBASE/lib:$CLHEP_BASE_DIR/lib:$LD_LIBRARY_PATH
export DYLD_LIBRARY_PATH=$ROOTSYS/lib:$GSLBASE/lib:$DYLD_LIBRARY_PATH
# bootstrap
export RAT_CFLAGS=""
export RATSYSTEM=Linux
# overwrite bootstrap variables to point to local installation
if [ -f "$RATROOT/env.sh" ]; then
    source $RATROOT/env.sh
fi
export PGPASSFILE=$RATROOT/.pgpass
