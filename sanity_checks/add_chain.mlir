module {
  llvm.func @add_chain() -> i32 {
    %neg = llvm.mlir.constant(-2 : i32) : i32
    %one = llvm.mlir.constant(1 : i32) : i32
    %pos = llvm.mlir.constant(4 : i32) : i32

    %first = llvm.add %neg, %one : i32
    %second = llvm.add %first, %pos : i32

    llvm.return %second : i32
  }
}
