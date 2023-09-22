#!/bin/bash

executable="$1"

# Define the base paths for the tests
base_files=("../../tools/visualization/loopy" "../../tools/visualization/lesson5-example" "../../tools/visualization/fib")
entrances=("main" "main" "main")
pairs=("exit loop right else end.elif if" "loop entry body endif" "done")

# Convert .bril files to .json
for base in "${base_files[@]}"; do
    bril2json < "${base}.bril" > "${base}.json"
done

# Execute the tests
for idx in $(seq 0 $((${#base_files[@]} - 1))); do
    json_file="${base_files[$idx]}.json"
    
    # Get the pairs of dominator and dominatee for this json
    current_pairs=(${pairs[$idx]})
    
    for ((i=0; i<${#current_pairs[@]}; i+=1)); do
        dominatee=${current_pairs[$i]}
        echo "Running test with $json_file, ${entrances[$idx]}, $dominatee"
        "$executable"  "$json_file" "${entrances[$idx]}" $dominatee
    done
done

# Clean up generated .json files
for base in "${base_files[@]}"; do
    rm -f "${base}.json"
done

