#!/bin/bash

# Loop over all files in the directory
for file in ./test/unit/*.cpp; do
    # Check if the file contains 'Catch::Approx'
    if ! grep -q "Catch::Approx" "$file"; then
        # If not, remove the '#include <catch2/catch_approx.hpp>' line
        sed -i '/#include <catch2\/catch_approx.hpp>/d' "$file"
        echo "Removed '#include <catch2/catch_approx.hpp>' from $file"
    else
        echo "'Catch::Approx' found in $file. No changes made."
    fi
done
