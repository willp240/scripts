#!/usr/bin/env python
'''
A python tool for evaluating the performance
of SNO+ fitter algorithms.

Author: Ed Leming <edward.leming@physics.ox.ac.uk>
Contact: Mark Anderson <anderson.mark@queensu.ca>
'''
import ROOT
# Stop ROOT stealing our useage messages
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gErrorIgnoreLevel = ROOT.kWarning

from timeit import default_timer as timer
import argparse
import os
import sys

from utilities.plotting_tools import plot_performance_vs_dimension, plot_2d

if __name__ == "__main__":
    parser = argparse.ArgumentParser() #formatter_class=argparse.ArgumentDefaultsHelpFormatter

    parser.add_argument('infile', type=str,
                        help="File(s) to be read in. Will accept wildcard (must be in quotes though!)")

    dimension_choices = ['x', 'y', 'z', 'r', 'e', 'radius', 'energy']
    choices = dimension_choices + ['2d']
    parser.add_argument('dimension', type=str.lower, default='x',
                        choices=choices, metavar='dimension',
                        help="Variable to make 1D plots with respect to. If 2D plots desired, use option 2D. Choices are: %(choices)s. (Default: %(default)s)")

    parser.add_argument('-n', '--nevents', type=int, default=100000,
                        help="Maximum number of fitted events to evaluate from infile(s). (Default: %(default)d)")

    parser.add_argument('-f', '--fitter', type=str, default="scintFitter",
                        help="Name of the fitter processor to be evaluated. (Default: %(default)s)")

    parser.add_argument('-o', '--outfile', type=str, default="1D_plots.root",
                        help="Path to output file. (Default: %(default)s)")

    parser.add_argument('-c', '--config', type=str, default="plotting_1D.cfg",
                        help="Path to plotting configuration file. (Default: %(default)s)")

    choices_bias_metrics = ['gaus_mean', 'mean', 'median', 'mode']
    parser.add_argument('--bias-metric', type=str, default="gaus_mean",
                        choices=choices_bias_metrics,
                        help="Path to plotting configuration file. Choices are: %(choices)s. (Default: %(default)s)")

    choices_resolution_metrics = ['gaus_std', 'fwhm', 'fwhm_std']
    parser.add_argument('--resolution-metric', type=str, default="gaus_std",
                        choices=choices_resolution_metrics,
                        help="Path to plotting configuration file. Choices are: %(choices)s. (Default: %(default)s)")

    parser.add_argument('--images', dest='images', action='store_true',
                        help="Images option. Will save main plots as PNG files.")
    parser.set_defaults(verbose=False)

    parser.add_argument('--verbose', dest='verbose', action='store_true',
                        help="Verbose option. Will print individual event information upon failures.")
    parser.set_defaults(verbose=False)

    parser.add_argument('--debug', dest='debug', action='store_true',
                        help="Debug option. Will save all intermediate histograms and plots to the destination file. Does nothing when dimension is 2D.")
    parser.set_defaults(debug=False)

    parser.add_argument('--autoview', dest='autoview', action='store_true',
                        help="Automatically change viewing range of debug histograms based on non-empty bins.")
    parser.set_defaults(autoview=False)

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
    if not os.path.isabs(args.config) and not os.path.dirname(args.config):
        directory = os.path.dirname(sys.argv[0])
        args.config = os.path.join(directory, args.config)

    # Read in plotting config.
    # TODO: Remove use of exec here, bad practice!
    config = {}
    exec(compile(open(args.config).read(), args.config, 'exec'), {}, config)

    # Based on dimension argument, choose whether to make 1D or 2D plots.
    if args.dimension in dimension_choices:
        # Plot performance vs the requested dimension
        plot_performance_vs_dimension(args.infile, args.dimension, args.outfile,
                                      config, fitter=args.fitter,
                                      nevents=args.nevents,
                                      bias_metric=args.bias_metric,
                                      resolution_metric=args.resolution_metric,
                                      images=args.images,
                                      verbose=args.verbose,
                                      debug=args.debug,
                                      autoview=args.autoview)
    elif args.dimension in ["2d"]:
        # Plot all of the 2D plots requested.
        plot_2d(args.infile, args.outfile,
                config, fitter=args.fitter,
                nevents=args.nevents,
                images=args.images,
                verbose=args.verbose)
    else:
        raise Warning("{} is not a valid dimension.".format(args.dimension))

    mm, ss = divmod(timer() - start, 60)
    print("######################################")
    print("Full script took {:02d}:{:02d}.".format(int(mm), int(ss)))
