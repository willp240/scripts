#!/bin/bash -l

# Script to push new RAT releases to cvmfs
# By Jamie Rajewski <jrajewsk@ualberta.ca>

# Set the program to exit the entire thing when ctrl+c is hit
trap "exit" INT

printf "\t\t\t\t\t[ CVMFS Pusher ]\n"
printf "\tAny comments, questions, issues, please contact Jamie Rajewski <jrajewsk@ualberta.ca>\n\n"


# Ask the user

while true; do
    printf "\nHave you initialized your proxy using 'voms-proxy-init' (This requires snoprod GRID passphrase)? (y/n): "
    read RESP
    case "$RESP" in
        y|Y ) printf "\nContinuing...\n" && break;;
        n|N ) printf "Please follow these steps and retry. Exiting...\n" && exit 0;;
        * ) printf "Invalid answer; please try again\n";;
    esac
done


printf "\nTesting cvmfs/ connection...\n"

# Test cvmfs connection
PROBE=$(cvmfs_config probe grid.cern.ch snoplus.egi.eu)
if [ $? -eq 1 ]; then
    printf "Unable to reach /cvmfs/grid.cern.ch or /cvmfs/snoplus.egi.eu, exiting...\n"
    exit 1
fi

printf "\ncvmfs/ is fully accessible!\n"

printf "\nTesting the grid and voms proxies...\n"

# Test the grid proxy and the VOMS proxy
PROXY_CODE=$(grid_production)
if [ "$PROXY_CODE" -eq 0 ]; then
    printf "Proxies are invalid, please contact Jamie or Fady to restart them.\n"
    exit 1
fi

printf "\nProxies appear to be working! (If you are prompted for a password when rsync'ing, you need to re-run voms-proxy-init\n"

printf "\nCreating cvmfs directory structure and copying SCons to it...\n"

SCONS_DIRNAME=scons-3.1.2
SCONS_PATH=~/temp_Will/$SCONS_DIRNAME

# Set up the directory structure
mkdir -p ./cvmfs/sl7/sw/dependencies/$SCONS_DIRNAME

# Copy scons to this new path, and rename it
#cp -r ${SCONS_PATH}/. ./cvmfs/sl7/sw/dependencies/$SCONS_DIRNAME

printf "\nAll pre-requisite tests completed successfully\n"

printf "\nChanging permissions of RAT...\n"

# After fixing the scripts, change the permissions across the files
#chmod -R a+r .
#find . -perm /u+x -print0 | xargs -L1 -0 chmod a+x
#cd ..


printf "\nEverything is ready, now copying to cvmfs...\n"

# Now we are ready to rsync to cvmfs; remember that it can take awhile
# (around an hour usually) to propagate

# Set the gsissh command to a new enough version
GSICMD=/cvmfs/grid.cern.ch/centos7-umd4-ui-4_200423/usr/bin/gsissh

rsync -avz -P -e "$GSICMD -p 1975" ./cvmfs/sl7 cvmfs-upload01.gridpp.rl.ac.uk:~/cvmfs_repo

SUCCESS=$?
if [ $SUCCESS -ne 0 ]; then
    printf "\nThe sync to CVMFS failed; please correct the error and try again."
    exit 1
fi
printf "\nSync complete. Your new RAT version is now available on cvmfs; it can take ~1 hour to propagate but remember to test it.\n"

# Now we need to add the new version to the benchmarking document on couchDB


printf "\nDone, now exiting...\n"
