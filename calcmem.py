import os
import re

def extract_memory_usage(filepath):
    with open(filepath, 'r') as f:
        for line in f:
            match = re.search(r'Memory\s+\(MB\)\s*:\s*(\d+)', line)
            if match:
                return int(match.group(1))
    return None

def main(directory):
    total_memory = 0
    count = 0

    for filename in os.listdir(directory):
        filepath = os.path.join(directory, filename)
        if os.path.isfile(filepath):
            mem = extract_memory_usage(filepath)
            if mem is not None:
                total_memory += mem
                count += 1
            else:
                print("no mem ", filepath)

    if count > 0:
        average = total_memory / count
        print(f"Found {count} files with memory usage data.")
        print(f"Total memory usage: {total_memory} MB")
        print(f"Average memory usage: {average:.2f} MB")
    else:
        print("No memory usage data found in any files.")

if __name__ == "__main__":
    import sys
    if len(sys.argv) < 2:
        print("Usage: python memory_avg.py <directory>")
    else:
        main(sys.argv[1])
