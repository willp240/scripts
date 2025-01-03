#!/bin/bash

export SNOCAVE=/data/snoplus/software/snocave_Alma9
export RAT_SCONS=$SNOCAVE/scons-3.1.2

G4BASE=$SNOCAVE//geant4.10.0.p04
source $SNOCAVE/geant4.10.00.p04/bin/geant4.sh
GSLBASE=$SNOCAVE/gsl-1.16

source $SNOCAVE/root-6.30.06/obj/bin/thisroot.sh

export PATH=$ROOTSYS/bin:$G4BASE/bin:$GSLBASE/bin:$PATH
export PYTHONPATH=$ROOTSYS/lib:$PYTHONPATH
export LD_LIBRARY_PATH=$ROOTSYS/lib:$GSLBASE/lib:$CLHEP_BASE_DIR/lib:$LD_LIBRARY_PATH
export DYLD_LIBRARY_PATH=$ROOTSYS/lib:$GSLBASE/lib:$DYLD_LIBRARY_PATH

source /home/parkerw/Software/rat_master/env.sh
export PGPASSFILE=/home/parkerw/Software/rat_cpp17/.pgpass
