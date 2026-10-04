// Direct LLVM-dialect tests for abstract signed integer division.
// Run manually with: ./run.sh test/divide.mlir
module {
  llvm.func @division_tests(%input: i32) -> i32 {
    %zero = llvm.mlir.constant(0 : i32) : i32
    %one = llvm.mlir.constant(1 : i32) : i32
    %two = llvm.mlir.constant(2 : i32) : i32
    %six = llvm.mlir.constant(6 : i32) : i32
    %negative_one = llvm.mlir.constant(-1 : i32) : i32
    %negative_two = llvm.mlir.constant(-2 : i32) : i32
    %negative_six = llvm.mlir.constant(-6 : i32) : i32

    // Positive numerator and denominator.
    %six_div_two = llvm.sdiv %six, %two : i32
    %one_div_one = llvm.sdiv %one, %one : i32
    %one_div_positive = llvm.sdiv %one, %two : i32

    // Negative numerator and denominator.
    %negative_six_div_two = llvm.sdiv %negative_six, %two : i32
    %six_div_negative_two = llvm.sdiv %six, %negative_two : i32
    %negative_six_div_negative_two = llvm.sdiv %negative_six, %negative_two : i32

    // Zero numerator.
    %zero_div_positive = llvm.sdiv %zero, %two : i32
    %zero_div_negative = llvm.sdiv %zero, %negative_two : i32

    // Division by zero is undefined, so the abstract result should be Top.
    %positive_div_zero = llvm.sdiv %six, %zero : i32

    // An unknown numerator or denominator makes the result unknown.
    %unknown_numerator = llvm.sdiv %input, %two : i32
    %unknown_denominator = llvm.sdiv %six, %input : i32

    llvm.return %unknown_numerator : i32
  }
}
