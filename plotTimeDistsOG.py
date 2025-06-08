import ROOT
import sys

# File to read from (change if needed)
log_file = "/data/snoplus/parkerw/ratSimulations/May23_SFTime_og/output/merged.output"
output_file = "May23_SFTime_oghists.root"

# Categories to make histograms for
categories = ["Quad Seed", "MultiPDF", "Total"]
histos = {}

# Create one TH1D for each category
for cat in categories:
    histos[cat] = ROOT.TH1D(cat.replace(" ", "_"), cat, 500, 0, 0.05)  # binning can be changed

# Process the file
linecount = 0
skipped = 0

with open(log_file, "r") as f:
    for line in f:
        linecount += 1
        if linecount % 1_000_000 == 0:
            print(f"Processed {linecount:,} lines...")
            
        #if linecount > 600000000:
        #    break
            
        if ":" not in line:
            skipped += 1
            continue

        key, val = line.split(":", 1)
        key = key.strip()
        if key not in histos:
            continue

        try:
            value = float(val.strip())
            histos[key].Fill(value)
        except ValueError:
            skipped += 1

print(f"\nFinished. Total lines: {linecount:,}, Skipped: {skipped:,}")

# Save histograms to file
out = ROOT.TFile(output_file, "RECREATE")
for hist in histos.values():
    hist.Write()
out.Close()

print(f"Histograms written to {output_file}")
