import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True

import numpy as np
from collections import OrderedDict

from .dimension_tools import DimensionTool1D

colorsDictionary = {"x" : ROOT.kRed-4,
                    "y" : ROOT.kAzure-3,
                    "z" : ROOT.kGreen+2,
                    "r" : ROOT.kBlack,
                    "e" : ROOT.kBlack}

legend_coords = (0.905, 0.675, 0.98, 0.8)
legend_coords_br = (0.871, 0.8165, 0.946, 0.9415)

def fit_histograms(histos, dimTool):
    '''
    Fit all histograms in a list to a Gaussian.
    Extract the bias and resolution for a given dimension.
    '''
    # Extract parameters from dim tool.
    axisMin, _ = dimTool.get_axis_bounds()
    binWidth = dimTool.get_bin_width()
    if dimTool.get_dimension() in ['x', 'y', 'z', 'r']:
        units = 'mm'
    else:
        units = 'MeV'
    
    # Init some arrays to hold values
    x, x_error = np.zeros( len(histos) ), np.zeros( len(histos) )+(binWidth/2.)
    bias, bias_error = np.zeros( len(histos) ), np.zeros( len(histos) )
    resolution, resolution_error = np.zeros( len(histos) ), np.zeros( len(histos) )
    
    chisquare = np.zeros( len(histos) )
    
    gausFunction = ROOT.TF1("myGaus", "gaus(0)")
    
    # loop over histograms and extract parameters from gaussian fits
    for i, hist in enumerate(histos): 
        x[i] =  axisMin + i*binWidth + binWidth/2.
        if hist.Integral() > dimTool.get_parameter("fit_threshold"):
            gausFunction.SetParameter(0, hist.GetMaximum())
            gausFunction.SetParameter(1, hist.GetBinLowEdge(hist.GetMaximumBin()))
            gausFunction.SetParameter(2, hist.GetStdDev())
            hist.Fit(gausFunction, "QMEL") # Quiet
            peakval, sigma_p, sigma_m, sigma_hpd = 0,0,0,0
            bias[i] = gausFunction.GetParameter(1)
            bias_error[i] = gausFunction.GetParError(1)
            resolution[i] = np.abs(gausFunction.GetParameter(2))
            resolution_error[i] = np.abs(gausFunction.GetParError(2))
            
            chisquare[i] = gausFunction.GetChisquare()/gausFunction.GetNDF()
            
        else:
            bias[i] = 0
            bias_error[i] = 0
            resolution[i] = 0
            resolution_error[i] = 0

        # Set a descriptive title.
        low = axisMin+i*binWidth
        up = axisMin+(i+1)*binWidth
        title = 'Distribution for events with {0} in range ({1:.2f}, {2:.2f}) {3}'
        title = title.format(dimTool.get_dimension().upper(), low, up, units)
        hist.SetTitle(title)
            
    # Delete points where fits weren't performed
    noFits = np.where(bias==0)
    x = np.delete(x,noFits,axis=0)
    bias = np.delete(bias,noFits,axis=0)
    bias_error = np.delete(bias_error,noFits,axis=0)
    resolution = np.delete(resolution,noFits,axis=0)
    resolution_error = np.delete(resolution_error,noFits,axis=0)
    
    chisquare = np.delete(chisquare,noFits,axis=0)
    
    if len(bias) == 0:
        print("WARNING: Cannot make TGraphs (all histograms below fit_threshold)")
        biasErr = ROOT.TGraphErrors()
        resolutionErr = ROOT.TGraphErrors()
        
        chisquare = ROOT.TGraph()
        
    else:
        biasErr = ROOT.TGraphErrors(len(bias), x, bias, x_error, bias_error)
        resolutionErr =  ROOT.TGraphErrors(len(resolution), x, resolution, x_error, resolution_error)

        chisquare = ROOT.TGraph(len(chisquare), x, chisquare)
        
    return biasErr, resolutionErr, chisquare

