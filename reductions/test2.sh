#!/bin/bash
#Uses sqlite3.ll
script_dir="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"

mlir-translate --import-llvm "$1" > out.mlir
BUILD_DIR="$script_dir/../build" "$script_dir/../run.sh" out.mlir > out.txt

grep -E '%[0-9]+ is Positive' out.txt | grep -vqE "constant|argument:" #> interesting.txt

rc=$?
exit $rc
