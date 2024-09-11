#!/bin/bash

# Loop over all files in the directory
for file in ./test/unit/*.cpp; do
    # Check if the file contains both includes
    if grep -q '#include <catch2/catch_approx.hpp>' "$file" && grep -q '#include <catch2/catch_all.hpp>' "$file"; then
        # Get the line numbers for both includes
        approx_line=$(grep -n '#include <catch2/catch_approx.hpp>' "$file" | cut -d: -f1)
        all_line=$(grep -n '#include <catch2/catch_all.hpp>' "$file" | cut -d: -f1)
        
        # If approx include is before all include, reorder them
        if [ "$approx_line" -lt "$all_line" ]; then
            echo "Reordering includes in $file"
            # Delete both includes
            sed -i '/#include <catch2\/catch_approx.hpp>/d' "$file"
            sed -i '/#include <catch2\/catch_all.hpp>/d' "$file"
            
            # Insert them back in the correct order (all first, then approx)
            sed -i "$all_line i#include <catch2/catch_approx.hpp>" "$file"
            sed -i "$all_line i#include <catch2/catch_all.hpp>" "$file"
        else
            echo "Includes already in correct order in $file"
        fi
    fi
done
