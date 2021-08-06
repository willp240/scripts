import sys
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
        print "Made directory %s...." % dname
    return dname

def pycondor_submit(job_batch, job_id, run_path, asmv_dir, fit_dir, dim, sleep_time = 1, priority = 5):
    '''
    submit a job to condor, write a sh file to source environment and execute command
    then write a submit file to be run by condor_submit
    '''

    print job_id

    ### set a condor path to be called later
    
    condor_path = "{0}/".format(fit_dir)

    ### write sh file (which sources environments, runs commands)
    base_filename = "{0}/condor_base.mac".format(run_path)
    in_macro_file = open(base_filename, "r")
    raw_text = string.Template(in_macro_file.read())
    in_macro_file.close()

    asmvfile = asmv_dir + "/asimov.h5"
    other_commands = 'sleep $[($RANDOM%'+str(sleep_time+1)+')+1]s'
    out_macro_text = raw_text.substitute(run_directory_sub=run_path, other_commands_sub=other_commands, asimovfile=asmvfile, dims=dim, outdir=fit_dir)

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

    parser = argparse.ArgumentParser("Launch a load of fits")
    parser.add_argument('asmv_dir', type=str, help='asimov directory')
    parser.add_argument('fit_dir', type=str, help='fit directory')
    parser.add_argument('dims', type=str, help='how many dimensions')
    parser.add_argument("-n", "--no_sims", type=int,
                       default=10,
                       help="how many identical fits would you like to run?")
    args = parser.parse_args()

    ## check if output and condor directories exist, create if they don't
    sub_dir = "/home/parkerw/Software/bb-likelihood-analysis/fit"
    asmv_dir = "/data/snoplus/parkerw/bb_sigex/" + args.asmv_dir
    asmv_dir = check_dir(asmv_dir)
    base_name = args.fit_dir
    over_dir = check_dir(asmv_dir+"/"+base_name) 

    dims = (args.dims)

    for i in range(args.no_sims):

        job_name = base_name + "_{0}".format(i)
        fit_dir = over_dir + "/" + job_name
        fit_dir = check_dir(fit_dir)
        dims = (args.dims)
        
        condor_directory = "{0}/condor".format(sub_dir)
        
        #    condor_dir = check_dir(condor_directory)
        log_dir = check_dir("{0}/log/".format(fit_dir))
        error_dir = check_dir("{0}/error/".format(fit_dir))
        sh_dir = check_dir("{0}/sh/".format(fit_dir))
        submit_dir = check_dir("{0}/submit/".format(fit_dir))
        output_dir = check_dir("{0}/output/".format(fit_dir))

        job_id = "{0}_{1}".format(base_name,i)
        batch_id = "{0}".format(args.fit_dir)
        pycondor_submit(batch_id, job_id, sub_dir, asmv_dir, fit_dir, dims, sleep_time = 1, priority = 5)