def generate_bias_resolution_plots(histDictionary, dimTool):
    '''
    Given dictionary of histograms, generate bias and resolution plots.
    '''
    # Create a TCanvas to use to prevent drawing on the ones that get saved.
    # Seems to happen when calling fit_histograms in the loop.
    # After drawing onto a TCanvas, cd to this one.
    dummy_canvas = ROOT.TCanvas("dummy_br", "dummy_br")
    
    # Fit all other build histograms and make plots
    biasMulti, resolutionMulti = ROOT.TMultiGraph(), ROOT.TMultiGraph()
    biasLeg, resolutionLeg = ROOT.TLegend(*legend_coords_br), ROOT.TLegend(*legend_coords_br)
    
    axisMin, axisMax = dimTool.get_axis_bounds()
    axisBinWidth = dimTool.get_bin_width()
    
    # Initialize lists to save the plots to.
    plots_intermediate = []
    plots_final = []
    
    for dim, histList in histDictionary.items():
        # Fit all of the histograms in the list and return the plots.
        biasPlot, resolutionPlot, chisquarePlot = fit_histograms(histList, dimTool)
        
        # Format plots and write 
        measurement, units = "Position", "mm"
        if dim in ['e']:
            measurement, units = "Energy", "%"
            #biasPlot.SetMaximum(50)
            #resolutionPlot.SetMaximum(50)
            
        chisquarePlot.SetName("{0}_chisquare".format(dim))
        chisquarePlot.SetTitle("#chi^{{2}}/ndf for fit of {}".format(dim))
        chisquarePlot.GetXaxis().SetTitle(dimTool.get_axis_title())
        chisquarePlot.GetYaxis().SetTitle("#chi^{2}/ndf")
        chisquarePlot.SetLineColor(colorsDictionary[dim])
        chisquarePlot.SetMarkerColor(colorsDictionary[dim])
        chisquarePlot.GetXaxis().SetRangeUser(axisMin, axisMax)
        plots_intermediate.append(chisquarePlot)
            
        biasPlot.SetName("{0}_bias".format(dim))
        biasPlot.SetTitle("{0} bias in {1}".format(measurement, dim))
        biasPlot.GetXaxis().SetTitle(dimTool.get_axis_title())
        biasPlot.GetYaxis().SetTitle("Bias [{0}]".format(units))
        #biasPlot.SetLineWidth(3)
        #biasPlot.SetMarkerStyle(21)
        biasPlot.SetLineColor(colorsDictionary[dim])
        biasPlot.SetMarkerColor(colorsDictionary[dim])
        biasPlot.SetFillStyle(0)
        biasPlot.GetXaxis().SetRangeUser(axisMin, axisMax)
        plots_intermediate.append(biasPlot)
        
        resolutionPlot.SetName("{0}_resolution".format(dim))
        resolutionPlot.SetTitle("{0} resolution in {1}".format(measurement, dim))
        resolutionPlot.GetXaxis().SetTitle(dimTool.get_axis_title())
        resolutionPlot.GetYaxis().SetTitle("Resolution [{0}]".format(units))
        #resolutionPlot.SetLineWidth(3)
        #resolutionPlot.SetMarkerStyle(21)
        resolutionPlot.SetLineColor(colorsDictionary[dim])
        resolutionPlot.SetMarkerColor(colorsDictionary[dim])
        resolutionPlot.SetFillStyle(0)
        resolutionPlot.GetXaxis().SetRangeUser(axisMin, axisMax)
        plots_intermediate.append(resolutionPlot)
        
        if dim in ['x','y','z']:
            # If Cartesian dimension, put into a TMultiGraph for comparison.
            biasMulti.Add(biasPlot,"APL")
            resolutionMulti.Add(resolutionPlot,"APL")
            biasLeg.AddEntry(biasPlot,"{0: >5}".format(dim), "l")
            resolutionLeg.AddEntry(resolutionPlot, "{0: >5}".format(dim), "l")
            
        elif dim in ['e', 'r']:
            # If energy or radius, plot on TCanvas immediately (no need for TMultiGraph).
            biasCanv = ROOT.TCanvas(biasPlot.GetName()+"_c",
                                    biasPlot.GetTitle())
            biasPlot.Draw("APL")
            biasCanv.Update()
            plots_final.append(biasCanv)
            
            resolutionCanv = ROOT.TCanvas(resolutionPlot.GetName()+"_c",
                                          resolutionPlot.GetTitle())
            resolutionPlot.Draw("APL")
            resolutionCanv.Update()
            plots_final.append(resolutionCanv)

        # Change to dummy TCanvas to prevent accidental overwrites.
        dummy_canvas.cd()
    
    # Plot multigraphs on canvas
    bias_name = "xyz_bias"
    biasMultiCan = ROOT.TCanvas(bias_name+"_c", "Position bias in x, y and z")
    biasMulti.SetTitle("Position bias in x, y and z")
    biasMulti.SetName(bias_name)
    biasMulti.Draw("APL")
    biasMulti.GetYaxis().SetTitle("Bias [mm]")
    biasMulti.GetXaxis().SetTitle(dimTool.get_axis_title())
    biasMulti.GetXaxis().SetRangeUser(axisMin, axisMax)
    biasLeg.Draw()
    biasMultiCan.Update()
    plots_intermediate.append(biasMulti)
    plots_final.append(biasMultiCan.Clone())
    
    resolution_name = "xyz_resolution"
    resolutionMultiCan = ROOT.TCanvas(resolution_name+"_c", "Position resolution in x, y and z")
    resolutionMulti.SetTitle("Position resolution in x, y and z")
    resolutionMulti.SetName(resolution_name)
    resolutionMulti.Draw("APL")
    resolutionMulti.GetYaxis().SetTitle("Resolution [mm]")
    resolutionMulti.GetXaxis().SetTitle(dimTool.get_axis_title())
    resolutionMulti.GetXaxis().SetRangeUser(axisMin, axisMax)
    resolutionLeg.Draw()
    resolutionMultiCan.Update()
    plots_intermediate.append(resolutionMulti)
    plots_final.append(resolutionMultiCan.Clone())
    
    return plots_intermediate, plots_final
    
