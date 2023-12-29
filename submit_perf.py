import string
import os
import utilities

### Check if directory exists, create it if it doesn't
def check_dir(dname):

    if(dname[-1] != "/"):
        dname = dname + "/"
    direc = os.path.dirname(dname)
    try:
        os.stat(direc)
    except:
        os.makedirs(direc)
        print ("Made directory %s...." % dname)
    return dname

### Writes the submission scripts and dag splices for running FitPerformanceTools
### "coord" is the coordinate the bias and resolution are to be plotted as a function of
def setup_tools_jobs(job_name, out_dir, in_dir, env_file, submission_dir, coord):

    ## Make a condor submit file from template
    template_condor_filename = "template_condor.sub"
    template_condor_file = open(template_condor_filename, "r")
    template_condor_raw_text = string.Template(template_condor_file.read())

    ## Make bash file from template
    template_sh_filename = "template_tools.sh"
    template_sh_file = open(template_sh_filename, "r")
    template_sh_raw_text = string.Template(template_sh_file.read())

    ### Setup output directories
    job_dir    = "{0}/{1}".format(out_dir, job_name)
    log_dir    = check_dir("{0}/log/".format(job_dir))
    error_dir  = check_dir("{0}/error/".format(job_dir))
    sh_dir     = check_dir("{0}/sh/".format(job_dir))
    submit_dir = check_dir("{0}/submit/".format(job_dir))
    output_dir = check_dir("{0}/output/".format(job_dir))
    dag_splice_text = ""

    input_files = "\"" + in_dir + "/*.root\""
    output_file = "{0}/{1}_{2}.root".format(job_dir, job_name, coord)

    ## And make the bash file to run
    sh_text = template_sh_raw_text.substitute(env_file=env_file,
                                              input_files=input_files,
                                              output_file = output_file,
                                              sleep = "$((1 + $RANDOM % 10))",
                                              submission_dir=submission_dir,
                                              coord=coord)
    sh_name = "{0}{1}_{2}.sh".format(sh_dir, job_name, coord)
    with open(sh_name, "w") as sh_file:
        sh_file.write(sh_text)
    os.chmod(sh_name, 0o777)

    ## And the condor submission macro
    sub_text = template_condor_raw_text.substitute(sh_file=sh_name,
                                                   error_file="{0}/{1}_{2}.error".format(error_dir, job_name, coord),
                                                   output_file="{0}/{1}_{2}.output".format(output_dir, job_name, coord),
                                                   log_file="{0}/{1}_{2}.log".format(log_dir, job_name, coord))
    sub_name = "{0}{1}_{2}.sub".format(submit_dir, job_name, coord)
    with open(sub_name, "w") as sub_file:
        sub_file.write(sub_text)

    command = 'condor_submit -batch-name \"fit_perf_tools\" '+sub_name
    print "executing job: "+command
    os.system(command)


if __name__ == "__main__":

    env_file = "/home/parkerw/Software/env-dev_5.34.38.sh"
    submission_dir = "/home/parkerw/Software/rat-tools_fork/FitPerformance/"
    data_dir = "/data/snoplus3/parkerw/ratSimulations/"

    true_sev_fv_2p5 = "Oct11_2p2glnewrecooord_2p5MeVallscint"
    true_sev_fv_1to10 = "Oct11_2p2glnewrecooord_1to10MeVallscint"
    reco_sev_fv_2p5 = "Oct11_2p2gl6mRecoord_2p5MeVallscint"
    reco_sev_fv_1to10 = "Oct11_2p2gl6mRecoord_1to10MeVallscint"

    out_dir = "Oct11_fitperftools_fv4_axistruth"

    #setup_tools_jobs("perf_e2p5MeV_SEVFVtrue", data_dir+out_dir, data_dir+true_sev_fv_2p5, env_file, submission_dir, "r")
    #setup_tools_jobs("perf_e2p5MeV_SEVFVtrue", data_dir+out_dir, data_dir+true_sev_fv_2p5, env_file, submission_dir, "z")
    #setup_tools_jobs("perf_e1to10MeV_SEVFVtrue", data_dir+out_dir, data_dir+true_sev_fv_1to10, env_file, submission_dir, "r")
    #setup_tools_jobs("perf_e1to10MeV_SEVFVtrue", data_dir+out_dir, data_dir+true_sev_fv_1to10, env_file, submission_dir, "z")
    setup_tools_jobs("perf_e1to10MeV_SEVFVtrue", data_dir+out_dir, data_dir+true_sev_fv_1to10, env_file, submission_dir, "e")

    #setup_tools_jobs("perf_e2p5MeV_SEVFVreco", data_dir+out_dir, data_dir+reco_sev_fv_2p5, env_file, submission_dir, "r")
    #setup_tools_jobs("perf_e2p5MeV_SEVFVreco", data_dir+out_dir, data_dir+reco_sev_fv_2p5, env_file, submission_dir, "z")
    #setup_tools_jobs("perf_e1to10MeV_SEVFVreco", data_dir+out_dir, data_dir+reco_sev_fv_1to10, env_file, submission_dir, "r")
    #setup_tools_jobs("perf_e1to10MeV_SEVFVreco", data_dir+out_dir, data_dir+reco_sev_fv_1to10, env_file, submission_dir, "z")
    setup_tools_jobs("perf_e1to10MeV_SEVFVreco", data_dir+out_dir, data_dir+reco_sev_fv_1to10, env_file, submission_dir, "e")
