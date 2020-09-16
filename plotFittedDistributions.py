#!/usr/bin/env python
'''
A python tool for plotting fitted positions

Author: Big Bad Bill

'''
import ROOT
# Stop ROOT stealing our useage messages
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gErrorIgnoreLevel = ROOT.kWarning

import rat

from timeit import default_timer as timer
import argparse
import os
import sys

def plot_fitted_distributions(fname, outfile):

    '''
    Plot the fitted positiions in x y and z

    '''
    # Make a primary dimension tool to handle requests wrt the passed dimension
    # primaryDimTool = DimensionTool1D(config, dimension)
    axisMin, axisMax = -6000, 6000
    axisBinWidth = 800
    nbins = 30
    outFile = ROOT.TFile(outfile, "RECREATE")
    use_retriggers = 0
    legend_coords = (0.7, 0.65, 0.9, 0.9)

    fullFitterNames = ["", "partialTestFitter", "partialTestFitter_2ZSplit", "partialTestFitter_4ZSplit", "partialTestFitter_8PDFs"]
    abvFitterNames = ["True MC", "1 PDF", "2 PDFs", "4 PDFs", "8 PDFs"]

    xEvents=[]
    yEvents=[]
    zEvents=[]
    for i_fitter in range(len(fullFitterNames)):

        # Fit valid histograms
        xEvents.append(ROOT.TH1D("nEventsX_"+fullFitterNames[i_fitter], abvFitterNames[i_fitter]+" Fitted positions", nbins, axisMin, axisMax))
        xEvents[i_fitter].GetXaxis().SetTitle("x [mm]")

        yEvents.append(ROOT.TH1D("nEventsY_"+fullFitterNames[i_fitter], abvFitterNames[i_fitter]+" Fitted positions", nbins, axisMin, axisMax))
        yEvents[i_fitter].GetXaxis().SetTitle("y [mm]")

        zEvents.append(ROOT.TH1D("nEventsZ_"+fullFitterNames[i_fitter], abvFitterNames[i_fitter]+" Fitted positions", nbins, axisMin, axisMax))
        zEvents[i_fitter].GetXaxis().SetTitle("z [mm]")

    simCounter, evCounter = 0, 0
    for ds, run in rat.dsreader(fname):
        if simCounter == 0:
            loopStart=timer()
            print("Beginning event loop...")
            simCounter += 1
            
        for iev in range(0, ds.GetEVCount()):

            # Use retriggers?
            if use_retriggers == 0 and iev > 0:
                continue
                
            # Increment counter
            evCounter += 1
            
            # Get DS variables
            ev = ds.GetEV(iev)
            mc = ds.GetMC()
        
            for i_fitter in range(len(fullFitterNames)):
        
                if i_fitter == 0:
                    xEvents[i_fitter].Fill(mc.GetMCParticle(0).GetPosition().X())
                    yEvents[i_fitter].Fill(mc.GetMCParticle(0).GetPosition().Y())
                    zEvents[i_fitter].Fill(mc.GetMCParticle(0).GetPosition().Z())
                    continue
                # Get fitter vertex
                try:
                    fitResult = ev.GetFitResult(fullFitterNames[i_fitter])
                    fitVertex = fitResult.GetVertex(0)
                    
                except Exception as e:
                    msg = "No {0} for event {1:d}, GTID {2:d} : {3}"
                    msg = msg.format(fullFitterNames[i_fitter], evCounter, ev.GetGTID(), e)
                    print(msg)
                    continue

                if ((not fitVertex.ContainsPosition())
                    or (not fitVertex.ValidPosition())
                    or (not fitVertex.ContainsTime())
                    or (not fitVertex.ValidTime())
                    or (not fitVertex.ContainsEnergy())
                    or (not fitVertex.ValidEnergy())
                    or (not fitVertex.ContainsDirection() and fullFitterNames[i_fitter].lower()=='waterfitter')
                    or (not fitVertex.ValidDirection() and fullFitterNames[i_fitter].lower()=='waterfitter')):
                    continue

                xEvents[i_fitter].Fill(fitVertex.GetPosition().X())
                yEvents[i_fitter].Fill(fitVertex.GetPosition().Y())
                zEvents[i_fitter].Fill(fitVertex.GetPosition().Z())

    xCanv = ROOT.TCanvas("X","X")
    yCanv = ROOT.TCanvas("Y","Y")
    zCanv = ROOT.TCanvas("Z","Z")
    Leg = ROOT.TLegend(*legend_coords)

    for i_fitter in range(len(fullFitterNames)):
    
        Leg.AddEntry(xEvents[i_fitter], abvFitterNames[i_fitter])

        xCanv.cd()
        xEvents[i_fitter].SetLineColor(i_fitter)
        xEvents[i_fitter].GetYaxis().SetTitle("Fitted Events")
        if i_fitter == 0:
            xEvents[i_fitter].SetMarkerSize(2)
            xEvents[i_fitter].SetMarkerStyle(2)
            xEvents[i_fitter].Draw("p")
        else:
            xEvents[i_fitter].Draw("same")
        xEvents[i_fitter].Write()
        
        yCanv.cd()
        yEvents[i_fitter].SetLineColor(i_fitter)
        yEvents[i_fitter].GetYaxis().SetTitle("Fitted Events")
        if i_fitter == 0:
            yEvents[i_fitter].SetMarkerSize(2)
            yEvents[i_fitter].SetMarkerStyle(2)
            yEvents[i_fitter].Draw("p")
        else:
            yEvents[i_fitter].Draw("same")
        yEvents[i_fitter].Write()
       
        zCanv.cd()
        zEvents[i_fitter].SetLineColor(i_fitter)
        zEvents[i_fitter].GetYaxis().SetTitle("Fitted Events")
        if i_fitter == 0:
            zEvents[i_fitter].SetMarkerSize(2)
            zEvents[i_fitter].SetMarkerStyle(2)
            zEvents[i_fitter].Draw("p")
        else:
            zEvents[i_fitter].Draw("same")
        zEvents[i_fitter].Write()

    xCanv.cd()
    Leg.Draw()
    yCanv.cd()
    Leg.Draw() 
    zCanv.cd()
    Leg.Draw()

    xCanv.Write()
    yCanv.Write()
    zCanv.Write()


if __name__ == "__main__":
    parser = argparse.ArgumentParser() #formatter_class=argparse.ArgumentDefaultsHelpFormatter

    parser.add_argument('infile', type=str,
                        help="File(s) to be read in. Will accept wildcard (must be in quotes though!)")

    parser.add_argument('-o', '--outfile', type=str, default="1D_plots.root",
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

    plot_fitted_distributions(args.infile, args.outfile);
    
    mm, ss = divmod(timer() - start, 60)
    print("######################################")
    print("Full script took {:02d}:{:02d}.".format(int(mm), int(ss)))

