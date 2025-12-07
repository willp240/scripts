#!/usr/bin/env python3
import os
import ROOT

directory = "/data/snoplus3/parkerw/ratSimulations/Oct7_fastSFDelay_copy/"

for fname in os.listdir(directory):
    if not fname.endswith(".root"):
        continue

    full = os.path.join(directory, fname)
    print(f"Checking {full}...")

    # Try to open the ROOT file safely
    f = ROOT.TFile.Open(full)
    if not f or f.IsZombie():
        print("  -> Could not open file (zombie). Deleting.")
        os.remove(full)
        continue

    # Get the tree
    tree = f.Get("T")

    # Check if tree exists and has entries
    delete_file = False
    if not tree:
        print("  -> No TTree named 'T'. Deleting.")
        delete_file = True
    elif tree.GetEntries() == 0:
        print("  -> T has 0 entries. Deleting.")
        delete_file = True

    f.Close()

    # Delete if needed
    if delete_file:
        os.remove(full)
        print("  -> Deleted.")
    else:
        print("  -> OK.")

print("Done.")
