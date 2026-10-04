// Direct LLVM-dialect tests for abstract multiplication.
// Run manually with: ./run.sh test/multiply.mlir
module {
  llvm.func @multiplication_tests(%input: i32) -> i32 {
    %zero = llvm.mlir.constant(0 : i32) : i32
    %one = llvm.mlir.constant(1 : i32) : i32
    %two = llvm.mlir.constant(2 : i32) : i32
    %negative_one = llvm.mlir.constant(-1 : i32) : i32
    %negative_two = llvm.mlir.constant(-2 : i32) : i32

    // Zero annihilates every finite sign.
    %zero_times_positive = llvm.mul %zero, %two : i32
    %zero_times_negative = llvm.mul %zero, %negative_two : i32

    // One preserves the other operand.
    %one_times_positive = llvm.mul %one, %two : i32
    %one_times_negative = llvm.mul %one, %negative_two : i32

    // Sign combinations.
    %positive_times_positive = llvm.mul %two, %two : i32
    %negative_times_negative = llvm.mul %negative_two, %negative_two : i32
    %positive_times_negative = llvm.mul %two, %negative_two : i32

    // Chained multiplication.
    %chain_one = llvm.mul %negative_two, %negative_one : i32
    %chain_two = llvm.mul %chain_one, %negative_two : i32

    // An unknown operand makes the result unknown unless the other operand is
    // known zero and the analysis models that case explicitly.
    %unknown_product = llvm.mul %input, %two : i32

    llvm.return %unknown_product : i32
  }
}
