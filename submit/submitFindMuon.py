import sys
import os
import string
import argparse
import numpy as np
    
def check_dir(dname):
    """Check if directory exists, create it if it doesn't"""
    if(dname[-1] != "/"):
        dname = dname + "/"
    direc = os.path.dirname(dname)
    try:
        os.stat(direc)
    except:
        os.makedirs(direc)
        print "Made directory %s...." % dname
    return dname

def pycondor_submit(job_batch, job_id, run_num, out_dir, sleep_time = 1, priority = 5):
    '''
    submit a job to condor, write a sh file to source environment and execute command
    then write a submit file to be run by condor_submit
    '''

    print job_id

    ### set a condor path to be called later
    
    condor_path = "{0}/".format(out_dir)

    ### write sh file (which sources environments, runs commands)

    base_filename = "/home/parkerw/Software/rat_submission/condor_base_findmuon.mac"
    in_macro_file = open(base_filename, "r")
    raw_text = string.Template(in_macro_file.read())
    in_macro_file.close()

    other_commands = 'sleep $[($RANDOM%'+str(sleep_time+1)+')+1]s'
    root_file = "\"/data/snoplus3/SNOplusData/processing/fullFill/rat-7.0.8/ratds/Analysis20R_r0000"+str(run_num)+"_s*_p000.root\""
    command = "./find_muon " + root_file 

    out_macro_text = raw_text.substitute(other_commands_sub=other_commands, command=command)

    sh_filepath = "{0}/sh/".format(condor_path)+str(job_id).replace("/", "")+'.sh'
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

    submit_filepath = os.path.join(submit_path, job_id)
    submit_filepath += ".submit"
    out_submit_text = "executable              = "+str(sh_filepath)+"\n"+\
                     "universe                = "+str(universe)+"\n"+\
                     "output                  = "+str(output_path)+"/"+str(job_id)+".output\n"+\
                     "error                   = "+str(error_path)+"/"+str(job_id)+".error\n"+\
                     "log                     = "+str(log_path)+"/"+str(job_id)+".log\n"+\
                     "notification            = "+str(notification)+"\n"+\
                     "priority                = "+str(priority)+"\n"+\
                     "getenv                  = "+str(getenv)+"\n"+\
                     "queue "+str(n_rep)+"\n"

    ## check and create output path
    if not os.path.exists(os.path.dirname(submit_filepath)):
        os.makedirs(os.path.dirname(submit_filepath))
    out_submit_file = open(submit_filepath, "w")
    out_submit_file.write(out_submit_text)
    out_submit_file.close()
    
    command = 'condor_submit -batch-name \"'+job_batch+'\" '+submit_filepath
    print "executing job: "+command
    os.system(command)


if __name__ == "__main__":

    parser = argparse.ArgumentParser("Launch a load of identical rat simulation jobs")
    parser.add_argument('out_dir', type=str, help='directory to place reprocessed files')
    parser.add_argument('run_num', type=int, help='run number')
    args = parser.parse_args()

    out_dir_name = args.out_dir
    run_num = args.run_num
    ## check if output and condor directories exist, create if they don't
    out_dir = check_dir("/data/snoplus3/parkerw/ratSimulations/{0}_{1}".format(out_dir_name,run_num))

    log_dir = check_dir("{0}/log/".format(out_dir))
    error_dir = check_dir("{0}/error/".format(out_dir))
    sh_dir = check_dir("{0}/sh/".format(out_dir))
    submit_dir = check_dir("{0}/submit/".format(out_dir))
    output_dir = check_dir("{0}/output/".format(out_dir))

    job_id = out_dir_name
    batch_id = "rat_{0}".format(job_id)
    pycondor_submit(batch_id, job_id, run_num, out_dir, sleep_time = 1, priority = 5)
