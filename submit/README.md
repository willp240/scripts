Change relevant file paths in subJobs.py

python submitJobs.py --help
usage: Launch a load of identical rat simulation jobs [-h] [-e ENV_FILE]
                                                      [-s SUBMISSION_DIRECTORY]
                                                      [-d RUN_DIRECTORY]
                                                      [-n NO_SIMS]
                                                      macro out_dir

positional arguments:
  macro                 template macro file to load
  out_dir               directory to place reprocessed files

optional arguments:
  -h, --help            show this help message and exit
  -e ENV_FILE, --env_file ENV_FILE
                        path to environment file
  -s SUBMISSION_DIRECTORY, --submission_directory SUBMISSION_DIRECTORY
                        path to the directory this file is in, for outputs and
                        inputs
  -d RUN_DIRECTORY, --run_directory RUN_DIRECTORY
                        base directory from which the scripts will be run
  -n NO_SIMS, --no_sims NO_SIMS
                        how many identical sims would you like to launch?

Macros folder is provided for convenience, but macros can be used from elsewhere.
condor_base.mac does not need to be changed