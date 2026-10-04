// Direct LLVM-dialect tests for signed integer equality.
// Run manually with: ./run.sh test/equal.mlir
module {
  llvm.func @equality_tests(%input: i32) -> i1 {
    %zero = llvm.mlir.constant(0 : i32) : i32
    %one = llvm.mlir.constant(1 : i32) : i32
    %two = llvm.mlir.constant(2 : i32) : i32
    %negative_one = llvm.mlir.constant(-1 : i32) : i32

    // Definitely true.
    %zero_eq_zero = llvm.icmp "eq" %zero, %zero : i32
    %one_eq_one = llvm.icmp "eq" %one, %one : i32

    // Definitely false because the concrete values differ or signs disagree.
    %one_eq_two = llvm.icmp "eq" %one, %two : i32
    %negative_eq_positive = llvm.icmp "eq" %negative_one, %one : i32
    %zero_eq_positive = llvm.icmp "eq" %zero, %one : i32

    // Same abstract sign, but equality cannot be determined from signs alone.
    %positive_eq_positive = llvm.icmp "eq" %one, %two : i32

    // Unknown operand.
    %unknown_eq_one = llvm.icmp "eq" %input, %one : i32

    llvm.return %one_eq_one : i1
  }
}