def generate_efficiency_histogram(histo, allEvents, dimTool, title=""):
    '''
    Function to take in histogram of events and return histogram of efficiency per bin.
    '''
    histo_eff = histo.Clone()
    histo_eff.SetName(histo.GetName()+"Efficiency")
    histo_eff.SetTitle(histo.GetTitle()+" efficiency")
    histo_eff.GetXaxis().SetTitle(dimTool.get_axis_title())
    histo_eff.GetXaxis().SetRangeUser(*dimTool.get_axis_bounds())
    histo_eff.GetYaxis().SetRangeUser(0, 1.2)
    histo_eff.Divide(allEvents)
    
    return histo_eff

def initialize_histograms_reconstruction(config):
    '''
    Function to initialize a dictionary of histograms.
    For the distributions of the reconstructed quantities.
    '''
    # Initialize ordered dictionary.
    histReconDict = OrderedDict()
    
    for dim in ['x','y','z','dirx','diry','dirz','r','e']:
        # For the reconstructed quantity.
        dimToolRecon = DimensionTool1D(config, dim)
        axisMin, axisMax = dimToolRecon.get_axis_bounds()
        nbins = int(dimToolRecon.get_nbins())*2
        
        long_name = dimToolRecon.get_dimension_long()
        title = "Reconstructed {}".format(long_name)
        
        if dim in ['x', 'y', 'z', 'r']:
            units = "mm"
            title = title + " [{}]".format(units)
        elif dim in ['e']:
            units = "MeV"
            title = title + " [{}]".format(units)
            
        h = ROOT.TH1D("{}_fit_distribution".format(dim),
                      "Reconstructed {}".format(long_name),
                      nbins, axisMin, axisMax)
        h.GetXaxis().SetTitle(title)
        histReconDict[dim] = h
        
    return histReconDict

