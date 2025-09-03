import os
import re
from datetime import datetime

# Directory containing .log files
log_dir = "/data/snoplus/parkerw/antinu/Sep1_2dsthetaprior/log"

start_pattern = re.compile(r"001 .*?(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})")
end_pattern = re.compile(r"005 .*?(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})")

def extract_time(line):
    match = re.search(r"\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}", line)
    return datetime.strptime(match.group(), "%Y-%m-%d %H:%M:%S") if match else None

for fname in sorted(os.listdir(log_dir)):
    if not fname.endswith(".log"):
        continue

    fpath = os.path.join(log_dir, fname)
    with open(fpath, "r") as f:
        lines = f.readlines()

    start_time = None
    end_time = None

    for line in lines:
        if line.startswith("001") and not start_time:
            start_time = extract_time(line)
        elif line.startswith("005"):
            end_time = extract_time(line)

    if start_time and end_time:
        duration = end_time - start_time
        print(f"{fname}: {duration}")
    else:
        print(f"{fname}: [missing time info]")
