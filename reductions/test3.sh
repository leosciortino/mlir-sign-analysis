#!/bin/bash
#Uses test3.ll
script_dir="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"

mlir-translate --import-llvm "$1" > out.mlir
BUILD_DIR="$script_dir/../build" "$script_dir/../run.sh" out.mlir > out.txt

# An equality comparison is interesting only when the sign analysis proves it
# true.  `One` is the abstract value for the true i1 result.
grep -E 'llvm\.icmp "eq".* is One$' out.txt > interesting.txt

rc=$?
exit $rc
