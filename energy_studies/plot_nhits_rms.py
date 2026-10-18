#!/usr/bin/env python3
"""Plot hit standard deviations versus square-root mean hits in true-energy bins.

Reads the events tree saved by plot_nhits_vs_energy.py; no MC reprocessing.
With --monoenergies, group by exact simulated energies rather than bins.
RMS means sample standard deviation about the mean (denominator n-1),
not sqrt(mean(x*x)) or the standard error of the mean. The spread includes
energy variation within bins and position variation in the selected sample.
"""

import argparse
from array import array
import csv
import math
from pathlib import Path
import statistics


def sigma_clip(values, threshold):
    """One pass using the original mean and sample SD, independently per observable."""
    if threshold is None or len(values) < 2:
        return list(values)
    mean, sd = statistics.mean(values), statistics.stdev(values)
    return [v for v in values if abs(v - mean) <= threshold * sd]


def bootstrap_errors(values, threshold, replicas, rng):
    """Bootstrap the complete mean/SD estimator, repeating the single-pass cut.

    Return standard errors of sqrt(mean) and sample SD, and their covariance.
    Resample original events, not just the retained core, so the uncertainty
    includes variation in the estimated clipping boundaries.
    """
    import numpy as np
    values = np.asarray(values, dtype=float)
    xs, ys = [], []
    for start in range(0, replicas, 128):
        count = min(128, replicas - start)
        draws = values[rng.integers(0, len(values), size=(count, len(values)))]
        means = draws.mean(axis=1)
        sds = draws.std(axis=1, ddof=1)
        if threshold is not None:
            keep = abs(draws - means[:, None]) <= threshold * sds[:, None]
            n = keep.sum(axis=1)
            if np.any(n < 2):
                raise ValueError('Bootstrap replica has fewer than two retained events')
            means = np.where(keep, draws, 0.).sum(axis=1) / n
            sds = np.sqrt(np.where(keep, (draws - means[:, None]) ** 2, 0.).sum(axis=1) / (n - 1))
        xs.extend(np.sqrt(means))
        ys.extend(sds)
    covariance = np.cov(xs, ys, ddof=1)
    return float(covariance[0, 0] ** 0.5), float(covariance[1, 1] ** 0.5), float(covariance[0, 1])


