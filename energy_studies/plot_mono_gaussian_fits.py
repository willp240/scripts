#!/usr/bin/env python3
"""Fit Gaussian cores to monoenergetic N/H samples, using a truncated likelihood.

Windows are fixed at median +/- 3 * (1.4826022185 MAD), independently per
energy and observable. The unbinned likelihood is normalised within each
window. Errors are Minuit HESSE errors (Delta NLL=0.5), conditional on that
fixed window and on the Gaussian model. No bootstrap errors are used.
"""
import argparse
from array import array
import csv
import json
import math
from pathlib import Path

import numpy as np
from scipy.special import log_ndtr, ndtr, ndtri
from scipy.stats import chi2


def log_probability(a, b):
    # Stable log(Phi(b)-Phi(a)), including windows wholly above the mean.
    if a > 0:
        a, b = -b, -a
    left, right = float(log_ndtr(a)), float(log_ndtr(b))
    if left >= right:
        return -math.inf
    return right + math.log(-math.expm1(left - right))


def fit_gaussian(values, window_sigma=3., fit_range=None, integer_counts=False):
    import ROOT
    x = np.asarray(values, dtype=float)
    if len(x) < 20 or not np.all(np.isfinite(x)):
        raise ValueError('Need at least 20 finite events')
    median = float(np.median(x))
    scale = float(1.482602218505602 * np.median(abs(x - median)))
    if scale <= 0:
        raise ValueError('Zero robust width')
    lo, hi = fit_range if fit_range is not None else (median-window_sigma*scale, median+window_sigma*scale)
    if integer_counts:
        # Put count-data boundaries between integers; retain the same events.
        lo, hi = math.ceil(lo)-0.5, math.floor(hi)+0.5
    selected = x[(x >= lo) & (x <= hi)]
    n = len(selected)
    if n < 20:
        raise ValueError('Too few events inside fit window')
    y = (selected - median) / scale
    a, b = (lo - median) / scale, (hi - median) / scale
    ymean, yvar = float(y.mean()), float(y.var())

    def nll(par):
        mean, log_sigma = float(par[0]), float(par[1])
        sigma = math.exp(log_sigma)
        log_z = log_probability((a - mean)/sigma, (b - mean)/sigma)
        if not math.isfinite(log_z):
            return 1.e100
        return n * (log_sigma + 0.5 * (yvar + (ymean - mean)**2) / sigma**2 + log_z)

    minimizer = ROOT.Math.Factory.CreateMinimizer('Minuit2', 'Migrad')
    minimizer.SetMaxFunctionCalls(10000)
    minimizer.SetTolerance(1.e-7)
    minimizer.SetPrintLevel(-1)
    minimizer.SetErrorDef(0.5)
    functor = ROOT.Math.Functor(nll, 2)
    minimizer.SetFunction(functor)
    minimizer.SetLimitedVariable(0, 'scaled_mean', ymean, 0.01, -10., 10.)
    minimizer.SetLimitedVariable(1, 'log_scaled_sigma', math.log(y.std()), 0.01, -5., 5.)
    success = bool(minimizer.Minimize())
    hesse = bool(minimizer.Hesse())
    status, covariance_status = int(minimizer.Status()), int(minimizer.CovMatrixStatus())
    if not success or not hesse or status != 0 or covariance_status != 3:
        raise RuntimeError(f'Gaussian fit failed: status={status}, covariance={covariance_status}')
    pars = [float(minimizer.X()[i]) for i in range(2)]
    if abs(pars[0]) > 9.9 or abs(pars[1]) > 4.9:
        raise RuntimeError('Fit reached a parameter boundary')
    mean = median + scale * pars[0]
    sigma = scale * math.exp(pars[1])
    covariance = np.array([[float(minimizer.CovMatrix(i,j)) for j in range(2)] for i in range(2)])
    jacobian = np.diag([scale, sigma])
    covariance = jacobian @ covariance @ jacobian
    mean_error, sigma_error = np.sqrt(np.diag(covariance))
    if mean <= 0 or not np.all(np.isfinite(covariance)):
        raise RuntimeError('Invalid fit parameters/covariance')
    log_z = log_probability((lo-mean)/sigma, (hi-mean)/sigma)

    # Approximate Pearson goodness of fit: 20 equal-probability bins, with
    # 2 fitted shape parameters and a fixed observed event total (17 dof).
    probabilities = np.linspace(ndtr((lo-mean)/sigma), ndtr((hi-mean)/sigma), 21)
    edges = mean + sigma * ndtri(probabilities)
    edges[0], edges[-1] = lo, hi
    if integer_counts:
        edges[1:-1] = np.floor(edges[1:-1]) + 0.5
        edges = np.unique(edges)
    observed, _ = np.histogram(selected, bins=edges)
    expected = n*np.diff(ndtr((edges-mean)/sigma))/math.exp(log_z)
    pearson = float(np.sum((observed-expected)**2/expected))
    dof = len(expected)-3
    return dict(input_events=len(x), fitted_events=n, median=median, robust_sigma=scale,
                fit_low=lo, fit_high=hi, mean=mean, mean_error=float(mean_error),
                sigma=sigma, sigma_error=float(sigma_error), sqrt_mean=math.sqrt(mean),
                sqrt_mean_error=float(mean_error)/(2*math.sqrt(mean)),
                mean_sigma_covariance=float(covariance[0,1]),
                mean_sigma_correlation=float(covariance[0,1]/(mean_error*sigma_error)),
                log_window_probability=log_z, status=status, covariance_status=covariance_status,
                edm=float(minimizer.Edm()), pearson_chi2=pearson, pearson_ndf=dof,
                pearson_p_approx=float(chi2.sf(pearson,dof)))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('input')
    parser.add_argument('-o', '--output-dir', required=True)
    parser.add_argument('--label', default='')
    parser.add_argument('--window-sigma', type=float, default=3.)
    parser.add_argument('--ymax', type=float)
    parser.add_argument('--xmax', type=float)
    parser.add_argument('--comparison-csv', help='Previous RMS CSV to compare with Gaussian widths')
    parser.add_argument('--style-from', help='Existing RMS ROOT canvas whose presentation is preserved exactly')
    args = parser.parse_args()
    if not math.isfinite(args.window_sigma) or args.window_sigma <= 0:
        parser.error('--window-sigma must be finite and positive')
    if args.ymax is not None and (not math.isfinite(args.ymax) or args.ymax <= 0):
        parser.error('--ymax must be finite and positive')
    if args.xmax is not None and (not math.isfinite(args.xmax) or args.xmax <= 0):
        parser.error('--xmax must be finite and positive')
    import ROOT
    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(0)
    outdir = Path(args.output_dir).resolve()
    if Path(args.input).resolve() == outdir / 'gaussian_fits.root':
        parser.error('Output must differ from input')
    source = ROOT.TFile.Open(args.input)
    if not source or source.IsZombie() or not source.Get('events'):
        raise ValueError('Input needs the saved events tree')
    groups = {}
    for row in source.Get('events'):
        energy = round(float(row.true_energy), 6)
        if not all(math.isfinite(v) for v in (energy, row.nhits, row.segmentor_h)):
            raise ValueError('Non-finite event value')
        group = groups.setdefault(energy, {'N': [], 'H': []})
        group['N'].append(float(row.nhits))
        group['H'].append(float(row.segmentor_h))
    source.Close()
    results = []
    for energy, group in sorted(groups.items()):
        for observable, values in group.items():
            result = fit_gaussian(values, args.window_sigma, integer_counts=observable=='N')
            result.update(energy_MeV=energy, observable=observable)
            results.append(result)
            print(f'{energy:g} MeV {observable}: sigma={result["sigma"]:.3f} +/- '
                  f'{result["sigma_error"]:.3f}; {result["fitted_events"]}/{len(values)} '
                  f'events; approximate p={result["pearson_p_approx"]:.3g}', flush=True)
    if not results:
        raise ValueError('No events')
    outdir.mkdir(parents=True, exist_ok=True)
    with (outdir/'fit_results.csv').open('w', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=list(results[0]))
        writer.writeheader()
        writer.writerows(results)
    (outdir/'fit_results.json').write_text(json.dumps(results, indent=2)+'\n')
    lookup = {(r['energy_MeV'], r['observable']): r for r in results}
    output = ROOT.TFile(str(outdir/'gaussian_fits.root'), 'RECREATE')
    if output.IsZombie():
        raise RuntimeError('Cannot create output')
    canvas_names = []
    for energy, group in sorted(groups.items()):
        tag = f'{energy:g}'.replace('.', 'p')+'MeV'
        canvas = ROOT.TCanvas('fits_'+tag, f'{energy:g} MeV Gaussian Fits', 1300, 620)
        canvas.Divide(2, 1)
        objects = []
        for panel, (observable, values) in enumerate(group.items(), 1):
            r = lookup[(energy, observable)]
            colour = ROOT.kBlue+1 if observable == 'N' else ROOT.kRed+1
            label = 'Raw Nhits' if observable == 'N' else 'Segmentor-Corrected Hits (H)'
            pad = canvas.cd(panel)
            pad.SetLeftMargin(0.14)
            pad.SetBottomMargin(0.12)
            pad.SetTopMargin(0.1)
            # Display original events within +/-4 robust sigmas. Explicitly
            # record counts outside the view; store full-range histograms too.
            lo, hi = r['median']-4*r['robust_sigma'], r['median']+4*r['robust_sigma']
            hist = ROOT.TH1D(f'{observable}_{tag}', '', 50, lo, hi)
            hist.SetDirectory(0)
            hist.FillN(len(values), array('d', values), array('d', [1.]*len(values)))
            hist.SetLineColor(colour)
            hist.SetMarkerColor(colour)
            hist.SetMarkerStyle(20)
            hist.SetMarkerSize(0.55)
            hist.SetTitle(f'{energy:g} MeV;{label};Events / {hist.GetBinWidth(1):.3g} Hits')
            hist.GetYaxis().SetTitleOffset(1.45)
            amplitude = r['fitted_events']*hist.GetBinWidth(1)/(r['sigma']*math.sqrt(2*math.pi)*math.exp(r['log_window_probability']))
            hist.SetMaximum(1.55*max(hist.GetMaximum(),amplitude))
            hist.SetMinimum(0.)
            hist.Draw('E1')
            curve = ROOT.TF1(f'gaussian_{observable}_{tag}', '[0]*exp(-0.5*((x-[1])/[2])^2)', r['fit_low'], r['fit_high'])
            curve.SetParameters(amplitude, r['mean'], r['sigma'])
            curve.SetParNames('Display Amplitude', 'Mean', 'Sigma')
            curve.SetParError(1, r['mean_error'])
            curve.SetParError(2, r['sigma_error'])
            curve.SetLineColor(ROOT.kBlack)
            curve.SetLineWidth(2)
            curve.Draw('SAME')
            objects.extend([hist,curve])
            for edge in (r['fit_low'], r['fit_high']):
                line = ROOT.TLine(edge, 0., edge, 0.63*hist.GetMaximum())
                line.SetLineStyle(2)
                line.SetLineColor(ROOT.kGray+2)
                line.Draw()
                objects.append(line)
            legend = ROOT.TLegend(0.17, 0.72, 0.78, 0.88)
            legend.SetBorderSize(0)
            legend.SetTextSize(0.028)
            legend.AddEntry(hist, label, 'lep')
            legend.AddEntry(curve, f'Gaussian Fit: #mu = {r["mean"]:.2f} #pm {r["mean_error"]:.2f}, '
                            f'#sigma = {r["sigma"]:.2f} #pm {r["sigma_error"]:.2f}', 'l')
            legend.Draw()
            objects.append(legend)
            full_lo, full_hi = min(values)-1., max(values)+1.
            full = ROOT.TH1D(f'full_{observable}_{tag}', 'All Events;Hits;Events', 200, full_lo, full_hi)
            full.SetDirectory(0)
            full.FillN(len(values),array('d',values),array('d',[1.]*len(values)))
            output.cd()
            hist.Write()
            full.Write()
            curve.Write()
            covariance = ROOT.TMatrixDSym(2)
            covariance[0][0] = r['mean_error']**2
            covariance[1][1] = r['sigma_error']**2
            covariance[0][1] = covariance[1][0] = r['mean_sigma_covariance']
            covariance.Write(f'covariance_{observable}_{tag}')
        canvas.cd()
        for extension in ('png','pdf'):
            canvas.SaveAs(str(outdir/f'fits_{tag}.{extension}'))
        output.cd()
        canvas.Write()
        canvas_names.append(canvas.GetName())
        canvas.Close()
    # ROOT requires multipage PDF writing to be separate from individual PDFs.
    for filename in ('all_energies.pdf', 'all_energy_fits.pdf'):
        path = str(outdir/filename)
        for index,name in enumerate(canvas_names):
            page = output.Get(name)
            if index == 0:
                page.Print(path+'[')
            page.Print(path)
            if index == len(canvas_names)-1:
                page.Print(path+']')
    summary = ROOT.TCanvas('gaussian_width_canvas', 'Gaussian Width Versus Square Root Of Fitted Mean', 1000, 700)
    summary.SetLeftMargin(0.14)
    summary.SetBottomMargin(0.13)
    summary.SetGrid()
    graphs = []
    for observable,colour,marker in [('N',ROOT.kBlue+1,20),('H',ROOT.kRed+1,21)]:
        rows = [r for r in results if r['observable']==observable]
        graph = ROOT.TGraphErrors(len(rows), *[array('d',[r[key] for r in rows]) for key in ('sqrt_mean','sigma','sqrt_mean_error','sigma_error')])
        graph.SetName('gaussian_'+observable)
        graph.SetMarkerColor(colour)
        graph.SetLineColor(colour)
        graph.SetMarkerStyle(marker)
        graphs.append(graph)
    xmax = args.xmax or 1.1*max(r['sqrt_mean']+r['sqrt_mean_error'] for r in results)
    ymax = args.ymax or 1.4*max(xmax,max(r['sigma']+r['sigma_error'] for r in results))
    frame = summary.DrawFrame(0.,0.,xmax,ymax)
    frame.SetTitle(';#sqrt{Mean Hits};RMS About Mean [Hits]')
    frame.GetYaxis().SetTitleOffset(1.5)
    reference = ROOT.TLine(0.,0.,xmax,xmax)
    reference.SetLineStyle(2)
    reference.SetLineColor(ROOT.kGray+2)
    reference.Draw()
    for graph in graphs:
        graph.Draw('P E1 SAME')
    legend = ROOT.TLegend(0.17,0.73,0.75,0.90)
    legend.SetBorderSize(0)
    legend.AddEntry(graphs[0],'Raw Nhits: #sigma_{N} vs #sqrt{#LTN#GT}','p')
    legend.AddEntry(graphs[1],'Segmentor-Corrected Hits: #sigma_{H} vs #sqrt{#LTH#GT}','p')
    legend.AddEntry(reference,'Poisson Reference: #sigma = #sqrt{Mean Hits}','l')
    legend.Draw()
    title = ROOT.TLatex()
    title.SetNDC(True)
    title.SetTextSize(0.028)
    title.DrawLatex(0.16,0.94,f'Monoenergetic Electrons; {args.label}; {args.window_sigma:g}#sigma Clip')
    if args.style_from:
        style_file = ROOT.TFile.Open(args.style_from)
        if not style_file or style_file.IsZombie() or not style_file.Get('rms_canvas'):
            raise ValueError('Style source needs an rms_canvas')
        summary.Close()
        summary = style_file.Get('rms_canvas').Clone('gaussian_width_canvas')
        style_file.Close()
        graphs = []
        for observable, name in [('N','raw_rms'), ('H','segmentor_rms')]:
            graph = summary.GetListOfPrimitives().FindObject(name)
            if not graph or not graph.InheritsFrom('TGraphErrors'):
                raise ValueError(f'Style source needs graph {name} with errors')
            rows = [r for r in results if r['observable']==observable]
            graph.Set(len(rows))
            for i,r in enumerate(rows):
                graph.SetPoint(i,r['sqrt_mean'],r['sigma'])
                graph.SetPointError(i,r['sqrt_mean_error'],r['sigma_error'])
            graphs.append(graph)
        if args.xmax is not None:
            frames = [obj for obj in summary.GetListOfPrimitives()
                      if obj.InheritsFrom('TH1')]
            if len(frames) != 1:
                raise ValueError('Style source needs exactly one axis frame')
            frames[0].GetXaxis().SetLimits(0., args.xmax)
            lines = [obj for obj in summary.GetListOfPrimitives()
                     if obj.InheritsFrom('TLine')]
            for line in lines:
                if line.GetX1() == 0. and line.GetY1() == 0.:
                    line.SetX2(args.xmax)
                    line.SetY2(args.xmax)
        if args.ymax is not None:
            frames = [obj for obj in summary.GetListOfPrimitives()
                      if obj.InheritsFrom('TH1')]
            if len(frames) != 1:
                raise ValueError('Style source needs exactly one axis frame')
            frames[0].SetMaximum(args.ymax)
        summary.Modified()
        summary.Update()
    for extension in ('png','pdf'):
        summary.SaveAs(str(outdir/f'gaussian_width_vs_sqrt_mean.{extension}'))
    output.cd()
    summary.Write()
    for observable,graph in zip(('N','H'), graphs):
        graph.Write('gaussian_'+observable)

    mean_canvas = ROOT.TCanvas('mean_hits_canvas', 'Mean Hits Versus Energy', 1000, 700)
    mean_canvas.SetLeftMargin(0.14)
    mean_canvas.SetBottomMargin(0.13)
    mean_canvas.SetGrid()
    mean_graphs = []
    for observable,colour,marker in [('N',ROOT.kBlue+1,20),('H',ROOT.kRed+1,21)]:
        rows = [r for r in results if r['observable']==observable]
        graph = ROOT.TGraphErrors(
            len(rows), array('d',[r['energy_MeV'] for r in rows]),
            array('d',[r['mean'] for r in rows]), array('d',[0.]*len(rows)),
            array('d',[r['sigma'] for r in rows]))
        graph.SetName('mean_'+observable)
        graph.SetMarkerColor(colour)
        graph.SetLineColor(colour)
        graph.SetMarkerStyle(marker)
        graph.SetLineWidth(2)
        mean_graphs.append(graph)
    energy_max = 1.05*max(r['energy_MeV'] for r in results)
    hits_max = 1.18*max(r['mean']+r['mean_error'] for r in results)
    mean_frame = mean_canvas.DrawFrame(0., 0., energy_max, hits_max)
    mean_frame.SetTitle(';True Electron Kinetic Energy [MeV];Mean Hits')
    mean_frame.GetYaxis().SetTitleOffset(1.5)
    for graph in mean_graphs:
        graph.Draw('PL E1 SAME')
    mean_legend = ROOT.TLegend(0.17,0.73,0.70,0.88)
    mean_legend.SetBorderSize(0)
    mean_legend.AddEntry(mean_graphs[0], 'Raw Nhits', 'lp')
    mean_legend.AddEntry(mean_graphs[1], 'Segmentor-Corrected Hits (H)', 'lp')
    mean_legend.Draw()
    mean_title = ROOT.TLatex()
    mean_title.SetNDC(True)
    mean_title.SetTextSize(0.028)
    mean_title.DrawLatex(0.16,0.94,f'Monoenergetic Electrons; {args.label}')
    mean_canvas.Modified()
    mean_canvas.Update()
    for extension in ('png','pdf'):
        mean_canvas.SaveAs(str(outdir/f'mean_hits_vs_energy.{extension}'))
    output.cd()
    mean_canvas.Write()
    for observable,graph in zip(('N','H'), mean_graphs):
        graph.Write('mean_'+observable)

    ROOT.TNamed('method', __doc__).Write()
    ROOT.TNamed('window_sigma', str(args.window_sigma)).Write()
    ROOT.TNamed('input_file', str(Path(args.input).resolve())).Write()
    output.Close()
    if args.comparison_csv:
        with open(args.comparison_csv) as f:
            previous = {float(r['energy_low_MeV']):r for r in csv.DictReader(f)}
        with (outdir/'comparison_with_rms.csv').open('w',newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['energy_MeV','observable','gaussian_sigma','fit_error','previous_rms','previous_bootstrap_error','relative_width_change'])
            for r in results:
                old=previous[r['energy_MeV']]
                rms=float(old['sigma_'+r['observable']])
                writer.writerow([r['energy_MeV'],r['observable'],r['sigma'],r['sigma_error'],rms,old['sigma_'+r['observable']+'_error'],r['sigma']/rms-1.])
    print(f'Saved Gaussian fits to {outdir}')


if __name__ == '__main__':
    main()
