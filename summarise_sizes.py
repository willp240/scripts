import os
import sys
import statistics

if len(sys.argv) != 2:
    print("Usage: python3 summarise_root_sizes.py /path/to/directory")
    sys.exit(1)

directory = sys.argv[1]

if not os.path.isdir(directory):
    print(f"ERROR: '{directory}' is not a directory.")
    sys.exit(1)

# Find all .root files
root_files = [os.path.join(directory, f)
              for f in os.listdir(directory)
              if f.lower().endswith(".root")]

if not root_files:
    print("No .root files found.")
    sys.exit(0)

sizes = []
for f in root_files:
    size_bytes = os.path.getsize(f)
    size_mb = size_bytes / (1024 * 1024)
    sizes.append(size_mb)

print(f"\nFound {len(sizes)} ROOT files in {directory}")

print(f"Average size: {statistics.mean(sizes):.2f} MB")
print(f"Std dev:       {statistics.stdev(sizes):.2f} MB")

print("\nIndividual file sizes:")
for f, s in zip(root_files, sizes):
    print(f"  {os.path.basename(f)} : {s:.2f} MB")
