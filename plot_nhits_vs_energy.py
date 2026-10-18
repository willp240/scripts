#!/usr/bin/env python3
"""Plot raw nhits and the unscaled segmentor H parameter from RAT DS files.

Uses the first triggered EV per generated electron (no reconstruction cut).
H follows EnergyRThetaFunctional's PMT selection, default segmentation and
N-1 saturation prescription, without position or detector-state scaling.
Outputs PNG, PDF, ROOT profiles/event values, and a CSV of bin means/SEMs.
Source the RAT environment first. Inputs must be finished full RAT files,
not ntuples; quoted globs and directories are accepted.
"""

import argparse
from array import array
import csv
import glob
from pathlib import Path


HELPER = r'''
#include <RAT/DS/EV.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/Segmentor.hh>
#include <RAT/DU/PMTCalStatus.hh>
#include <cmath>
#include <stdexcept>
#include <vector>
namespace NhitsStudy {
// Return H, saturated-segment count, selected-hit count.
std::vector<double> corrected(const RAT::DS::EV& ev) {
  auto* utility = RAT::DU::Utility::Get();
  const auto& status = utility->GetPMTCalStatus();
  const auto segmentor = utility->GetSegmentor();
  const auto ids = segmentor.GetSegmentIDs();
  const auto populations = segmentor.GetSegmentPopulations();
  std::vector<unsigned int> hits(populations.size(), 0);
  const unsigned int mask = (1u << status.kCHSBit) | (1u << status.kECABit)
    | (1u << status.kPCABit) | (1u << status.kXTalkBit) | (1u << status.kCSSBit);
  unsigned int selected = 0;
  const auto& pmts = ev.GetCalPMTs();
  for (size_t i = 0; i < pmts.GetCount(); ++i) {
    const auto& pmt = pmts.GetPMT(i);
    if (status.GetHitStatus(pmt) & mask) continue;
    const int id = pmt.GetID();
    if (!status.GetChannelStatus(id)) continue;
    ++hits.at(ids.at(id));
    ++selected;
  }
  double h = 0;
  unsigned int saturated = 0;
  for (size_t s = 0; s < populations.size(); ++s) {
    const double n = populations[s];
    double k = hits[s];
    if (k > n) throw std::runtime_error("Segment hits exceed population");
    if (!n) continue;
    if (k == n) { k = n - 1; ++saturated; }
    h -= n * std::log1p(-k / n);
  }
  return {h, double(saturated), double(selected)};
}
}
'''


