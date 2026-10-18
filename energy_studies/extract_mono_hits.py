#!/usr/bin/env python3
"""Extract monoenergetic hit summaries with verified shared run-0 conditions.

Uses the same H implementation as plot_nhits_vs_energy.py. Initialise DU
once per campaign, verifying stored DB contents/overrides and software
versions for every file before using that configuration. Saves provenance.
"""
import argparse
from array import array
import hashlib
import json
from pathlib import Path

from plot_nhits_vs_energy import HELPER

ENERGIES = (1, 2, 4, 6, 10, 15, 20, 25, 30, 40)


def signature(meta):
    data = [list(map(str, meta.GetVersions())), list(map(str, meta.GetGeant4Versions()))]
    for i in range(meta.GetPassCount()):
        db = meta.GetMetaDB(i)
        data.append([str(db.GetFile(j)) for j in range(db.GetFileCount())])
        data.append([(str(db.GetOverrideCommand(j).first), str(db.GetOverrideCommand(j).second))
                     for j in range(db.GetOverrideCommandCount())])
    return hashlib.sha256(json.dumps(data).encode()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('campaign')
    parser.add_argument('-o', '--output', required=True)
    parser.add_argument('--z', type=float, required=True, help='Expected source z in mm')
    parser.add_argument('--expected-per-energy', type=int, default=1000)
    args = parser.parse_args()
    import ROOT
    import rat
    ROOT.gROOT.SetBatch(True)
    assert ROOT.gInterpreter.Declare(HELPER)
    paths = []
    for energy in ENERGIES:
        files = sorted(p for p in (Path(args.campaign) / f'{energy}MeV').glob('*.root')
                       if not p.name.endswith('.ntuple.root'))
        if not files:
            raise ValueError(f'No full RAT files at {energy} MeV')
        paths.extend((energy, p.resolve()) for p in files)
    target = Path(args.output).resolve()
    if target in [p for _, p in paths]:
        raise ValueError('Output would overwrite input')
    target.parent.mkdir(parents=True, exist_ok=True)
    reference_reader = ROOT.RAT.DU.DSReader(str(paths[0][1]))
    out = ROOT.TFile(str(target), 'RECREATE')
    tree = ROOT.TTree('events', 'First trigger per generated electron')
    fields = ('true_energy', 'true_radius', 'true_x', 'true_y', 'true_z',
              'nhits', 'segmentor_h', 'saturated_segments', 'selected_hits',
              'mc_photoelectrons', 'source_file', 'source_entry', 'seed', 'ev_count')
    values = {name: array('d', [0.]) for name in fields}
    for name, buffer in values.items():
        tree.Branch(name, buffer, name + '/D')
    ref_signature = None
    counts = {e: dict(generated=0, triggered=0, saturated=0) for e in ENERGIES}
    for file_index, (energy, path) in enumerate(paths):
        source = ROOT.TFile.Open(str(path))
        if not source or source.IsZombie() or source.TestBit(ROOT.TFile.kRecovered):
            raise ValueError(f'Unfinished or corrupt file: {path}')
        meta = source.Get('meta')
        events = source.Get('T')
        if not meta or not events:
            raise ValueError(f'Missing metadata or event tree: {path}')
        current = signature(meta)
        if ref_signature is None:
            ref_signature = current
        if current != ref_signature:
            raise ValueError(f'File has different DB/software configuration: {path}')
        for entry_index, record in enumerate(events):
            ds = record.ds
            if ds.GetRunID() != 0:
                raise ValueError('This extractor expects homogeneous run-0 MC')
            mc = ds.GetMC()
            if mc.GetMCParticleCount() != 1 or mc.GetMCParticle(0).GetPDGCode() != 11:
                raise ValueError('Expected single-electron MC')
            primary = mc.GetMCParticle(0)
            pos = primary.GetPosition()
            if abs(primary.GetKineticEnergy() - energy) > 1e-6:
                raise ValueError(f'Unexpected true energy in {path}')
            if abs(pos.X()) > 1e-6 or abs(pos.Y()) > 1e-6 or abs(pos.Z() - args.z) > 1e-6:
                raise ValueError(f'Unexpected source position in {path}')
            counts[energy]['generated'] += 1
            if not ds.GetEVCount():
                continue
            ev = ds.GetEV(0)
            h, saturated, selected = ROOT.NhitsStudy.corrected(ev)
            row = (primary.GetKineticEnergy(), pos.Mag(), pos.X(), pos.Y(), pos.Z(),
                   ev.GetNhits(), h, saturated, selected, mc.GetMCPECount(),
                   file_index, entry_index, ds.GetSeed(), ds.GetEVCount())
            for name, value in zip(fields, row):
                values[name][0] = value
            tree.Fill()
            counts[energy]['triggered'] += 1
            counts[energy]['saturated'] += int(saturated > 0)
        source.Close()
        if (file_index + 1) % 100 == 0:
            print(f'{file_index + 1}/{len(paths)} files; {energy} MeV: {counts[energy]}', flush=True)
    for energy, stats in counts.items():
        if stats['generated'] != args.expected_per_energy:
            raise ValueError(f'{energy} MeV: expected {args.expected_per_energy}, found {stats}')
    out.cd()
    tree.Write()
    provenance = dict(campaign=str(Path(args.campaign).resolve()), z_mm=args.z,
                      files=[str(p) for _, p in paths], counts=counts,
                      metadata_sha256=ref_signature, definition=__doc__)
    ROOT.TNamed('provenance', json.dumps(provenance)).Write()
    out.Close()
    target.with_suffix('.json').write_text(json.dumps(provenance, indent=2) + '\n')
    print(f'Saved {target}', flush=True)


if __name__ == '__main__':
    main()
