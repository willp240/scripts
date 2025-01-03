import glob
import os
import string

count=0
command_string=""
job_count=0

os.chdir("/data/snoplus3/griddata/Prod_7_0_8_9_Preliminary_Scintillator_Gold/ntuples/")
for filepath in glob.iglob('ScintFit_2p2ReactoribdRun*'):
    count += 1
    command_string+= "/home/parkerw/Software/rat-tools_fresh/AntinuTools/scale_reactor_flux.exe " + filepath + " 1 1 0 crust liveDB \n"
    if count % 500 == 0 or count == 2277:
        print command_string
        
        base_filename = "/home/parkerw/Software/rat_submission/condor_base_scaleIBD.mac"
        in_macro_file = open(base_filename, "r")
        raw_text = string.Template(in_macro_file.read())
        in_macro_file.close()
        
        out_macro_text = raw_text.substitute(command=command_string)

        sh_filename = "/home/parkerw/Software/rat_submission/condor_base_scaleIBD_" + str(job_count) + ".mac"
        sh_file = open(sh_filename, "w")
        sh_file.write(out_macro_text)
        sh_file.close()
        os.chmod(sh_filename, 0o777)
        
        job_batch = "scale_" + str(job_count)
        submit_filepath = "/home/parkerw/Software/rat_submission/" + job_batch + ".submit"
        out_submit_text = "executable              = "+str(sh_filename)+"\n"+\
                          "universe                =  vanilla\n"+\
                          "output                  =  "+str(job_batch)+".output\n"+\
                          "error                   =  "+str(job_batch)+".error\n"+\
                          "log                     =  "+str(job_batch)+".log\n"+\
                          "notification            =  never\n"+\
                          "priority                =  1\n"+\
                          "getenv                  =  False\n"+\
                          "queue 1\n"

        out_submit_file = open(submit_filepath, "w")
        out_submit_file.write(out_submit_text)
        out_submit_file.close()

        command = 'condor_submit -batch-name \"'+job_batch+'\" '+submit_filepath
        print "executing job: "+command
        os.system(command)

        print "\n\n"
        command_string = ""
        job_count += 1
