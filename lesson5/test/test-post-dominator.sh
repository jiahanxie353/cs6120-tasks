#!/bin/bash

executable="$1"

# Define the base paths for the tests
base_files=("../../tools/visualization/loopy" "../../tools/visualization/lesson5-example" "../../tools/visualization/fib")
entrances=("main" "main" "main")
pairs=("exit exit elif elif loop entry right loop if end.elif end.elif then right left" "loop entry endif body endif then loop then" "done done loop body loop b0 body body")

# Convert .bril files to .json
for base in "${base_files[@]}"; do
    bril2json < "${base}.bril" > "${base}.json"
done

# Execute the tests
for idx in $(seq 0 $((${#base_files[@]} - 1))); do
    json_file="${base_files[$idx]}.json"
    
    # Get the pairs of dominator and dominatee for this json
    current_pairs=(${pairs[$idx]})
    
    # If pairs is not even number, then there's a mismatch in pairs.
    if [ $((${#current_pairs[@]} % 2)) -ne 0 ]; then
        echo "Mismatch in dominator-dominatee pairs for $json_file"
        continue
    fi
    
    for ((i=0; i<${#current_pairs[@]}; i+=2)); do
        dominator=${current_pairs[$i]}
        dominatee=${current_pairs[$i+1]}
        echo "Running test with $json_file, ${entrances[$idx]}, $dominator, $dominatee"
        "$executable"  "$json_file" "${entrances[$idx]}" $dominator $dominatee
    done
done

# Clean up generated .json files
for base in "${base_files[@]}"; do
    rm -f "${base}.json"
done

