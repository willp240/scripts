import os
import argparse
import configparser

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


def pycondor_submit(job_name, exec_name, out_dir, run_dir, env_file, fit_config, event_config, pdf_config, syst_config, osc_config, sleep_time = 1, priority = 5):
    '''
    submit a job to condor, write a sh file to source environment and execute command
    then write a submit file to be run by condor_submit
    '''

    print (job_name)
    index = ""
    batch_name = job_name

    # Set a condor path to be called later
    condor_path = "{0}/".format(out_dir)
    exec_path = run_dir + "/bin/" + exec_name

    configs_path = os.path.abspath('{0}/cfg'.format(condor_path))
    check_dir(configs_path)

    if fit_config != "":
        os.system("cp " + str(fit_config) + " " + str(configs_path) + "/" + os.path.basename(fit_config) )
        fit_config = configs_path + "/" + fit_config.split("/")[-1]
    if event_config != "":
        os.system("cp " + str(event_config) + " " + str(configs_path) + "/" + os.path.basename(event_config) )
        event_config = configs_path + "/" + event_config.split("/")[-1]
    if pdf_config != "":
        os.system("cp " + str(pdf_config) + " " + str(configs_path) + "/" + os.path.basename(pdf_config) )
        pdf_config = configs_path + "/" + pdf_config.split("/")[-1]
    if syst_config != "":
        os.system("cp " + str(syst_config) + " " + str(configs_path) + "/" + os.path.basename(syst_config) )
        syst_config = configs_path + "/" + syst_config.split("/")[-1]
    if osc_config != "":
        os.system("cp " + str(osc_config) + " " + str(configs_path) + "/" + os.path.basename(osc_config) )
        osc_config = configs_path + "/" + osc_config.split("/")[-1]

    other_commands = 'sleep $[($RANDOM%' + str(sleep_time+1) + ')+1]s'

    # Read the file
    with open(fit_config, "r") as file:
        lines = file.readlines()

    for iline, line in enumerate(lines):
        # Update output_directory
        if line.startswith("output_directory ="):
            lines[iline] = f"output_directory = {out_dir}\n"

    # Write the updated lines back to the file
    with open(fit_config, "w") as file:
        file.writelines(lines)
        out_macro_text = "#!/usr/bin/sh\n" + \
                 "set -eo pipefail\n" + \
                 "echo '=== Setup ==='\n" + \
                 "echo HOME=$HOME\n" + \
                 "date\n" + \
                 "source " + env_file + "\n" + \
                 "source " + run_dir + "/env.sh\n" + \
                 "\n" + \
                 "echo '=== Host info ==='\n" + \
                 "hostname\n" + \
                 "uname -a\n" + \
                 "echo\n" + \
                 "echo '=== CPU info ==='\n" + \
                 "if command -v lscpu >/dev/null 2>&1; then lscpu | egrep 'Model name|Socket|Thread|CPU\\(s\\)|MHz'; fi\n" + \
                 "grep -m1 'model name' /proc/cpuinfo || true\n" + \
                 "\n" + \
                 "echo '=== Memory info ==='\n" + \
                 "free -h || true\n" + \
                 "grep MemTotal /proc/meminfo || true\n" + \
                 "\n" + \
                 "echo '=== HTCondor ClassAds (from sandbox files) ==='\n" + \
                 "if [ -n \"${_CONDOR_MACHINE_AD-}\" ] && [ -f \"$_CONDOR_MACHINE_AD\" ]; then\n" + \
                 "  echo '--- MACHINE ClassAd ($_CONDOR_MACHINE_AD) ---'\n" + \
                 "  cat \"$_CONDOR_MACHINE_AD\"\n" + \
                 "fi\n" + \
                 "if [ -n \"${_CONDOR_JOB_AD-}\" ] && [ -f \"$_CONDOR_JOB_AD\" ]; then\n" + \
                 "  echo '--- JOB ClassAd ($_CONDOR_JOB_AD) ---'\n" + \
                 "  cat \"$_CONDOR_JOB_AD\"\n" + \
                 "fi\n" + \
                 "\n" + \
                 "echo '=== HTCondor ClassAds (via condor_status, if available) ==='\n" + \
                 "if command -v condor_status >/dev/null 2>&1; then\n" + \
                 "  echo '--- condor_status -l $(hostname) ---'\n" + \
                 "  condor_status -l $(hostname) || true\n" + \
                 "fi\n" + \
                 "\n" + \
                 "echo '=== Quick CPU probe (sanity check) ==='\n" + \
                 "python3 - <<'EOF'\n" + \
                 "import time, math\n" + \
                 "N = 10_000_000\n" + \
                 "t0 = time.time(); s = 0.0\n" + \
                 "for i in range(N): s += math.sin(i)\n" + \
                 "print('Python loop benchmark:', round(time.time()-t0, 3), 's; checksum=', round(s, 3))\n" + \
                 "EOF\n" + \
                 "\n" + \
                 "echo '=== Run LLH one-eval timing ==='\n" + \
                 "cd " + str(run_dir) + "\n" + \
                 str(other_commands) + "\n" + \
                 str(exec_path) + " " + str(fit_config) + " " + str(event_config) + " " + str(pdf_config) + " " + str(syst_config) + " " + str(osc_config) + " " + index + "\n" + \
                 "echo '=== Done ==='\n"

    sh_filepath = "{0}/sh/".format(condor_path) + str(job_name).replace("/", "") + '.sh'
    if not os.path.exists(os.path.dirname(sh_filepath)):
        os.makedirs(os.path.dirname(sh_filepath))
    sh_file = open(sh_filepath, "w")
    sh_file.write(out_macro_text)
    sh_file.close()
    os.chmod(sh_filepath, 0o777)
    
    # Now create submit file
    error_path = os.path.abspath('{0}/error'.format(condor_path))
    output_path = os.path.abspath('{0}/output'.format(condor_path))
    log_path = os.path.abspath('{0}/log'.format(condor_path))
    submit_path = os.path.abspath('{0}/submit'.format(condor_path))

    universe = "vanilla"
    notification = "never"
    n_rep = 1
    getenv = "True"

    submit_filepath = os.path.join(submit_path, job_name)
    submit_filepath += ".submit"
    out_submit_text = "executable              = " + str(sh_filepath) + "\n" + \
                     "universe                 = " + str(universe) + "\n" + \
                     "output                   = " + str(output_path) + "/" + str(job_name) + ".output\n" + \
                     "error                    = " + str(error_path) + "/" + str(job_name) + ".error\n" + \
                     "log                      = " + str(log_path) + "/" + str(job_name) + ".log\n" + \
                     "notification             = " + str(notification) + "\n" + \
                     "priority                 = " + str(priority) + "\n" + \
                     "getenv                   = " + str(getenv) + "\n" + \
                     "requirements             = (TARGET.CpuFamily == 25)" + "\n" + \
                     "queue "+str(n_rep)+"\n"

    # Check and create output path
    if not os.path.exists(os.path.dirname(submit_filepath)):
        os.makedirs(os.path.dirname(submit_filepath))
    out_submit_file = open(submit_filepath, "w")
    out_submit_file.write(out_submit_text)
    out_submit_file.close()

    # Lez do dis
    command = 'condor_submit -batch-name \"' + batch_name +'\" ' + submit_filepath
    print ("executing job: " + command)
    os.system(command)


