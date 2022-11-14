#!/usr/bin/env python
import ROOT, rat
# Useful secondary functions for the ScintEffectiveSpeed Coordinator
# Author I Coulter - 09/11/2012 <icoulter@hep.upenn.edu>
#        K Majumdar - 10/09/2014 - Cleanup of Coordinators for new DS

Speeds = [182.0, 182.5, 183.0, 183.5, 184.0, 184.5, 185.0, 185.5, 186.0, 186.5, 187.0, 187.5]
fiducialCut = 4000


# Produce the Radial Bias vs. Effective Speed plots (2D scatter, and a weighted 1D) across all Effective Speeds
def ProducePlots():

    fullPlot = ROOT.TH2D("fullPlot", "Radial Bias vs. Effective Speed", 2000, -1000.0, 1000.0, 150, 100, 250)
    oneDimPlot = ROOT.TH1D("oneDimPlot", "Radial Bias vs. Effective Speed in 1 Dimension", 2000, -1000.0, 1000.0)
    oneDimTGraph = ROOT.TGraphErrors()

    count = 0
    for speed in Speeds:
        infileName = "scintFit_" + str(float(speed)) + ".root"
        infileName = "/data/snoplus/parkerw/ratSimulations/batch/Nov4RecoordScintVel_round5/Template_Macro_" + str(float(speed)) + ".root"
        meanBias = MeanRadialBias(infileName)

        fullPlot.Fill(meanBias[0], speed)
        oneDimPlot.Fill(meanBias[0], speed)
        #oneDimTGraph.SetPoint(count, meanBias[0], speed)
        oneDimTGraph.SetPoint(count, speed, meanBias[0])
        oneDimTGraph.SetPointError(count, 0, meanBias[1])
        print(speed, meanBias[0])
        count = count + 1

    return [fullPlot, oneDimPlot, oneDimTGraph]
	
	
# Find the mean radial bias at a single speed
def MeanRadialBias(infileName):

    biasPlot = ROOT.TH1D("RadialBias", "RadialBias", 200, -1000.0, 1000.0)
    biasFit = ROOT.TF1("biasFit", "gaus", -1000, 1000)

    for ds, run in rat.dsreader(infileName):
        if ds.GetEVCount() == 0:
            continue

        startPosition = ds.GetMC().GetMCParticle(0).GetPosition()

        ev = ds.GetEV(0)
        
        if not ev.FitResultExists("positionTimeFit"):
            continue
        if not ev.GetFitResult("positionTimeFit").GetValid():
            continue

        try:
            fitPosition = ev.GetFitResult("positionTimeFit").GetVertex(0).GetPosition()
            radialBias = (fitPosition - startPosition).Dot(fitPosition.Unit())

            if fitPosition.Mag() < fiducialCut:
                biasPlot.Fill(radialBias)
        except:
            pass

    biasPlot.Fit(biasFit)

    return (biasFit.GetParameter(1), biasFit.GetParError(1))

	
# Draw the Radial Bias vs. Effective Speed 2D plot
def DrawPlot():

    histograms = ProducePlots()
	
    histograms[2].Draw("AP")
    histograms[2].GetXaxis().SetTitle("Radial Bias, mm")
    histograms[2].GetYaxis().SetTitle("Effective Speed, mm/ns")
    histograms[2].SetMarkerStyle(20)

    raw_input("Press 'Enter' to exit")


ROOT.gROOT.SetStyle("Plain")
ROOT.gStyle.SetCanvasBorderMode(0)
ROOT.gStyle.SetPadBorderMode(0)
ROOT.gStyle.SetPadColor(0)
ROOT.gStyle.SetCanvasColor(0)
ROOT.gStyle.SetOptTitle(0)
ROOT.gStyle.SetOptStat(0)

