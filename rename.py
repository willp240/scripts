#!/usr/bin/env python3

import sys
from pathlib import Path

def add_suffix(directory: Path, suffix: str):
    for png in directory.glob("*.png"):
        # Skip any file that already has an underscore in the name
        if "_" in png.stem:
            continue

        new_name = png.with_name(f"{png.stem}_{suffix}{png.suffix}")
        png.rename(new_name)
        print(f"{png.name} -> {new_name.name}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python add_suffix_to_pngs.py <directory> <suffix>")
        sys.exit(1)

    directory = Path(sys.argv[1]).expanduser().resolve()
    suffix = sys.argv[2]

    if not directory.is_dir():
        print(f"Error: {directory} is not a directory")
        sys.exit(1)

    add_suffix(directory, suffix)
