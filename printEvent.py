#!/usr/bin/env python
'''
A python tool for plotting fitted positions

Author: Big Bad Bill

'''
import ROOT
# Stop ROOT stealing our useage messages
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gErrorIgnoreLevel = ROOT.kWarning

try:
    import numpy as np
except:
    print("Failed to import numpy.")
    exit()

import rat

from timeit import default_timer as timer
import argparse
import os
import sys

def saveTree(fname):

    '''
    Print fitted positions

    '''
    use_retriggers = 0
    
    simCounter, evCounter = 0, 0
    for ds, run in rat.dsreader(fname):

        if simCounter > 0:
            continue

        simCounter += 1
                
        for iev in range(0,1):
                        
            # Use retriggers?
            if use_retriggers == 0 and iev > 0:
                continue
                
            # Increment counter
            evCounter += 1
            
            # Get DS variables
            ev = ds.GetEV(iev)
            mc = ds.GetMC()
            
            xTrue = mc.GetMCParticle(0).GetPosition().X()
            yTrue = mc.GetMCParticle(0).GetPosition().Y()
            zTrue = mc.GetMCParticle(0).GetPosition().Z()
                      
            # Get fitter vertex
            try:
                fitResult = ev.GetFitResult("MultiPDFFitter")
                fitVertex = fitResult.GetVertex(0)
                                    
            except Exception as e:
                msg = "No {0} for event {1:d}, GTID {2:d} : {3}"
                msg = msg.format("MultiPDFFitter", evCounter, ev.GetGTID(), e)
                print(msg)
                continue
                
            xFit = fitVertex.GetPosition().X()
            yFit = fitVertex.GetPosition().Y()
            zFit = fitVertex.GetPosition().Z()
            
            print ("True (x y z) = (", xTrue, yTrue, zTrue )
            print ("Fit  (x y z) = (", xFit, yFit, zFit )


if __name__ == "__main__":
    parser = argparse.ArgumentParser() #formatter_class=argparse.ArgumentDefaultsHelpFormatter

    args = parser.parse_args()
    start = timer()
        
    # ROOT stuff
    ROOT.gROOT.SetBatch(True)


    fname1 = "/data/snoplus/parkerw/ratSimulations/batch/Jan27_MultiPDF_LPCSF/input_0.root"
    fname2 = "/data/snoplus/parkerw/ratSimulations/batch/Jan25_MultiPDF_test2/input_0.root"
    fname3 = "/data/snoplus/parkerw/ratSimulations/batch/Jan25_MultiPDF_test2/input_0.root"

    print(fname1)
    saveTree(fname1);
    
    mm, ss = divmod(timer() - start, 60)
    print("######################################")
    print("Full script took {:02d}:{:02d}.".format(int(mm), int(ss)))

