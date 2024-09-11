#!/bin/bash

# Directory containing your .cpp files
DIRECTORY="./test/unit"

# Find all .cpp files in the specified directory and its subdirectories
find "$DIRECTORY" -type f -name "*.cpp" | while read -r file; do
    echo "Processing $file..."
    # Replace #include <catch.hpp> with #include <catch2/catch_all.hpp>
    sed -i 's|#include <catch_all.hpp>|#include <catch2/catch_test_macros.hpp>|g' "$file"
done

echo "Replacement complete."
