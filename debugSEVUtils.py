#!/usr/bin/env python
import string, ROOT, SEVUtilities, AnalyseData, os
ROOT.gROOT.SetBatch(True)

# Author I T Coulter - 09/11/2012 <icoulter@hep.upenn.edu>
#        K Majumdar - 11/09/2014 - Cleanup of Coordinators for new DS
#        W Parker - 18/04/2023 - Add nhit interpolation method


def AnalysisFunction(index, n_energies, inputFileDir, inputFileDirHighE, savePlotDir, overwriteTable, boundaries):

    bound_list = []

    for bound in boundaries.split(","):
        if bound.isdigit():
            bound_list.append(float(bound))

    for i_energy in range(int(n_energies)):

        #if i_energy == 0:
        #    input_file = inputFileDir
        #    en_suffix = "_lowE_"
        #else:
        #    input_file = inputFileDirHighE
        #    en_suffix = "_highE_"

        histograms = SEVUtilities.ProducePlots(0, "/data/snoplus3/parkerw/ratSimulations/Nov3_autorecoord_6mSEV//sev_recon_round1/", bound_list)

        print("1hists[0] type: >>>>>>>",type(histograms[0][0]))

        outfile = ROOT.TFile( "test.root", 'RECREATE' )
        outfile.cd()

        print("2hists[0] type: >>>>>>>",type(histograms[0][0]))

        histograms[0][0].Write()

        print("dilly done")

def makehistos():

    histolist1 = []
    histolist2 = []

    for i in range(5):
        histolist1.append(ROOT.TH2D("histolist1_"+str(i), "Histogram1", 2000, -1000.0, 1000.0, 150, 100, 250))
        histolist2.append(ROOT.TH1D("histolist2"+str(i), "Histogram2", 2000, -1000.0, 1000.0))

    return [histolist1, histolist2]

import optparse
if __name__ == '__main__':

    print("starting ok")

    #AnalysisFunction("labppo_2p2_scintillator", 2, "/data/snoplus3/parkerw/ratSimulations/Nov3_autorecoord_6mSEV//sev_recon_round1/", "/data/snoplus3/parkerw/ratSimulations/Nov3_autorecoord_6mSEV//sev_recon_high_e/", "./plots", False, "0,2000,3000,4000,5000,6000")

#    for i in range(2):

        #histograms = SEVUtilities.ProducePlots(0, "/data/snoplus3/parkerw/ratSimulations/Nov3_autorecoord_6mSEV//sev_recon_round1/", [0,2000,3000,4000,5000,6000])

    histograms = makehistos()

        #       print(i, type(histograms[0][0]))

    #outfile = ROOT.TFile( "./test.root", 'RECREATE' )
    #   outfile.cd()

    #    print(i, type(histograms[0][0]))

    #    histograms[0][0].Write()

    #    outfile.Close()

    canv  = ROOT.TCanvas("c1", "c1", 800, 600)
    for i_shell in range(5):
        histograms[0][i_shell].Draw("colz")
        raw_input("Press 'Enter' to for next radial shell")