def initialize_histograms_comparison(config, nbins):
    '''
    Function to initialize a dictionary of lists of histograms.
    For the bias and resolution plots.
    Accepts config dictionary and number of bins for the desired plotting dimension.
    Each histogram is the difference (fit-truth) corresponding to a given bin.
    '''
    # Initialize ordered dictionary.
    histDictionary = OrderedDict()
    
    # Loop over each y-axis dimension specified in config.
    # Then loop over all bins and initialize the appropriate histogram.
    for dim in config["dimensions"]:
        histDictionary[dim] = []

        # Get the bounds of the histogram for each y-axis dimension.
        minBin, maxBin = -400, 400 #DimensionTool1D(config, dim).get_histo_bounds()

        #TODO: move this to DimensionTool.
        n, units = 20, "mm" #500
        if dim in ['e']: # Special case as we use fractional (%) bias / res for energy
            n, units = 35, "%" #100
        x_title = "Fit - Truth in {0} [{1}]".format(dim.upper(), units)
        
        for i in range(nbins):
            h_i = ROOT.TH1D("{0}_{1:d}".format(dim, i), "", n, minBin, maxBin)
            h_i.GetXaxis().SetTitle(x_title)
            histDictionary[dim].append(h_i)

    return histDictionary

def overlay_distributions(histDictionary):
    '''
    Function to overlay x, y, z distributions on top of each other.
    Applies to both position and direction.
    Also sets the viewing range to something easier to see.
    '''
    # Initialize dictionary for final plots.
    distribution_list_final = []
    
    # Position canvas.
    xyzPosCanv = ROOT.TCanvas("xyz_fit_distribution",
                              "Distribution of reconstructed positions")
    xyzPosLeg = ROOT.TLegend(*legend_coords)
    
    # Direction canvas.
    xyzDirCanv = ROOT.TCanvas("dirxyz_fit_distribution",
                              "Distribution of reconstructed directions")
    xyzDirLeg = ROOT.TLegend(*legend_coords)
    
    for dim, h_dim in histDictionary.items():
        # Set the axis range to better fit the data.
        low = h_dim.FindFirstBinAbove(0)
        high = h_dim.FindLastBinAbove(0)
        if low > 0 or high < h_dim.GetNbinsX():
            h_dim.GetXaxis().SetRange(low, high)
        
        if dim in ['x', 'y', 'z']:
            xyzPosCanv.cd()
            h_dim.SetLineColor(colorsDictionary[dim])
            h_dim.Draw("SAME")
            xyzPosLeg.AddEntry(h_dim, "{0: >5}".format(dim))
            
        elif dim[:3] == 'dir':
            xyzDirCanv.cd()
            h_dim.SetLineColor(colorsDictionary[dim[-1]])
            h_dim.GetYaxis().SetRangeUser(0, h_dim.GetBinContent(h_dim.GetMaximumBin())*1.15)
            h_dim.Draw("SAME")
            xyzDirLeg.AddEntry(h_dim, "{0: >5}".format(dim))
            
        else:
            c = ROOT.TCanvas(h_dim.GetName(), h_dim.GetTitle())
            h_dim.Draw()
            c.Update()
            distribution_list_final.append(c.Clone())

        # To prevent name conflicts, rename all of the histograms.
        h_dim.SetName( h_dim.GetName()+"_h" )
        
    # Draw the legend onto the position canvas.
    xyzPosCanv.cd()
    xyzPosLeg.Draw()
    xyzPosCanv.Update()

    # The following is needed to change the canvas title.
    # And to have it appear the same way as when it uses the first histogram.
    # Absurd that this is required to change a TCanvas title.
    # Why doesn't the SetTitle method work???
    # Based on: https://root-forum.cern.ch/t/change-histograms-title-on-canvas/17854/9
    l = xyzPosCanv.GetListOfPrimitives()
    old_title = l[1].GetTitle() # l[0] is TFrame
    l[1].SetTitle("Distribution of reconstructed positions")
    l.Remove(l.FindObject("title"))
    xyzPosCanv.Update()
    
    distribution_list_final.append(xyzPosCanv.Clone())
    
    # Revert the title back for the first primitive.
    l[1].SetTitle(old_title)
    
    #Draw the legend onto the direction canvas.
    xyzDirCanv.cd()
    xyzDirLeg.Draw()
    xyzDirCanv.Update()
    
    # Same stupid thing to change the title.
    l = xyzDirCanv.GetListOfPrimitives()
    old_title = l[1].GetTitle() # l[0] is TFrame
    l[1].SetTitle("Distribution of reconstructed directions")
    l.Remove(l.FindObject("title"))
    xyzDirCanv.Update()
    
    distribution_list_final.append(xyzDirCanv.Clone())
    
    # Revert the title back for the first primitive.
    l[1].SetTitle(old_title)
    
    return distribution_list_final


