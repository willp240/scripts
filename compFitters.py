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

def compFitters(fname, outfile):

    '''
    Plot the fitted positiions in x y and z

    '''
    #fname = "/data/snoplus/parkerw/ratSimulations/batch/Oct11MPDFvsSF/mpdf_0.root"

    outFile = ROOT.TFile(outfile, "RECREATE")
    use_retriggers = 0
    use_energy = 0
    legend_coords = (0.7, 0.65, 0.9, 0.9)
    
    xTrue=[]
    yTrue=[]
    zTrue=[]
    tiTrue=[]
    xSF=[]
    ySF=[]
    zSF=[]
    tiSF=[]
    xMPDF=[]
    yMPDF=[]
    zMPDF=[]
    tiMPDF=[]
    eveNum=[]
    scaledLLHSF=[]
    LLHSF=[]
    scaledLLHMPDF=[]
    LLHMPDF=[]
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

    xSF = np.empty((1), dtype="float32")
    posTree.Branch("xSF", xSF, "xSF/F")
    ySF = np.empty((1), dtype="float32")
    posTree.Branch("ySF", ySF, "ySF/F")
    zSF = np.empty((1), dtype="float32")
    posTree.Branch("zSF", zSF, "zSF/F")
    tiSF = np.empty((1), dtype="float32")
    posTree.Branch("tiSF", tiSF, "tiSF/F")

    
    xMPDF = np.empty((1), dtype="float32")
    posTree.Branch("xMPDF", xMPDF, "xMPDF/F")
    yMPDF = np.empty((1), dtype="float32")
    posTree.Branch("yMPDF", yMPDF, "yMPDF/F")
    zMPDF = np.empty((1), dtype="float32")
    posTree.Branch("zMPDF", zMPDF, "zMPDF/F")
    tiMPDF = np.empty((1), dtype="float32")
    posTree.Branch("tiMPDF", tiMPDF, "tiMPDF/F")

    xMPDF2 = np.empty((1), dtype="float32")
    posTree.Branch("xMPDF2", xMPDF2, "xMPDF2/F")
    yMPDF2 = np.empty((1), dtype="float32")
    posTree.Branch("yMPDF2", yMPDF2, "yMPDF2/F")
    zMPDF2 = np.empty((1), dtype="float32")
    posTree.Branch("zMPDF2", zMPDF2, "zMPDF2/F")
    tiMPDF2 = np.empty((1), dtype="float32")
    posTree.Branch("tiMPDF2", tiMPDF2, "tiMPDF2/F")
    
    if use_energy:
        ESF = np.empty((1), dtype="float32")
        posTree.Branch("ESF", ESF, "ESF/F")

    scaledLLHSF = np.empty((1), dtype="float32")
    posTree.Branch("scaledLLHSF", scaledLLHSF, "scaledLLHSF/F")
    LLHSF = np.empty((1), dtype="float32")
    posTree.Branch("LLHSF", LLHSF, "LLHSF/F")
    
    scaledLLHMPDF = np.empty((1), dtype="float32")
    posTree.Branch("scaledLLHMPDF", scaledLLHMPDF, "scaledLLHMPDF/F")
    LLHMPDF = np.empty((1), dtype="float32")
    posTree.Branch("LLHMPDF", LLHMPDF, "LLHMPDF/F")

    scaledLLHMPDF2 = np.empty((1), dtype="float32")
    posTree.Branch("scaledLLHMPDF2", scaledLLHMPDF2, "scaledLLHMPDF2/F")
    LLHMPDF2 = np.empty((1), dtype="float32")
    posTree.Branch("LLHMPDF2", LLHMPDF2, "LLHMPDF2/F")

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
                    
        for iev in range(0,ds.GetEVCount()):
                        
            # Use retriggers?
            if use_retriggers == 0 and iev > 0:
                continue
                
            # Increment counter
            evCounter += 1
            
            # Get DS variables
            ev = ds.GetEV(iev)
            #mc = ds.GetMC()
            #mceve = ds.GetMCEV(iev)
                                            
            #xTrue[0] = mc.GetMCParticle(0).GetPosition().X()
            #yTrue[0] = mc.GetMCParticle(0).GetPosition().Y()
            #zTrue[0] = mc.GetMCParticle(0).GetPosition().Z()
            #tiTrue[0] = 390 - mceve.GetGTTime()
            #ETrue[0] =  mc.GetMCParticle(0).GetKineticEnergy()

            eveNum[0] = simCounter

            # Get fitter vertex
            try:
                SFResult = ev.GetFitResult("scintFitter")
                SFVertex = SFResult.GetVertex(0)
                MPDFResult = ev.GetFitResult("multiPDFFit")
                MPDFVertex = MPDFResult.GetVertex(0)
                MPDF2Result = ev.GetFitResult("multiPDFFit")
                MPDF2Vertex = MPDF2Result.GetVertex(0)

            except Exception as e:
                msg = "No {0} for event {1:d}, GTID {2:d} : {3}"
                msg = msg.format("fitter", evCounter, ev.GetGTID(), e)
                print(msg)
                continue
                
            if not SFVertex.ValidPosition():
                print ("not valid SF pos ", SFVertex.GetPosition().X(), " ", SFVertex.GetPosition().Y(), " ", SFVertex.GetPosition().Z())
                continue
            if not SFVertex.ContainsTime():
                print ("not contains SF time")
                continue
            if not SFVertex.ValidTime():
                print("not valid SF time")
                continue

            if not MPDFVertex.ValidPosition():
                print ("not valid MPDF pos ", MPDFVertex.GetPosition().X(), " ", MPDFVertex.GetPosition().Y(), " ", MPDFVertex.GetPosition().Z())
                continue
            if not MPDFVertex.ContainsTime():
                print ("not contains MPDF time")
                continue
            if not MPDFVertex.ValidTime():
                print("not valid MPDF time")
                continue

            if not MPDF2Vertex.ValidPosition():
                print ("not valid MPDF2 pos ", MPDF2Vertex.GetPosition().X(), " ", MPDF2Vertex.GetPosition().Y(), " ", MPDF2Vertex.GetPosition().Z())
                continue
            if not MPDF2Vertex.ContainsTime():
                print ("not contains MPDF2 time")
                continue
            if not MPDF2Vertex.ValidTime():
                print("not valid MPDF2 time")
                continue

            if(SFVertex.GetPosition().Z() > 7000):
                print("valid z SF ", evCounter, " " , SFVertex.GetPosition().Z())
                #print("true ", mc.GetMCParticle(0).GetPosition().X(), mc.GetMCParticle(0).GetPosition().Y(), mc.GetMCParticle(0).GetPosition().Z())

            if(MPDFVertex.GetPosition().Z() > 7000):
                print("valid z MPDF ", evCounter, " " , MPDFVertex.GetPosition().Z())
                #print("true ", mc.GetMCParticle(0).GetPosition().X(), mc.GetMCParticle(0).GetPosition().Y(), mc.GetMCParticle(0).GetPosition().Z())

            if(MPDF2Vertex.GetPosition().Z() > 7000):
                print("valid z MPDF2 ", evCounter, " " , MPDF2Vertex.GetPosition().Z())
                #print("true ", mc.GetMCParticle(0).GetPosition().X(), mc.GetMCParticle(0).GetPosition().Y(), mc.GetMCParticle(0).GetPosition().Z())

            xSF[0] = SFVertex.GetPosition().X()
            ySF[0] = SFVertex.GetPosition().Y()
            zSF[0] = SFVertex.GetPosition().Z()
            tiSF[0] = SFVertex.GetTime()

            xMPDF[0] = MPDFVertex.GetPosition().X()
            yMPDF[0] = MPDFVertex.GetPosition().Y()
            zMPDF[0] = MPDFVertex.GetPosition().Z()
            tiMPDF[0] = MPDFVertex.GetTime()

            xMPDF2[0] = MPDF2Vertex.GetPosition().X()
            yMPDF2[0] = MPDF2Vertex.GetPosition().Y()
            zMPDF2[0] = MPDF2Vertex.GetPosition().Z()
            tiMPDF2[0] = MPDF2Vertex.GetTime()

            if use_energy:
                ESF[0] = SFVertex.GetEnergy()

            #scintfitter
            LLHSF[0] = SFResult.GetFOM("PositionLogL")
            scaledLLHSF[0] = LLHSF[0]/SFResult.GetFOM("PositionSelectedNHit")

            #mpdffitter
            LLHMPDF[0] = MPDFResult.GetFOM("LogL")
            scaledLLHMPDF[0] = LLHMPDF[0]/MPDFResult.GetFOM("SelectedNHit")

            #mpdf2fitter
            LLHMPDF2[0] = MPDF2Result.GetFOM("LogL")
            scaledLLHMPDF2[0] = LLHMPDF2[0]/MPDF2Result.GetFOM("SelectedNHit")

            posTree.Fill()

    posTree.Write()

    print("Written to ", outfile)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument('infile', type=str,
                        help="File(s) to be read in. Will accept wildcard (must be in quotes though!)")

    parser.add_argument('outfile', type=str, default="testo.root",
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

    compFitters(args.infile, args.outfile);
    
    mm, ss = divmod(timer() - start, 60)
    print("######################################")
    print("Full script took {:02d}:{:02d}.".format(int(mm), int(ss)))

