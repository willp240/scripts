import sys
import re
import os
import string
import argparse
    
def check_dir(dname):
    """Check if directory exists, create it if it doesn't"""
    if(dname[-1] != "/"):
        dname = dname + "/"
    direc = os.path.dirname(dname)
    try:
        os.stat(direc)
    except:
        os.makedirs(direc)
        print ("Made directory %s...." % dname)
    return dname

def pycondor_submit(batch_name, env_file, sub_dir, out_dir, in_reproc_filename, in_orig_filename, sleep_time = 1, priority = 5):
    '''
    submit a job to condor, write a sh file to source environment and execute command
    then write a submit file to be run by condor_submit
    '''

    print (in_reproc_filename)

    inputbasename = os.path.basename(os.path.normpath(in_reproc_filename))
    outfile = out_dir + "/" + inputbasename + "_tree.root"
    
    ### set a condor path to be called later
    condor_path = "{0}/".format(out_dir)

    ### write sh file (which sources environments, runs commands)

    base_filename = "{0}/condor_base_root.mac".format(sub_dir)
    in_macro_file = open(base_filename, "r")
    raw_text = string.Template(in_macro_file.read())
    in_macro_file.close()

    other_commands = 'sleep $[($RANDOM%'+str(sleep_time+1)+')+1]s'
    out_macro_text = raw_text.substitute(sh_sub=env_file, other_commands_sub=other_commands, reprocfile=in_reproc_filename, origfile=in_orig_filename, outfile=outfile)

    sh_filepath = "{0}/sh/".format(condor_path)+str(inputbasename).replace(".root", "")+'.sh'
    if not os.path.exists(os.path.dirname(sh_filepath)):
        os.makedirs(os.path.dirname(sh_filepath))
    sh_file = open(sh_filepath, "w")
    sh_file.write(out_macro_text)
    sh_file.close()
    os.chmod(sh_filepath, 0o777)
    
    ### now create submit file
    error_path = os.path.abspath('{0}/error'.format(condor_path))
    output_path = os.path.abspath('{0}/output'.format(condor_path))
    log_path = os.path.abspath('{0}/log'.format(condor_path))
    submit_path = os.path.abspath('{0}/submit'.format(condor_path))

    universe = "vanilla"
    notification = "never"
    n_rep = 1
    getenv = "False" # "False"

    submit_filepath = os.path.join(submit_path, inputbasename)
    submit_filepath += ".submit"
    out_submit_text = "executable              = "+str(sh_filepath)+"\n"+\
                     "universe                = "+str(universe)+"\n"+\
                     "output                  = "+str(output_path)+"/"+str(inputbasename)+".output\n"+\
                     "error                   = "+str(error_path)+"/"+str(inputbasename)+".error\n"+\
                     "log                     = "+str(log_path)+"/"+str(inputbasename)+".log\n"+\
                     "notification            = "+str(notification)+"\n"+\
                     "priority                = "+str(priority)+"\n"+\
                     "getenv                  = "+str(getenv)+"\n"+\
                     "request_memory          = 4 GB\n"+\
                     "transfer_input_files    = "+str(in_reproc_filename)+"\n"+\
                     "should_transfer_files   = YES\n"+\
                     "when_to_transfer_output = ON_EXIT_OR_EVICT\n"+\
                     "stream_output           = true\n"+\
                     "stream_error            = true\n"+\
                     "periodic_hold           = ((ResidentSetSize =!= undefined ? ResidentSetSize : 0) > 1500 * RequestMemory)\n"+\
                     "queue "+str(n_rep)+"\n"

    ## check and create output path
    if not os.path.exists(os.path.dirname(submit_filepath)):
        os.makedirs(os.path.dirname(submit_filepath))
    out_submit_file = open(submit_filepath, "w")
    out_submit_file.write(out_submit_text)
    out_submit_file.close()
    
    command = 'condor_submit -batch-name \"'+batch_name+'\" '+submit_filepath
    print ("executing job: ", command)
    os.system(command)


if __name__ == "__main__":

    parser = argparse.ArgumentParser("Launch a load of identical rat simulation jobs")
    parser.add_argument('out_dir', type=str, help='directory to place reprocessed files')
    parser.add_argument('-e', '--env_file', type=str,
                        help='path to environment file',
                        default="/path/to/environment/env.sh")
    parser.add_argument("-s", "--submission_directory", type=str,
                       default="/path/to/this/repository/rat_submission/",
                       help="path to the directory this file is in, for outputs and inputs")
    parser.add_argument("-r", "--input_reproc_dir", type=str,
                        default="input_reproc_dir",
                        help="reproc input files")
    parser.add_argument("-o", "--input_orig_dir", type=str,
                        default="input_orig_dir",
                        help="orig input files")
    parser.add_argument("-b", "--batch_name", type=str,
			default="reproc_pair",
                        help="Batch name")
    args = parser.parse_args()

    ## check if output and condor directories exist, create if they don't
    out_dir = check_dir(args.out_dir)
    in_reproc_dir = check_dir(args.input_reproc_dir)
    in_orig_dir = check_dir(args.input_orig_dir)

    condor_directory = "{0}/condor".format(args.submission_directory)

    log_dir = check_dir("{0}/log/".format(out_dir))
    error_dir = check_dir("{0}/error/".format(out_dir))
    sh_dir = check_dir("{0}/sh/".format(out_dir))
    submit_dir = check_dir("{0}/submit/".format(out_dir))
    output_dir = check_dir("{0}/output/".format(out_dir))

    env_file = args.env_file
    subdir = args.submission_directory
    
    batch_name = args.batch_name
    
    # pattern to extract run and part numbers
    pattern = re.compile(r"r(\d+)_s(\d+)_p(\d+)\.root")

    for in_name in os.listdir(in_reproc_dir):

        if not in_name.endswith(".root"):
            continue

        m = pattern.search(in_name)

        if not m:
            print(f"Skipping unrecognised filename: {in_name}")
            continue
        run, sub, part = m.groups()

        # now look for a corresponding file in match_dir
        corresponding = None
        for match_name in os.listdir(in_orig_dir):
            if f"r{run}" in match_name and f"p{part}" in match_name and f"s{sub}" in match_name and match_name.endswith(".root"):
                corresponding = match_name
                break

        if corresponding:
            print(f"Found match:\n  {in_name}\n  → {corresponding}")
        else:
            print(f"No match found for {in_name}")
        
        pycondor_submit(batch_name, env_file, subdir, out_dir, in_reproc_dir+"/"+in_name, in_orig_dir+"/"+corresponding, sleep_time = 1, priority = 5, )

