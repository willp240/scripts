#!/bin/bash

# Loop over all directories starting with "th"
for thdir in th*; do
    # Only act if it's a directory
    if [ -d "$thdir" ]; then
        echo "Processing $thdir"

        # Create the new cfg subdirectory
        mkdir -p "$thdir/cfg"

        # Copy the contents of both cfg_dm... directories into cfg
        cp -r "$thdir"/cfg_dm0.00003002-0.00006986/* "$thdir/cfg/"
        cp -r "$thdir"/cfg_dm0.00007002-0.00010986/* "$thdir/cfg/"
    fi
done
