// Direct LLVM-dialect tests for signed greater-than comparisons.
// The result of llvm.icmp is an i1 Boolean: false is Zero and true is One.
// Run manually with: ./run.sh test/compare.mlir
module {
  llvm.func @signed_greater_than_tests(%input: i32) -> i1 {
    %zero = llvm.mlir.constant(0 : i32) : i32
    %one = llvm.mlir.constant(1 : i32) : i32
    %two = llvm.mlir.constant(2 : i32) : i32
    %negative_one = llvm.mlir.constant(-1 : i32) : i32

    // Provably true comparisons.
    %positive_gt_zero = llvm.icmp "sgt" %two, %zero : i32
    %zero_gt_negative = llvm.icmp "sgt" %zero, %negative_one : i32
    %positive_gt_negative = llvm.icmp "sgt" %one, %negative_one : i32

    // Provably false comparisons.
    %negative_gt_zero = llvm.icmp "sgt" %negative_one, %zero : i32
    %zero_gt_positive = llvm.icmp "sgt" %zero, %one : i32

    // The signs alone do not determine these comparisons.
    %positive_gt_positive = llvm.icmp "sgt" %one, %two : i32
    %unknown_gt_one = llvm.icmp "sgt" %input, %one : i32

    llvm.return %positive_gt_zero : i1
  }
}