if __name__ == "__main__":

    parser = argparse.ArgumentParser("Submit jobs to condor")
    parser.add_argument('exe', type=str, help='executable')
    parser.add_argument('out_dir', type=str, help='output directory')
    parser.add_argument('-r', "--run_dir", type=str, default=".", help='directory to run executable from')
    parser.add_argument('-e', "--env_file", type=str, default="", help='environment variable file to source')
    parser.add_argument('-f', "--fit_cfg", type=str, default="", help='fit config path')
    parser.add_argument('-i', "--event_cfg", type=str, default="", help='event config path')
    parser.add_argument('-p', "--pdf_cfg", type=str, default="", help='pdf config path')
    parser.add_argument('-s', "--syst_cfg", type=str, default="", help='syst config path')
    parser.add_argument('-o', "--osc_cfg", type=str, default="", help='osc grid config path')
    parser.add_argument("-n", "--num_jobs", type=int, default=1, help="how many identical jobs would you like to run?")
    parser.add_argument("-j", "--job_name", type=str, default="", help='job name')
    args = parser.parse_args()

    # Check if output and condor directories exist, create if they don't
    exec_name = args.exe
    out_dir = check_dir(args.out_dir)
    base_name = out_dir.split("/")[-2]
    run_dir = args.run_dir
    env_file = args.env_file
    fit_config = args.fit_cfg
    event_config = args.event_cfg
    pdf_config = args.pdf_cfg
    syst_config = args.syst_cfg
    osc_config = args.osc_cfg

    job_name = args.job_name
    if fit_config != "":
        fit_config = run_dir + "/" + args.fit_cfg
    if event_config != "":
        event_config = run_dir + "/" + args.event_cfg
    if pdf_config != "":
        pdf_config = run_dir + "/" + args.pdf_cfg
    if syst_config != "":
        syst_config = run_dir + "/" + args.syst_cfg
    if osc_config != "":
        osc_config = run_dir + "/" + args.osc_cfg

    if job_name == "":
        job_name = base_name

 
    # Otherwise do N jobs
    for i in range(args.num_jobs):

        job_name = base_name + "_{0}".format(i)

        log_dir = check_dir("{0}/log/".format(out_dir))
        error_dir = check_dir("{0}/error/".format(out_dir))
        sh_dir = check_dir("{0}/sh/".format(out_dir))
        submit_dir = check_dir("{0}/submit/".format(out_dir))
        output_dir = check_dir("{0}/output/".format(out_dir))

        pycondor_submit(job_name, exec_name, out_dir, run_dir, env_file, fit_config, event_config, pdf_config, syst_config, osc_config, sleep_time = 1, priority = 5)
