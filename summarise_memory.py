import os
import re
import sys
import statistics
from glob import glob

if len(sys.argv) < 2:
    print("Usage: python3 summarise_memory.py /path/to/logdir1 /path/to/logdir2 ...")
    sys.exit(1)

log_dirs = sys.argv[1:]

mem_re = re.compile(r"^\s*(\d+)\s*-\s*MemoryUsage of job", re.MULTILINE)
rss_re = re.compile(r"^\s*(\d+)\s*-\s*ResidentSetSize of job", re.MULTILINE)

results = {}

for log_path in log_dirs:
    if not os.path.isdir(log_path):
        print(f"WARNING: '{log_path}' is not a directory, skipping.")
        continue

    # Category name = final directory component (e.g. "/x/y/e_notrack/log" → "log", so use parent)
    # If the directory is ".../e_notrack/log", we want "e_notrack"
    basename = os.path.basename(os.path.abspath(log_path))
    parent = os.path.basename(os.path.dirname(os.path.abspath(log_path)))

    # Prefer using parent directory name if directory is called "log"
    category = parent if basename.lower() == "log" else basename

    logfiles = sorted(glob(os.path.join(log_path, "*.log")))

    if not logfiles:
        print(f"WARNING: No .log files found in {log_path}")
        continue

    results[category] = {"mem": [], "rss": []}

    for logfile in logfiles:
        with open(logfile, "r") as f:
            text = f.read()

        mem_values = mem_re.findall(text)
        rss_values = rss_re.findall(text)

        if not mem_values:
            print(f"WARNING: No MemoryUsage found in {logfile}")
            continue

        mem_MB = int(mem_values[-1])
        rss_KB = int(rss_values[-1]) if rss_values else None

        results[category]["mem"].append(mem_MB)
        results[category]["rss"].append(rss_KB)

# ---- Output summary ----

print("\n=== Memory Summary ===\n")

for category, vals in results.items():
    mems = vals["mem"]
    rsss = vals["rss"]

    print(f"{category}")
    print(f"  Jobs: {len(mems)}")
    print(f"  MemoryUsage (MB): mean={statistics.mean(mems):.1f},  std={statistics.stdev(mems):.1f}")

    if all(rsss):
        rss_MB = [kb/1024 for kb in rsss]
        print(f"  RSS peak (MB):     mean={statistics.mean(rss_MB):.1f},  std={statistics.stdev(rss_MB):.1f}")

    print()