def input_files(patterns):
    files = set()
    for pattern in patterns:
        path = Path(pattern)
        matches = list(path.glob('*.root')) if path.is_dir() else glob.glob(pattern)
        if not matches:
            raise ValueError(f'No files match {pattern}')
        files.update(str(Path(p).resolve()) for p in matches
                     if not str(p).endswith('.ntuple.root'))
    if not files:
        raise ValueError('No full RAT ROOT files found (ntuples are excluded)')
    return sorted(files)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('inputs', nargs='+', help='Full RAT files, globs or directories')
    parser.add_argument('-o', '--output', default='plots/nhits_vs_true_energy',
                        help='Output prefix (default: %(default)s)')
    parser.add_argument('--bins', type=int, default=39, help='Energy bins (default: 39)')
    parser.add_argument('--emin', type=float, default=0.5)
    parser.add_argument('--emax', type=float, default=20.0)
    parser.add_argument('--rmax', type=float, default=5000., help='True radius cut in mm')
    parser.add_argument('--max-events', type=int, help='Limit generated entries read')
    args = parser.parse_args()
    if args.bins <= 0 or args.emax <= args.emin or args.rmax <= 0:
        parser.error('Require bins > 0, emax > emin and rmax > 0')
    if args.max_events is not None and args.max_events <= 0:
        parser.error('--max-events must be positive')
    try:
        files = input_files(args.inputs)
    except ValueError as exc:
        parser.error(str(exc))

    import ROOT
    import rat  # Loads RAT dictionaries.
    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(0)
    if not ROOT.gInterpreter.Declare(HELPER):
        raise RuntimeError('Could not compile the segmentor helper')

    prefix = Path(args.output).resolve()
    if str(prefix) + '.root' in files:
        parser.error('Output ROOT file would overwrite an input')
    prefix.parent.mkdir(parents=True, exist_ok=True)
    profiles = []
    for name, title in [('raw', 'Raw nhits'), ('segmentor', 'Segmentor H'),
                        ('saturation', 'Fraction with a saturated segment')]:
        p = ROOT.TProfile(name, title, args.bins, args.emin, args.emax)
        p.SetDirectory(0)
        profiles.append(p)
    raw, corrected, saturation = profiles
    output = ROOT.TFile(str(prefix) + '.root', 'RECREATE')
    if output.IsZombie():
        raise RuntimeError('Cannot create output ROOT file')
    events = ROOT.TTree('events', 'First trigger per generated electron')
    values = {key: array('d', [0.]) for key in
              ('true_energy', 'true_radius', 'nhits', 'segmentor_h',
               'saturated_segments', 'selected_hits')}
    for key, value in values.items():
        events.Branch(key, value, key + '/D')
    read = accepted = no_trigger = 0
    saturated_events = 0
    for filename in files:
        if args.max_events is not None and read >= args.max_events:
            break
        check = ROOT.TFile.Open(filename)
        if not check or check.IsZombie() or not check.Get('T') or not check.Get('runT'):
            raise RuntimeError(f'Not a readable full RAT DS file: {filename}')
        count = check.Get('T').GetEntries()
        check.Close()
        if not count:
            print(f'Skipping empty file: {filename}', flush=True)
            continue
        print(f'Reading {filename}', flush=True)
        reader = ROOT.RAT.DU.DSReader(filename)
        for index in range(reader.GetEntryCount()):
            if args.max_events is not None and read >= args.max_events:
                break
            ds = reader.GetEntry(index)
            read += 1
            mc = ds.GetMC()
            if mc.GetMCParticleCount() != 1 or mc.GetMCParticle(0).GetPDGCode() != 11:
                raise RuntimeError('Expected single-primary electron MC')
            particle = mc.GetMCParticle(0)
            energy = particle.GetKineticEnergy()
            radius = particle.GetPosition().Mag()
            if not (args.emin <= energy < args.emax) or radius > args.rmax:
                continue
            if not ds.GetEVCount():
                no_trigger += 1
                continue
            ev = ds.GetEV(0)
            h, nsat, selected = ROOT.NhitsStudy.corrected(ev)
            nhits = ev.GetNhits()
            raw.Fill(energy, nhits)
            corrected.Fill(energy, h)
            saturation.Fill(energy, float(nsat > 0))
            for key, value in zip(values, (energy, radius, nhits, h, nsat, selected)):
                values[key][0] = value
            events.Fill()
            accepted += 1
            saturated_events += nsat > 0
        del reader
    if not accepted:
        output.Close()
        raise RuntimeError('No triggered electron events passed the energy/radius cuts')

    with open(str(prefix) + '.csv', 'w', newline='') as handle:
        writer = csv.writer(handle)
        writer.writerow(['energy_low_MeV', 'energy_high_MeV', 'events',
                         'raw_mean', 'raw_sem', 'segmentor_mean', 'segmentor_sem',
                         'saturated_event_fraction'])
        for i in range(1, args.bins + 1):
            writer.writerow([raw.GetXaxis().GetBinLowEdge(i),
                             raw.GetXaxis().GetBinUpEdge(i), raw.GetBinEntries(i),
                             raw.GetBinContent(i), raw.GetBinError(i),
                             corrected.GetBinContent(i), corrected.GetBinError(i),
                             saturation.GetBinContent(i)])
    canvas = ROOT.TCanvas('nhits_canvas', 'Hits versus true energy', 1000, 700)
    canvas.SetLeftMargin(0.12)
    canvas.SetGrid()
    raw.SetTitle(';True Electron Kinetic Energy [MeV];Mean Hits')
    raw.GetYaxis().SetTitleOffset(1.35)
    raw.SetLineColor(ROOT.kBlue + 1)
    raw.SetMarkerColor(ROOT.kBlue + 1)
    raw.SetMarkerStyle(20)
    corrected.SetLineColor(ROOT.kRed + 1)
    corrected.SetMarkerColor(ROOT.kRed + 1)
    corrected.SetMarkerStyle(21)
    raw.SetMinimum(0)
    # Leave room above the data for the legend, including narrow test samples.
    ymax = max(p.GetBinContent(i) + p.GetBinError(i)
               for p in (raw, corrected) for i in range(1, args.bins + 1))
    raw.SetMaximum(1.45 * ymax)
    raw.Draw('E1')
    corrected.Draw('E1 SAME')
    legend = ROOT.TLegend(0.15, 0.72, 0.62, 0.89)
    legend.SetBorderSize(0)
    legend.AddEntry(raw, 'Raw Nhits', 'lp')
    legend.AddEntry(corrected, 'Segmentor-Corrected Hits (H)', 'lp')
    legend.Draw()
    for extension in ('png', 'pdf'):
        canvas.SaveAs(str(prefix) + '.' + extension)
    output.cd()
    events.Write()
    for profile in profiles:
        profile.Write()
    canvas.Write()
    ROOT.TNamed('definition', __doc__).Write()
    output.Close()
    print(f'Read {read}; plotted {accepted}; no trigger within cuts: {no_trigger}; '
          f'events with saturated segments: {saturated_events} '
          f'({saturated_events / accepted:.2%})')
    print(f'Saved {prefix}.{{png,pdf,root,csv}}')


if __name__ == '__main__':
    main()
