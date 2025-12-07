import os
import re
import sys
import statistics
from glob import glob
from datetime import datetime

if len(sys.argv) < 2:
    print("Usage: python3 summarise_memory.py /path/to/logdir1 /path/to/logdir2 ...")
    sys.exit(1)

log_dirs = sys.argv[1:]

mem_re = re.compile(r"^\s*(\d+)\s*-\s*MemoryUsage of job", re.MULTILINE)
rss_re = re.compile(r"^\s*(\d+)\s*-\s*ResidentSetSize of job", re.MULTILINE)

# Timestamps from events 001 and 005
start_re = re.compile(r"^001 .*? (\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}) Job executing", re.MULTILINE)
end_re   = re.compile(r"^005 .*? (\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}) Job terminated", re.MULTILINE)

results = {}

for log_path in log_dirs:
    if not os.path.isdir(log_path):
        print(f"WARNING: '{log_path}' is not a directory, skipping.")
        continue

    basename = os.path.basename(os.path.abspath(log_path))
    parent = os.path.basename(os.path.dirname(os.path.abspath(log_path)))
    category = parent if basename.lower() == "log" else basename

    logfiles = sorted(glob(os.path.join(log_path, "*.log")))
    if not logfiles:
        print(f"WARNING: No .log files found in {log_path}")
        continue

    results[category] = {"mem": [], "rss": [], "runtime": []}

    for logfile in logfiles:
        with open(logfile, "r") as f:
            text = f.read()

        # Memory extraction
        mem_values = mem_re.findall(text)
        rss_values = rss_re.findall(text)

        if not mem_values:
            print(f"WARNING: No MemoryUsage found in {logfile}")
            continue

        mem_MB = int(mem_values[-1])
        rss_KB = int(rss_values[-1]) if rss_values else None

        # Time extraction
        start_match = start_re.search(text)
        end_match   = end_re.search(text)

        if start_match and end_match:
            t_start = datetime.strptime(start_match.group(1), "%Y-%m-%d %H:%M:%S")
            t_end   = datetime.strptime(end_match.group(1), "%Y-%m-%d %H:%M:%S")
            runtime_sec = (t_end - t_start).total_seconds()
        else:
            runtime_sec = None  # job failed or badly formatted log

        results[category]["mem"].append(mem_MB)
        results[category]["rss"].append(rss_KB)
        results[category]["runtime"].append(runtime_sec)

# ---- Output summary ----

print("\n=== Memory + Runtime Summary ===\n")

for category, vals in results.items():
    mems = vals["mem"]
    rsss = vals["rss"]
    runtimes = [rt for rt in vals["runtime"] if rt is not None]

    print(f"{category}")
    print(f"  Jobs: {len(mems)}")
    print(f"  MemoryUsage (MB): mean={statistics.mean(mems):.1f},  std={statistics.stdev(mems):.1f}")

    if all(rsss):
        rss_MB = [kb/1024 for kb in rsss]
        print(f"  RSS peak (MB):     mean={statistics.mean(rss_MB):.1f},  std={statistics.stdev(rss_MB):.1f}")

    if runtimes:
        mins = [rt/60 for rt in runtimes]
        print(f"  Runtime (min):     mean={statistics.mean(mins):.1f}, std={statistics.stdev(mins):.1f}")

    print()
