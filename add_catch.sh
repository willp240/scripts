#!/bin/bash

# Define the directory to search for .cpp files
DIRECTORY="./test/unit/"

# Loop over all .cpp files in the specified directory
for file in "$DIRECTORY"/*.cpp; do
    if [[ -f "$file" ]]; then
        # Check if the file contains the term "Approx"
        if grep -q "Catch::Catch::Catch::" "$file"; then
            # Replace "Approx" with "Catch::Approx"
            sed -i 's/\bCatch::Catch::Catch::\b/Catch::/g' "$file"
        fi
        
        # Check if the first line is already the include statement
        if ! grep -q '#include <catch2/catch_approx.hpp>' "$file"; then
            # Add the include statement to the beginning of the file
            sed -i '1s|^|#include <catch2/catch_approx.hpp>\n|' "$file"
        fi
    fi
done
