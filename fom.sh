#!/bin/bash

#make directory
DIR="/home/parkerw/Software/rat_b/fomplots5"
mkdir -p $DIR

#get input script
SCRIPTNAME="analysis"
SCRIPT="${DIR}/${SCRIPTNAME}.cpp"
initialFile=XX

#for i 0-100
for i in {0..100}; do
    #copy i to input/output files in script
    #put new script in dir
    sed "s/${initialFile}/${i}/g" ${SCRIPT} > ${DIR}/${SCRIPTNAME}_${i}.cpp
    #run script
    command="root -l -b -q ${DIR}/${SCRIPTNAME}_${i}.cpp"
    eval $command
done
