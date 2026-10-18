#!/usr/bin/env python3
"""Overlay raw nhits and segmentor H at each monoenergetic electron energy.

Reads the saved events summary, retaining every event used by the RMS plot.
Histograms at each energy share identical bin edges. Displayed means and
sample standard deviations are calculated from unbinned event values.
"""

import argparse
from array import array
import math
from pathlib import Path
import statistics
from plot_nhits_rms import sigma_clip


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('input', help='ROOT file containing the saved events tree')
    parser.add_argument('-o', '--output-dir', default='plots/mono_centre_distributions')
    parser.add_argument('--bins', type=int, default=100,
                        help='Common number of bins per overlay (default: 100)')
    parser.add_argument('--clip-sigma', type=float, help='Single-pass clipping, independently for N and H')
    parser.add_argument('--label', default='', help='Sample label for plot')
    args = parser.parse_args()
    if args.clip_sigma is not None and (not math.isfinite(args.clip_sigma) or args.clip_sigma <= 0):
        parser.error('--clip-sigma must be finite and positive')
    if args.bins < 1:
        parser.error('--bins must be positive')

    import ROOT
    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(0)
    outdir = Path(args.output_dir).resolve()
    rootpath = outdir / 'hit_distributions.root'
    if Path(args.input).resolve() == rootpath:
        parser.error('Output must differ from input')
    source = ROOT.TFile.Open(args.input)
    if not source or source.IsZombie():
        parser.error('Cannot open input')
    tree = source.Get('events')
    if not tree or any(not tree.GetBranch(key) for key in
                       ('true_energy', 'nhits', 'segmentor_h')):
        parser.error('Missing events tree or required branches')
    groups = {}
    for event in tree:
        energy, raw, h = float(event.true_energy), float(event.nhits), float(event.segmentor_h)
        if not all(math.isfinite(x) and x >= 0 for x in (energy, raw, h)):
            raise ValueError('Invalid event value')
        nominal = round(energy, 6)
        group = groups.setdefault(nominal, ([], []))
        group[0].append(raw)
        group[1].append(h)
    source.Close()
    if not groups or any(len(raw) < 2 for raw, _ in groups.values()):
        raise ValueError('Require at least two events per monoenergetic sample')

    outdir.mkdir(parents=True, exist_ok=True)
    output = ROOT.TFile(str(rootpath), 'RECREATE')
    if output.IsZombie():
        raise RuntimeError('Cannot create ROOT output')
    overview = outdir / 'all_energies.pdf'
    for index, (energy, (raw, h)) in enumerate(sorted(groups.items())):
        original_count = len(raw)
        raw, h = sigma_clip(raw, args.clip_sigma), sigma_clip(h, args.clip_sigma)
        if min(len(raw), len(h)) < 2:
            raise ValueError(f'Too few retained events at {energy:g} MeV')
        tag = f'{energy:g}'.replace('.', 'p') + 'MeV'
        low, high = min(min(raw), min(h)), max(max(raw), max(h))
        padding = max(1., 0.05 * (high - low))
        low, high = max(-0.5, low - padding), high + padding
        histograms = []
        for name, values, color in [('nhits', raw, ROOT.kBlue + 1),
                                    ('H', h, ROOT.kRed + 1)]:
            hist = ROOT.TH1D(f'{name}_{tag}', '', args.bins, low, high)
            hist.SetDirectory(0)
            hist.FillN(len(values), array('d', values), array('d', [1.] * len(values)))
            hist.SetLineColor(color)
            hist.SetLineWidth(2)
            histograms.append(hist)
        canvas = ROOT.TCanvas(f'distributions_{tag}', f'{energy:g} MeV Hit Distributions', 1000, 700)
        canvas.SetLeftMargin(0.13)
        canvas.SetBottomMargin(0.12)
        canvas.SetGrid()
        first, second = histograms
        first.SetTitle(f';Hits / Hit Equivalents;Events / {first.GetBinWidth(1):.3g} Hits')
        first.GetYaxis().SetTitleOffset(1.45)
        first.SetMinimum(0.)
        first.SetMaximum(1.5 * max(first.GetMaximum(), second.GetMaximum()))
        first.Draw('HIST')
        second.Draw('HIST SAME')
        legend = ROOT.TLegend(0.16, 0.74, 0.88, 0.90)
        legend.SetBorderSize(0)
        legend.SetTextSize(0.027)
        for hist, label, values in zip(histograms, ('Raw Nhits', 'Segmentor-Corrected Hits (H)'), (raw, h)):
            legend.AddEntry(hist, f'{label}: Mean = {statistics.mean(values):.1f}, '
                            f'#sigma = {statistics.stdev(values):.1f}, n = {len(values)}', 'l')
        legend.Draw()
        title = ROOT.TLatex()
        title.SetNDC(True)
        title.SetTextSize(0.032)
        heading = f'{energy:g} MeV Electrons; {original_count} Input Events'
        if args.label:
            heading += '; ' + args.label
        if args.clip_sigma is not None:
            heading += f'; {args.clip_sigma:g}#sigma Clip'
        title.SetTextSize(0.027)
        title.DrawLatex(0.14, 0.94, heading)
        canvas.Modified()
        canvas.Update()
        for extension in ('png', 'pdf'):
            canvas.SaveAs(str(outdir / f'distributions_{tag}.{extension}'))
        output.cd()
        for hist, values in zip(histograms, (raw, h)):
            assert hist.Integral() == len(values)
            hist.Write()
        canvas.Write()
        canvas.Close()
    output.cd()
    ROOT.TNamed('definition', __doc__).Write()
    ROOT.TNamed('input_file', str(Path(args.input).resolve())).Write()
    ROOT.TNamed('sigma_clipping', f'One pass, independent N/H masks, {args.clip_sigma} SD; statistics from retained unbinned values').Write()
    # Make the multipage PDF separately: ROOT cannot interleave this with
    # writes to individual PDF files without corrupting its PDF state.
    for index, energy in enumerate(sorted(groups)):
        tag = f'{energy:g}'.replace('.', 'p') + 'MeV'
        page = output.Get(f'distributions_{tag}')
        if index == 0:
            page.Print(str(overview) + '[')
        page.Print(str(overview))
        if index == len(groups) - 1:
            page.Print(str(overview) + ']')
    output.Close()
    print(f'Saved {len(groups)} distribution overlays ({sum(len(v[0]) for v in groups.values())} events) to {outdir}')


if __name__ == '__main__':
    main()
