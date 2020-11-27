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

def saveTree(fname, outfile):

    '''
    Plot the fitted positiions in x y and z

    '''
    fname = "/data/snoplus/parkerw/ratSimulations/batch/Nov25_RefactoredPDF_Fit/multiPDF_*.root"

    outFile = ROOT.TFile(outfile, "RECREATE")
    use_retriggers = 0
    legend_coords = (0.7, 0.65, 0.9, 0.9)

    xTrue=[]
    yTrue=[]
    zTrue=[]
    xFit=[]
    yFit=[]
    zFit=[]
    xBias=[]
    yBias=[]
    zBias=[]
    eveNum=[]
    scaledLLH=[]
    LLH=[]

    posTree=ROOT.TTree("eveTree","eveTree")
    xTrue = np.empty((1), dtype="float32")
    posTree.Branch("xTrue", xTrue, "xTrue/F")
    yTrue = np.empty((1), dtype="float32")
    posTree.Branch("yTrue", yTrue, "yTrue/F")
    zTrue = np.empty((1), dtype="float32")
    posTree.Branch("zTrue", zTrue, "zTrue/F")

    xFit = np.empty((1), dtype="float32")
    posTree.Branch("xFit", xFit, "xFit/F")
    yFit = np.empty((1), dtype="float32")
    posTree.Branch("yFit", yFit, "yFit/F")
    zFit = np.empty((1), dtype="float32")
    posTree.Branch("zFit", zFit, "zFit/F")

    xBias = np.empty((1), dtype="float32")
    posTree.Branch("xBias", xBias, "xBias/F")
    yBias = np.empty((1), dtype="float32")
    posTree.Branch("yBias", yBias, "yBias/F")
    zBias = np.empty((1), dtype="float32")
    posTree.Branch("zBias", zBias, "zBias/F")

    scaledLLH = np.empty((1), dtype="float32")
    posTree.Branch("scaledLLH", scaledLLH, "scaledLLH/F")
    LLH = np.empty((1), dtype="float32")
    posTree.Branch("LLH", LLH, "LLH/F")

    eveNum = np.empty((1), dtype="int32")
    posTree.Branch("eveNum", eveNum, "eveNum/I")
       
    simCounter, evCounter = 0, 0
    for ds, run in rat.dsreader(fname):
        if simCounter == 0:
            loopStart=timer()
            print("Beginning event loop...")
        simCounter += 1
            
        if simCounter % 100 == 0:
            print("event ", simCounter)
   #     if simCounter > 100:
   #         print "simCounter >100"
   #         break
        
        for iev in range(0,ds.GetEVCount()):

            # Use retriggers?
            if use_retriggers == 0 and iev > 0:
                continue

            # Increment counter
            evCounter += 1
            
            # Get DS variables
            ev = ds.GetEV(iev)
            mc = ds.GetMC()

            if mc.GetMCParticle(0).GetPosition().Z() > 6000:
                continue

            xTrue[0] = mc.GetMCParticle(0).GetPosition().X()
            yTrue[0] = mc.GetMCParticle(0).GetPosition().Y()
            zTrue[0] = mc.GetMCParticle(0).GetPosition().Z()
            eveNum[0] = simCounter
                    
            # Get fitter vertex
            try:
                fitResult = ev.GetFitResult("partialFitter")
                fitVertex = fitResult.GetVertex(0)
                    
            except Exception as e:
                msg = "No {0} for event {1:d}, GTID {2:d} : {3}"
                msg = msg.format("partialFitter", evCounter, ev.GetGTID(), e)
                print(msg)
                continue

            if (not fitVertex.ValidPosition()
                or (not fitVertex.ContainsTime())
                or (not fitVertex.ValidTime())):
                print("not valid")
                continue

            xFit[0] = fitVertex.GetPosition().X()
            yFit[0] = fitVertex.GetPosition().Y()
            zFit[0] = fitVertex.GetPosition().Z()
            
            xBias[0] = xFit[0] - xTrue[0]
            yBias[0] = yFit[0] - yTrue[0]
            zBias[0] = zFit[0] - zTrue[0]
            
            #LLH[0] = fitResult.GetFOM("PositionPositionLogL")
            #scaledLLH[0] = LLH[0]/fitResult.GetFOM("PositionPositionSelectedNHit")

            LLH[0] = fitResult.GetFOM("Positionmultipath_scintwater")
            scaledLLH[0] = LLH[0]/fitResult.GetFOM("Positionmultipath_SelectedNHit_scintwater")
            
            posTree.Fill()

    posTree.Write()

if __name__ == "__main__":
    parser = argparse.ArgumentParser() #formatter_class=argparse.ArgumentDefaultsHelpFormatter

    parser.add_argument('infile', type=str,
                        help="File(s) to be read in. Will accept wildcard (must be in quotes though!)")

    parser.add_argument('-o', '--outfile', type=str, default="nov26multipath_plots.root",
                        help="Path to output file. (Default: %(default)s)")

    args = parser.parse_args()
    start = timer()
        
    # ROOT stuff
    ROOT.gROOT.SetBatch(True)

    # Check if path is absolute or relative directory.
    # If it is neither, use the directory of this script.
    # Makes it easier to run the tool from a different location.
    if not os.path.isabs(args.outfile) and not os.path.dirname(args.outfile):
        directory = os.path.dirname(sys.argv[0])
        args.outfile = os.path.join(directory, args.outfile)

    saveTree(args.infile, args.outfile);
    
    mm, ss = divmod(timer() - start, 60)
    print("######################################")
    print("Full script took {:02d}:{:02d}.".format(int(mm), int(ss)))

