// Direct LLVM-dialect tests for abstract subtraction.
// Run manually with: ./run.sh test/subtract.mlir
module {
  llvm.func @subtraction_tests(%input: i32) -> i32 {
    %zero = llvm.mlir.constant(0 : i32) : i32
    %one = llvm.mlir.constant(1 : i32) : i32
    %two = llvm.mlir.constant(2 : i32) : i32
    %three = llvm.mlir.constant(3 : i32) : i32
    %negative_one = llvm.mlir.constant(-1 : i32) : i32
    %negative_two = llvm.mlir.constant(-2 : i32) : i32
    %negative_three = llvm.mlir.constant(-3 : i32) : i32

    // lhs - rhs = lhs + (-rhs), preserving lhs/rhs order.
    %zero_minus_negative = llvm.sub %zero, %negative_two : i32
    // 0 - (-2) = positive

    %zero_minus_positive = llvm.sub %zero, %two : i32
    // 0 - 2 = negative

    %one_minus_negative = llvm.sub %one, %negative_two : i32
    // 1 - (-2) = positive

    %one_minus_one = llvm.sub %one, %one : i32
    // 1 - 1 = zero

    %negative_minus_negative = llvm.sub %negative_two, %negative_three : i32
    // negative - negative = top

    %positive_minus_positive = llvm.sub %three, %two : i32
    // positive - positive = top

    %negative_minus_positive = llvm.sub %negative_two, %three : i32
    // negative - positive = negative

    %positive_minus_negative = llvm.sub %three, %negative_two : i32
    // positive - negative = positive

    %zero_minus_zero = llvm.sub %zero, %zero : i32
    // 0 - 0 = zero

    %chain_one = llvm.sub %negative_two, %one : i32
    // negative - one = negative
    %chain_two = llvm.sub %chain_one, %negative_one : i32
    // negative - negative = top

    %unknown_lhs = llvm.sub %input, %one : i32
    // unknown - one = top

    llvm.return %chain_two : i32
  }
}
