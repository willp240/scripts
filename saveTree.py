#!/usr/bin/env python
'''
A python tool for plotting fitted positions

Author: Big Bad Bill

'''
import ROOT
#Stop ROOT stealing our useage messages
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
    fname = "/home/parkerw/Software/rat-tools_master/FitPerformance/scintFit_185.root"
    #fname = "/data/snoplus/parkerw/ratSimulations/batch/Sep27_recoordScintEffVelMPDF_Perf_ds/*.root"
    #fname = "/data/snoplus/parkerw/ratSimulations/batch/Sep27_SES_fill/scintEff_0.root"

    #    fname = "/home/parkerw/Software/rat_b/testpartialmpdf.root"
    outFile = ROOT.TFile(outfile, "RECREATE")
    use_retriggers = 0
    use_dir = 0
    use_energy = 0
    legend_coords = (0.7, 0.65, 0.9, 0.9)
    
    xTrue=[]
    yTrue=[]
    zTrue=[]
    tiTrue=[]
    xFit=[]
    yFit=[]
    zFit=[]
    tiFit=[]
    xBias=[]
    yBias=[]
    zBias=[]
    eveNum=[]
    scaledLLH=[]
    LLH=[]
    numScint=[]
    numCerenk=[]
    
    posTree=ROOT.TTree("eveTree","eveTree")
    xTrue = np.empty((1), dtype="float32")
    posTree.Branch("xTrue", xTrue, "xTrue/F")
    yTrue = np.empty((1), dtype="float32")
    posTree.Branch("yTrue", yTrue, "yTrue/F")
    zTrue = np.empty((1), dtype="float32")
    posTree.Branch("zTrue", zTrue, "zTrue/F")
    tiTrue = np.empty((1), dtype="float32")
    posTree.Branch("tiTrue", tiTrue, "tiTrue/F")
    ETrue = np.empty((1), dtype="float32")
    posTree.Branch("ETrue", ETrue, "ETrue/F")    

    xFit = np.empty((1), dtype="float32")
    posTree.Branch("xFit", xFit, "xFit/F")
    yFit = np.empty((1), dtype="float32")
    posTree.Branch("yFit", yFit, "yFit/F")
    zFit = np.empty((1), dtype="float32")
    posTree.Branch("zFit", zFit, "zFit/F")
    tiFit = np.empty((1), dtype="float32")
    posTree.Branch("tiFit", tiFit, "tiFit/F")

    xBias = np.empty((1), dtype="float32")
    posTree.Branch("xBias", xBias, "xBias/F")
    yBias = np.empty((1), dtype="float32")
    posTree.Branch("yBias", yBias, "yBias/F")
    zBias = np.empty((1), dtype="float32")
    posTree.Branch("zBias", zBias, "zBias/F")
    
    xdirTrue = np.empty((1), dtype="float32")
    posTree.Branch("xdirTrue", xdirTrue, "xdirTrue/F")
    ydirTrue = np.empty((1), dtype="float32")
    posTree.Branch("ydirTrue", ydirTrue, "ydirTrue/F")
    zdirTrue = np.empty((1), dtype="float32")
    posTree.Branch("zdirTrue", zdirTrue, "zdirTrue/F")
    tTrue = np.empty((1), dtype="float32")
    posTree.Branch("tTrue", tTrue, "tTrue/F")
    phiTrue = np.empty((1), dtype="float32")
    posTree.Branch("phiTrue", phiTrue, "phiTrue/F")
    
    if use_energy:
        EFit = np.empty((1), dtype="float32")
        posTree.Branch("EFit", EFit, "EFit/F")

    if use_dir:
        xdirFit = np.empty((1), dtype="float32")
        posTree.Branch("xdirFit", xdirFit, "xdirFit/F")
        ydirFit = np.empty((1), dtype="float32")
        posTree.Branch("ydirFit", ydirFit, "ydirFit/F")
        zdirFit = np.empty((1), dtype="float32")
        posTree.Branch("zdirFit", zdirFit, "zdirFit/F")
        tFit = np.empty((1), dtype="float32")
        posTree.Branch("tFit", tFit, "tFit/F")
        phiFit = np.empty((1), dtype="float32")
        posTree.Branch("phiFit", phiFit, "phiFit/F")


    scaledLLH = np.empty((1), dtype="float32")
    posTree.Branch("scaledLLH", scaledLLH, "scaledLLH/F")
    LLH = np.empty((1), dtype="float32")
    posTree.Branch("LLH", LLH, "LLH/F")
    
    eveNum = np.empty((1), dtype="int32")
    posTree.Branch("eveNum", eveNum, "eveNum/I")
    
    numScint = np.empty((1), dtype="int32")
    posTree.Branch("numScint", numScint, "numScint/I")
    numCerenk = np.empty((1), dtype="int32")
    posTree.Branch("numCerenk", numCerenk, "numCerenk/I")

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
            mceve = ds.GetMCEV(iev)
            
            #if mc.GetMCParticle(0).GetPosition().Z() > 6000:
            #    print("bad z")
            #    continue
                                
            xTrue[0] = mc.GetMCParticle(0).GetPosition().X()
            yTrue[0] = mc.GetMCParticle(0).GetPosition().Y()
            zTrue[0] = mc.GetMCParticle(0).GetPosition().Z()
            tiTrue[0] = 390 - mceve.GetGTTime()
            ETrue[0] =  mc.GetMCParticle(0).GetKineticEnergy()

            xdirTrue[0] = mc.GetMCParticle(0).GetMomentum().X()
            ydirTrue[0] = mc.GetMCParticle(0).GetMomentum().Y()
            zdirTrue[0] = mc.GetMCParticle(0).GetMomentum().Z()
            tTrue[0] = mc.GetMCParticle(0).GetMomentum().Theta()
            phiTrue[0] = mc.GetMCParticle(0).GetMomentum().Phi()

            eveNum[0] = simCounter

            # Get fitter vertex
            try:
                #fitResult = ev.GetFitResult("direction2DFit")
                #fitResult = ev.GetFitResult("scintFitter")
                #fitResult = ev.GetFitResult("multiPDFFit")
                fitResult = ev.GetFitResult("positionTimeFit")
                #fitResult = ev.GetFitResult("diPoFit")
                fitVertex = fitResult.GetVertex(0)
                                    
            except Exception as e:
                msg = "No {0} for event {1:d}, GTID {2:d} : {3}"
                msg = msg.format("MultiPDFFitter", evCounter, ev.GetGTID(), e)
                print(msg)
                continue
                
            if not fitVertex.ValidPosition():
                #print ("not valid pos ",fitVertex.GetPosition().X(), " ", fitVertex.GetPosition().Y(), " ", fitVertex.GetPosition().Z())
                continue
            if not fitVertex.ContainsTime():
                print ("not contains time")
                continue
            if not fitVertex.ValidTime():
                print("not valid")
                continue

            if(fitVertex.GetPosition().Z() > 7000):
                print("valid z ", evCounter, " " , fitVertex.GetPosition().Z())
                print("true ", mc.GetMCParticle(0).GetPosition().X(), mc.GetMCParticle(0).GetPosition().Y(), mc.GetMCParticle(0).GetPosition().Z())

            xFit[0] = fitVertex.GetPosition().X()
            yFit[0] = fitVertex.GetPosition().Y()
            zFit[0] = fitVertex.GetPosition().Z()
            tiFit[0] = fitVertex.GetTime()

            if use_dir:
                xdirFit[0] = fitVertex.GetDirection().X()
                ydirFit[0] = fitVertex.GetDirection().Y()
                zdirFit[0] = fitVertex.GetDirection().Z()
                tFit[0] = fitVertex.GetDirection().Theta()
                phiFit[0] = fitVertex.GetDirection().Phi()

            if use_energy:
                EFit[0] = fitVertex.GetEnergy()

            xBias[0] = xFit[0] - xTrue[0]
            yBias[0] = yFit[0] - yTrue[0]
            zBias[0] = zFit[0] - zTrue[0]

            #print(xFit, phiFit, tiFit)
            #print(xFit, yFit, zFit, tiFit, tFit, phiFit, tiTrue)
            fomnames = fitResult.GetFOMNames()
            #for i_names in range(fomnames.size()):
            #    print(fomnames.at(i_names))
            
            #LLH[0] = fitResult.GetFOM("PositionPositionLogL")
            #scaledLLH[0] = LLH[0]/fitResult.GetFOM("PositionPositionSelectedNHit")

            #scintfitter
            #LLH[0] = fitResult.GetFOM("PositionLogL")
            #scaledLLH[0] = LLH[0]/fitResult.GetFOM("PositionSelectedNHit")
            
            #LLH[0] = fitResult.GetFOM("multipath_scint")
            #scaledLLH[0] = LLH[0]/fitResult.GetFOM("multipath_SelectedNHit_scint")

            #dipo
            #LLH[0] = fitResult.GetFOM("LogL")
            #scaledLLH[0] = LLH[0]/fitResult.GetFOM("SelectedNHit")
            
            numScint[0] = mc.GetNScintPhotons()
            numCerenk[0] = mc.GetNCherPhotons()

            posTree.Fill()

    posTree.Write()

    print("Written to ", outfile)

if __name__ == "__main__":
    parser = argparse.ArgumentParser() #formatter_class=argparse.ArgumentDefaultsHelpFormatter

    parser.add_argument('infile', type=str,
                        help="File(s) to be read in. Will accept wildcard (must be in quotes though!)")

    parser.add_argument('-o', '--outfile', type=str, default="testo.root",
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