class Moments:
    """Stable online mean and variance."""

    def __init__(self):
        self.n = 0
        self.mean = 0.0
        self.m2 = 0.0

    def add(self, value):
        self.n += 1
        delta = value - self.mean
        self.mean += delta / self.n
        self.m2 += delta * (value - self.mean)

    def sd(self):
        return math.sqrt(max(0., self.m2 / (self.n - 1)))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('input', help='ROOT summary from plot_nhits_vs_energy.py')
    parser.add_argument('-o', '--output', default='plots/nhits_rms_vs_sqrt_mean')
    parser.add_argument('--emin', type=float, default=1.)
    parser.add_argument('--emax', type=float, default=30.)
    parser.add_argument('--bin-width', type=float, default=1.)
    parser.add_argument('--rmax', type=float, help='Require true radius < rmax in mm')
    parser.add_argument('--monoenergies', type=float, nargs='+',
                        help='Group by these exact kinetic energies in MeV (tolerance 1e-6 MeV)')
    parser.add_argument('--clip-sigma', type=float, help='Single-pass clipping, independently for N and H')
    parser.add_argument('--label', default='', help='Sample label for plot')
    parser.add_argument('--ymax', type=float, help='Maximum displayed y-axis value')
    parser.add_argument('--bootstrap', type=int, default=2000, help='Bootstrap replicas for error bars')
    parser.add_argument('--seed', type=int, default=20261008, help='Bootstrap RNG seed')
    args = parser.parse_args()
    if args.bootstrap < 2:
        parser.error('--bootstrap must be at least 2')
    import numpy as np
    rng = np.random.default_rng(args.seed)
    if args.ymax is not None and (not math.isfinite(args.ymax) or args.ymax <= 0):
        parser.error('--ymax must be finite and positive')
    if args.clip_sigma is not None and (not math.isfinite(args.clip_sigma) or args.clip_sigma <= 0):
        parser.error('--clip-sigma must be finite and positive')
    if args.rmax is not None and (not math.isfinite(args.rmax) or args.rmax <= 0):
        parser.error('--rmax must be finite and positive')
    if not all(math.isfinite(v) for v in (args.emin, args.emax, args.bin_width)):
        parser.error('Energy limits and bin width must be finite')
    if args.bin_width <= 0 or args.emax <= args.emin:
        parser.error('Require bin-width > 0 and emax > emin')
    nbins = round((args.emax - args.emin) / args.bin_width)
    if nbins < 1 or not math.isclose(nbins * args.bin_width, args.emax - args.emin):
        parser.error('Energy range must contain an integer number of bins')
    if args.monoenergies:
        if any(not math.isfinite(e) or e <= 0 for e in args.monoenergies):
            parser.error('Monoenergies must be finite and positive')
        args.monoenergies = sorted(set(args.monoenergies))
        nbins = len(args.monoenergies)
    prefix = Path(args.output).resolve()
    if Path(args.input).resolve() == Path(str(prefix) + '.root'):
        parser.error('Output must differ from input')

    import ROOT
    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(0)
    source = ROOT.TFile.Open(args.input)
    if not source or source.IsZombie():
        parser.error('Cannot open input ROOT file')
    events = source.Get('events')
    if not events or any(not events.GetBranch(b) for b in
                         ('true_energy', 'nhits', 'segmentor_h')):
        parser.error('Input needs events tree with true_energy, nhits and segmentor_h')
    if args.rmax is not None and not events.GetBranch('true_radius'):
        parser.error('--rmax requires the true_radius branch')
    bins = [(Moments(), Moments()) for _ in range(nbins)]
    samples = [([], []) for _ in range(nbins)]
    skipped = 0
    outside_radius = 0
    for event in events:
        if args.rmax is not None:
            radius = float(event.true_radius)
            if not math.isfinite(radius) or radius < 0:
                raise ValueError('Invalid true radius in input')
            if radius >= args.rmax:
                outside_radius += 1
                continue
        energy, raw, corrected = float(event.true_energy), float(event.nhits), float(event.segmentor_h)
        if not all(math.isfinite(v) for v in (energy, raw, corrected)):
            raise ValueError('Non-finite event value in input')
        if raw < 0 or corrected < 0:
            raise ValueError('Negative hit count in input')
        if args.monoenergies:
            matches = [i for i, e in enumerate(args.monoenergies) if abs(energy - e) < 1e-6]
            if len(matches) != 1:
                raise ValueError(f'Unexpected primary energy {energy} MeV')
            index = matches[0]
        else:
            if not args.emin <= energy < args.emax:
                skipped += 1
                continue
            index = min(nbins - 1, int((energy - args.emin) / args.bin_width))
        bins[index][0].add(raw)
        bins[index][1].add(corrected)
        samples[index][0].append(raw)
        samples[index][1].append(corrected)
    source.Close()
    rows = []
    for index, (raw, corrected) in enumerate(bins):
        low = args.monoenergies[index] if args.monoenergies else args.emin + index * args.bin_width
        high = low if args.monoenergies else low + args.bin_width
        if raw.n < 2:
            if args.monoenergies:
                raise ValueError(f'Monoenergetic sample {low:g} MeV has only {raw.n} events')
            print(f'Omitting bin {low:g} MeV: {raw.n} events')
            continue
        original = (raw.n, raw.mean, raw.sd(), corrected.mean, corrected.sd())
        if args.clip_sigma is not None:
            raw, corrected = Moments(), Moments()
            for moment, values in zip((raw, corrected), samples[index]):
                for value in sigma_clip(values, args.clip_sigma):
                    moment.add(value)
                if moment.n < 2:
                    raise ValueError(f'Clipping leaves fewer than two events at {low:g} MeV')
        nx, ny, ncov = bootstrap_errors(samples[index][0], args.clip_sigma, args.bootstrap, rng)
        hx, hy, hcov = bootstrap_errors(samples[index][1], args.clip_sigma, args.bootstrap, rng)
        rows.append([low, high, original[0],
                     raw.mean, math.sqrt(raw.mean), raw.sd(),
                     corrected.mean, math.sqrt(corrected.mean), corrected.sd(),
                     raw.n, corrected.n, *original[1:], nx, ny, hx, hy, ncov, hcov])
    if not rows:
        raise ValueError('No bins with at least two events')

    prefix.parent.mkdir(parents=True, exist_ok=True)
    with open(str(prefix) + '.csv', 'w', newline='') as stream:
        writer = csv.writer(stream)
        writer.writerow(['energy_low_MeV', 'energy_high_MeV', 'events',
                         'mean_N', 'sqrt_mean_N', 'sigma_N',
                         'mean_H', 'sqrt_mean_H', 'sigma_H', 'retained_N', 'retained_H',
                         'original_mean_N', 'original_sigma_N', 'original_mean_H', 'original_sigma_H',
                         'sqrt_mean_N_error', 'sigma_N_error', 'sqrt_mean_H_error', 'sigma_H_error',
                         'sqrt_mean_N_sigma_N_covariance', 'sqrt_mean_H_sigma_H_covariance'])
        writer.writerows(rows)
    graphs = []
    for name, xcol, ycol, excol, eycol, color, marker in [
            ('raw_rms', 4, 5, 15, 16, ROOT.kBlue + 1, 20),
            ('segmentor_rms', 7, 8, 17, 18, ROOT.kRed + 1, 21)]:
        graph = ROOT.TGraphErrors(len(rows), array('d', [r[xcol] for r in rows]),
                            array('d', [r[ycol] for r in rows]),
                            array('d', [r[excol] for r in rows]), array('d', [r[eycol] for r in rows]))
        graph.SetName(name)
        graph.SetMarkerColor(color)
        graph.SetLineColor(color)
        graph.SetMarkerStyle(marker)
        graph.SetMarkerSize(1.0)
        graphs.append(graph)
    xmax = 1.1 * max(r[c] for r in rows for c in (4, 7))
    ymax = 1.4 * max(xmax, max(r[c] + r[ec] for r in rows for c, ec in ((5, 16), (8, 18))))
    if args.ymax is not None:
        ymax = args.ymax
    canvas = ROOT.TCanvas('rms_canvas', 'Hit RMS Versus Square Root Of Mean Hits', 1000, 700)
    canvas.SetLeftMargin(0.14)
    canvas.SetBottomMargin(0.13)
    canvas.SetGrid()
    frame = canvas.DrawFrame(0., 0., xmax, ymax)
    frame.SetTitle(';#sqrt{Mean Hits};RMS About Mean [Hits]')
    frame.GetYaxis().SetTitleOffset(1.5)
    reference = ROOT.TLine(0., 0., xmax, xmax)
    reference.SetLineStyle(2)
    reference.SetLineColor(ROOT.kGray + 2)
    reference.Draw()
    for graph in graphs:
        graph.Draw('P E1 SAME')
    legend = ROOT.TLegend(0.17, 0.73, 0.75, 0.9)
    legend.SetBorderSize(0)
    legend.AddEntry(graphs[0], 'Raw Nhits: #sigma_{N} vs #sqrt{#LTN#GT}', 'p')
    legend.AddEntry(graphs[1], 'Segmentor-Corrected Hits: #sigma_{H} vs #sqrt{#LTH#GT}', 'p')
    legend.AddEntry(reference, 'Poisson Reference: #sigma = #sqrt{Mean Hits}', 'l')
    legend.Draw()
    note = ROOT.TLatex()
    note.SetNDC(True)
    note.SetTextSize(0.028)
    selection = f'; True R < {args.rmax / 1000:g} m' if args.rmax is not None else ''
    grouping = ('Monoenergetic Electrons' if args.monoenergies
                else f'{args.bin_width:g} MeV True-Energy Bins; One Point Per Bin')
    if args.label:
        grouping += '; ' + args.label
    if args.clip_sigma is not None:
        grouping += f'; {args.clip_sigma:g}#sigma Clip'
    note.DrawLatex(0.16, 0.94, grouping + selection)
    for ext in ('png', 'pdf'):
        canvas.SaveAs(str(prefix) + '.' + ext)
    output = ROOT.TFile(str(prefix) + '.root', 'RECREATE')
    if output.IsZombie():
        raise RuntimeError('Cannot create output ROOT file')
    for graph in graphs:
        graph.Write()
    canvas.Write()
    ROOT.TNamed('definition', __doc__).Write()
    ROOT.TNamed('input_file', str(Path(args.input).resolve())).Write()
    ROOT.TNamed('error_bars', f'Bootstrap standard errors; {args.bootstrap} replicas; seed {args.seed}; '
                'resample original events and reapply clipping in each replica; errors on both axes').Write()
    ROOT.TNamed('sigma_clipping',
                f'One pass, independent N/H masks, {args.clip_sigma} SD; recompute mean and sample SD'
                if args.clip_sigma is not None else 'No clipping').Write()
    if args.monoenergies:
        ROOT.TNamed('monoenergies_MeV', ', '.join(map(str, args.monoenergies))).Write()
    else:
        ROOT.TNamed('energy_bins', f'{args.emin:g} <= E < {args.emax:g} MeV; width {args.bin_width:g} MeV').Write()
    ROOT.TNamed('radius_cut', f'True R < {args.rmax:g} mm' if args.rmax is not None
                else 'No additional radius cut').Write()
    output.Close()
    print(f'Plotted {len(rows)} groups from {sum(r[2] for r in rows)} input events; '
          f'{outside_radius} events rejected by radius cut; '
          f'{skipped} remaining events outside energy range.')
    if args.clip_sigma is not None:
        print(f'Retained N: {sum(r[9] for r in rows)}; retained H: {sum(r[10] for r in rows)}')
    print(f'Saved {prefix}.{{png,pdf,root,csv}}')


if __name__ == '__main__':
    main()
