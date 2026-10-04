My project is a modified version of an analysis written by Prof. John Regehr and can be found here https://github.com/regehr/mlir-analysis-pass

## Building

```sh
cmake -S . -B build
cmake --build build
```

To build against a specific MLIR instead:

```sh
cmake -S . -B build -DMLIR_DIR=/path/to/prefix/lib/cmake/mlir
```

## Running

```sh
./run.sh input.mlir
```

`run.sh` locates the plugin whatever it is called on your platform and puts the
annotated listing on stdout.

Get input in the LLVM dialect from C with:

```sh
clang -S -emit-llvm -o - input.c | mlir-translate --import-llvm
```

## Sign lattice
I implement the lattice from page 56 of Static Program Analysis by Anders Møller and Michael I. Schwartzbach.

                         ⊤
                       /   \
                     0-     0+
                    /  \   /  \
                   -    0    +
                    \   |   |
                     \  |   1
                      \ |  /
                        ⊥

I implement 6 transfer functions: +, -, *, /, >, ==

When compiling with clang -02 I do get interesting facts for +, *. The others seem to probably mostly being loaded values that can't be put as constants in the program.

Here is an example of a cool fact that it found. on line 11082 of the output:

^bb10(%94: i32, %95: !llvm.ptr):  // 2 preds: ^bb9, ^bb10
    // argument: %94 is ZeroPositive
    %96 = llvm.load %95 {alignment = 8 : i64, tbaa = [#tbaa_tag265]} : !llvm.ptr -> !llvm.ptr
    %97 = llvm.add %94, %0 : i32 // %97 is Positive

Where %0 is defined as one at the beginning of the function.

## Provided output
The following files are provided
+ sqlite-amalgamation-3530400/sqlite3.c
+ sqlite-amalgamation-3530400/sqlite3.ll
+ sqlite-amalgamation-3530400/sqlite3.mlir
+ sqlite-amalgamation-3530400/sqlite3_analysis.out

You can see in the analysis.out what the analysis concluded.

## Running sqlite
run the following to get the sqlite mlir, you may need to replace where your mac sdk is located if on linux you may not even need to include this flag.
```sh
clang -O2 -isysroot /Library/Developer/CommandLineTools/SDKs/MacOSX15.0.sdk -S -emit-llvm -o - sqlite-amalgamation-3530400/sqlite3.c | mlir-translate --import-llvm > sqlite-amalgamation-3530400/sqlite3.mlir
```

Then to run use 
```sh
./run.sh sqlite-amalgamation-3530400/sqlite3.mlir
```

## Sanity Checks
The mlir in the sanity-checks directory is code that codex wrote that helped me test as I went.