#// **************************
def GetHPD(hpost, central, error, error_pos, error_neg):
#  // **************************

  #// Get the bin which has the largest posterior density
    MaxBin = hpost.GetMaximumBin()
  #// And it's value
    peakval = hpost.GetBinCenter(MaxBin)

    #// The total integral of the posterior
    integral = hpost.Integral()

    # Keep count of how much area we're covering
    sum1 = 0.0

  # Counter for current bin
    CurrBin = MaxBin
    while (sum1/integral < 0.6827/2.0 and CurrBin < hpost.GetNbinsX()+1):
        sum1 += hpost.GetBinContent(CurrBin)
        CurrBin+=1
  
    sigma_p = abs(hpost.GetBinCenter(MaxBin)-hpost.GetBinCenter(CurrBin))
 # // Reset the sum
    sum1 = 0.0

#  // Reset the bin counter
    CurrBin = MaxBin
 # // Counter for current bin
    while (sum1/integral < 0.6827/2.0 and CurrBin >= 0):
        sum1 += hpost.GetBinContent(CurrBin)
        CurrBin-=1
    
    sigma_m = abs(hpost.GetBinCenter(CurrBin)-hpost.GetBinCenter(MaxBin));

#  // Now do the double sided HPD
    sum1 = 0.0
    LowBin = MaxBin-1
    HighBin = MaxBin+1
    LowCon = 0.0
    HighCon = 0.0
    while (sum1/integral < 0.6827 and LowBin >= 0 and HighBin < hpost.GetNbinsX()+1):

#    // Get the slice
        LowCon = hpost.GetBinContent(LowBin)
        HighCon = hpost.GetBinContent(HighBin)

#    // If we're on the last slice and the lower contour is larger than the upper
        if (sum1+LowCon+HighCon)/integral > 0.6827 and LowCon > HighCon:
             sum1 += LowCon
             break
        # // If we're on the last slice and the upper contour is larger than the lower
        elif ((sum1+LowCon+HighCon)/integral > 0.6827 and HighCon >= LowCon):
             sum1 += HighCon
             break
        else:
             sum1 += LowCon + HighCon

        LowBin-=1
        HighBin+=1
  
    sigma_hpd = 0.0
    if LowCon > HighCon:
        sigma_hpd = abs(hpost.GetBinCenter(LowBin)-hpost.GetBinCenter(MaxBin))
    else:
        sigma_hpd = abs(hpost.GetBinCenter(HighBin)-hpost.GetBinCenter(MaxBin))

    central = peakval
    error = sigma_hpd
    error_pos = sigma_p
    error_neg = sigma_m

    return error
