#!/bin/sh

bril_file="$1"
base_name="$(basename "$bril_file" .bril)"
json_file="$base_name.json"
executable="$2"
graphviz="$base_name.pdf"

bril2json < "$bril_file" > "$json_file" 
"$executable" "$json_file" | dot -Tpdf -o "$graphviz"
rm "$json_file"