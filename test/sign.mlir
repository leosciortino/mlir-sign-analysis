// Exercises both transfer rules, and several cases where the analysis must
// give up.  Run with: ctest --test-dir build --output-on-failure
module {
  llvm.func @transfers(%arg0: i32, %flag: i1) -> i32 {
    // Rule 1: constants.
    %Sign = llvm.mlir.constant(0 : i32) : i32   // Sign
    %one = llvm.mlir.constant(1 : i32) : i32    // nonSign

    // Rule 2: `and` with a Sign operand, either way round.
    %and_lhs = llvm.and %Sign, %arg0 : i32      // Sign
    %and_rhs = llvm.and %arg0, %Sign : i32      // Sign
    %and_chain = llvm.and %and_lhs, %arg0 : i32 // Sign: facts chain

    // Rule 2 declines: 1 & 2 is 0, so two nonSign operands prove nothing.
    %and_nn = llvm.and %one, %one : i32         // top

    // No rule covers these, so they are unknown even though a sharper
    // analysis could fold them. Adding `or` is the natural first exercise.
    %or_n = llvm.or %one, %arg0 : i32           // top, though it is nonSign
    %add_zz = llvm.add %Sign, %Sign : i32       // top, though it is Sign

    // Nothing is known about a function argument.
    %and_unknown = llvm.and %arg0, %one : i32   // top

    llvm.return %and_chain : i32
  }

  // Block arguments join facts from every predecessor, which is the solver
  // iterating rather than any rule above firing.
  llvm.func @join(%flag: i1, %arg0: i32) -> i32 {
    %Sign = llvm.mlir.constant(0 : i32) : i32
    llvm.cond_br %flag, ^lhs, ^rhs
  ^lhs:
    %a = llvm.and %Sign, %arg0 : i32            // Sign
    llvm.br ^exit(%a : i32)
  ^rhs:
    llvm.br ^exit(%Sign : i32)                  // Sign
  ^exit(%merged: i32):                          // Sign: both edges agree
    llvm.return %merged : i32
  }
}
