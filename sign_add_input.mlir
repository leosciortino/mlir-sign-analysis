module attributes {dlti.dl_spec = #dlti.dl_spec<i64 = dense<64> : vector<2xi64>, i128 = dense<128> : vector<2xi64>, !llvm.ptr = dense<64> : vector<4xi64>, i1 = dense<8> : vector<2xi64>, i8 = dense<8> : vector<2xi64>, i16 = dense<16> : vector<2xi64>, i32 = dense<32> : vector<2xi64>, f16 = dense<16> : vector<2xi64>, f64 = dense<64> : vector<2xi64>, f128 = dense<128> : vector<2xi64>, "dlti.endianness" = "little", "dlti.mangling_mode" = "o", "dlti.legal_int_widths" = array<i32: 32, 64>, "dlti.stack_alignment" = 128 : i64>, llvm.ident = "Apple clang version 15.0.0 (clang-1500.3.9.4)", llvm.module_asm = [], llvm.target_triple = "arm64-apple-macosx15.0.0"} {
  llvm.module_flags [#llvm.mlir.module_flag<error, "wchar_size", 4 : i32>, #llvm.mlir.module_flag<min, "PIC Level", 2 : i32>, #llvm.mlir.module_flag<max, "uwtable", 1 : i32>, #llvm.mlir.module_flag<max, "frame-pointer", 1 : i32>]
  llvm.func @one_plus_negative() -> i32 attributes {frame_pointer = #llvm.framePointerKind<"non-leaf">, no_inline, no_unwind, optimize_none, passthrough = ["ssp", ["min-legal-vector-width", "0"], ["no-trapping-math", "true"], ["probe-stack", "__chkstk_darwin"], ["stack-protector-buffer-size", "8"], ["target-cpu", "apple-m1"]], target_cpu = "apple-m1", target_features = #llvm.target_features<["+aes", "+crc", "+crypto", "+dotprod", "+fp-armv8", "+fp16fml", "+fullfp16", "+lse", "+neon", "+ras", "+rcpc", "+rdm", "+sha2", "+sha3", "+sm4", "+v8.1a", "+v8.2a", "+v8.3a", "+v8.4a", "+v8.5a", "+v8a", "+zcm", "+zcz"]>, uwtable_kind = #llvm.uwtableKind<sync>} {
    %0 = llvm.mlir.constant(1 : i32) : i32
    %1 = llvm.mlir.constant(-2 : i32) : i32
    %2 = llvm.alloca %0 x i32 {alignment = 4 : i64} : (i32) -> !llvm.ptr
    %3 = llvm.alloca %0 x i32 {alignment = 4 : i64} : (i32) -> !llvm.ptr
    llvm.store %0, %2 {alignment = 4 : i64} : i32, !llvm.ptr
    llvm.store %1, %3 {alignment = 4 : i64} : i32, !llvm.ptr
    %4 = llvm.load %2 {alignment = 4 : i64} : !llvm.ptr -> i32
    %5 = llvm.load %3 {alignment = 4 : i64} : !llvm.ptr -> i32
    %6 = llvm.add %4, %5 overflow<nsw> : i32
    llvm.return %6 : i32
  }
  llvm.func @one_plus_zero() -> i32 attributes {frame_pointer = #llvm.framePointerKind<"non-leaf">, no_inline, no_unwind, optimize_none, passthrough = ["ssp", ["min-legal-vector-width", "0"], ["no-trapping-math", "true"], ["probe-stack", "__chkstk_darwin"], ["stack-protector-buffer-size", "8"], ["target-cpu", "apple-m1"]], target_cpu = "apple-m1", target_features = #llvm.target_features<["+aes", "+crc", "+crypto", "+dotprod", "+fp-armv8", "+fp16fml", "+fullfp16", "+lse", "+neon", "+ras", "+rcpc", "+rdm", "+sha2", "+sha3", "+sm4", "+v8.1a", "+v8.2a", "+v8.3a", "+v8.4a", "+v8.5a", "+v8a", "+zcm", "+zcz"]>, uwtable_kind = #llvm.uwtableKind<sync>} {
    %0 = llvm.mlir.constant(1 : i32) : i32
    %1 = llvm.mlir.constant(0 : i32) : i32
    %2 = llvm.alloca %0 x i32 {alignment = 4 : i64} : (i32) -> !llvm.ptr
    %3 = llvm.alloca %0 x i32 {alignment = 4 : i64} : (i32) -> !llvm.ptr
    llvm.store %0, %2 {alignment = 4 : i64} : i32, !llvm.ptr
    llvm.store %1, %3 {alignment = 4 : i64} : i32, !llvm.ptr
    %4 = llvm.load %2 {alignment = 4 : i64} : !llvm.ptr -> i32
    %5 = llvm.load %3 {alignment = 4 : i64} : !llvm.ptr -> i32
    %6 = llvm.add %4, %5 overflow<nsw> : i32
    llvm.return %6 : i32
  }
  llvm.func @negative_plus_negative() -> i32 attributes {frame_pointer = #llvm.framePointerKind<"non-leaf">, no_inline, no_unwind, optimize_none, passthrough = ["ssp", ["min-legal-vector-width", "0"], ["no-trapping-math", "true"], ["probe-stack", "__chkstk_darwin"], ["stack-protector-buffer-size", "8"], ["target-cpu", "apple-m1"]], target_cpu = "apple-m1", target_features = #llvm.target_features<["+aes", "+crc", "+crypto", "+dotprod", "+fp-armv8", "+fp16fml", "+fullfp16", "+lse", "+neon", "+ras", "+rcpc", "+rdm", "+sha2", "+sha3", "+sm4", "+v8.1a", "+v8.2a", "+v8.3a", "+v8.4a", "+v8.5a", "+v8a", "+zcm", "+zcz"]>, uwtable_kind = #llvm.uwtableKind<sync>} {
    %0 = llvm.mlir.constant(1 : i32) : i32
    %1 = llvm.mlir.constant(-2 : i32) : i32
    %2 = llvm.mlir.constant(-3 : i32) : i32
    %3 = llvm.alloca %0 x i32 {alignment = 4 : i64} : (i32) -> !llvm.ptr
    %4 = llvm.alloca %0 x i32 {alignment = 4 : i64} : (i32) -> !llvm.ptr
    llvm.store %1, %3 {alignment = 4 : i64} : i32, !llvm.ptr
    llvm.store %2, %4 {alignment = 4 : i64} : i32, !llvm.ptr
    %5 = llvm.load %3 {alignment = 4 : i64} : !llvm.ptr -> i32
    %6 = llvm.load %4 {alignment = 4 : i64} : !llvm.ptr -> i32
    %7 = llvm.add %5, %6 overflow<nsw> : i32
    llvm.return %7 : i32
  }
  llvm.func @positive_plus_positive() -> i32 attributes {frame_pointer = #llvm.framePointerKind<"non-leaf">, no_inline, no_unwind, optimize_none, passthrough = ["ssp", ["min-legal-vector-width", "0"], ["no-trapping-math", "true"], ["probe-stack", "__chkstk_darwin"], ["stack-protector-buffer-size", "8"], ["target-cpu", "apple-m1"]], target_cpu = "apple-m1", target_features = #llvm.target_features<["+aes", "+crc", "+crypto", "+dotprod", "+fp-armv8", "+fp16fml", "+fullfp16", "+lse", "+neon", "+ras", "+rcpc", "+rdm", "+sha2", "+sha3", "+sm4", "+v8.1a", "+v8.2a", "+v8.3a", "+v8.4a", "+v8.5a", "+v8a", "+zcm", "+zcz"]>, uwtable_kind = #llvm.uwtableKind<sync>} {
    %0 = llvm.mlir.constant(1 : i32) : i32
    %1 = llvm.mlir.constant(2 : i32) : i32
    %2 = llvm.mlir.constant(3 : i32) : i32
    %3 = llvm.alloca %0 x i32 {alignment = 4 : i64} : (i32) -> !llvm.ptr
    %4 = llvm.alloca %0 x i32 {alignment = 4 : i64} : (i32) -> !llvm.ptr
    llvm.store %1, %3 {alignment = 4 : i64} : i32, !llvm.ptr
    llvm.store %2, %4 {alignment = 4 : i64} : i32, !llvm.ptr
    %5 = llvm.load %3 {alignment = 4 : i64} : !llvm.ptr -> i32
    %6 = llvm.load %4 {alignment = 4 : i64} : !llvm.ptr -> i32
    %7 = llvm.add %5, %6 overflow<nsw> : i32
    llvm.return %7 : i32
  }
  llvm.func @negative_plus_positive() -> i32 attributes {frame_pointer = #llvm.framePointerKind<"non-leaf">, no_inline, no_unwind, optimize_none, passthrough = ["ssp", ["min-legal-vector-width", "0"], ["no-trapping-math", "true"], ["probe-stack", "__chkstk_darwin"], ["stack-protector-buffer-size", "8"], ["target-cpu", "apple-m1"]], target_cpu = "apple-m1", target_features = #llvm.target_features<["+aes", "+crc", "+crypto", "+dotprod", "+fp-armv8", "+fp16fml", "+fullfp16", "+lse", "+neon", "+ras", "+rcpc", "+rdm", "+sha2", "+sha3", "+sm4", "+v8.1a", "+v8.2a", "+v8.3a", "+v8.4a", "+v8.5a", "+v8a", "+zcm", "+zcz"]>, uwtable_kind = #llvm.uwtableKind<sync>} {
    %0 = llvm.mlir.constant(1 : i32) : i32
    %1 = llvm.mlir.constant(-2 : i32) : i32
    %2 = llvm.mlir.constant(3 : i32) : i32
    %3 = llvm.alloca %0 x i32 {alignment = 4 : i64} : (i32) -> !llvm.ptr
    %4 = llvm.alloca %0 x i32 {alignment = 4 : i64} : (i32) -> !llvm.ptr
    llvm.store %1, %3 {alignment = 4 : i64} : i32, !llvm.ptr
    llvm.store %2, %4 {alignment = 4 : i64} : i32, !llvm.ptr
    %5 = llvm.load %3 {alignment = 4 : i64} : !llvm.ptr -> i32
    %6 = llvm.load %4 {alignment = 4 : i64} : !llvm.ptr -> i32
    %7 = llvm.add %5, %6 overflow<nsw> : i32
    llvm.return %7 : i32
  }
  llvm.func @main() -> i32 attributes {frame_pointer = #llvm.framePointerKind<"non-leaf">, no_inline, no_unwind, optimize_none, passthrough = ["ssp", ["min-legal-vector-width", "0"], ["no-trapping-math", "true"], ["probe-stack", "__chkstk_darwin"], ["stack-protector-buffer-size", "8"], ["target-cpu", "apple-m1"]], target_cpu = "apple-m1", target_features = #llvm.target_features<["+aes", "+crc", "+crypto", "+dotprod", "+fp-armv8", "+fp16fml", "+fullfp16", "+lse", "+neon", "+ras", "+rcpc", "+rdm", "+sha2", "+sha3", "+sm4", "+v8.1a", "+v8.2a", "+v8.3a", "+v8.4a", "+v8.5a", "+v8a", "+zcm", "+zcz"]>, uwtable_kind = #llvm.uwtableKind<sync>} {
    %0 = llvm.mlir.constant(1 : i32) : i32
    %1 = llvm.mlir.constant(0 : i32) : i32
    %2 = llvm.alloca %0 x i32 {alignment = 4 : i64} : (i32) -> !llvm.ptr
    llvm.store %1, %2 {alignment = 4 : i64} : i32, !llvm.ptr
    %3 = llvm.call @one_plus_negative() : () -> i32
    %4 = llvm.call @one_plus_zero() : () -> i32
    %5 = llvm.add %3, %4 overflow<nsw> : i32
    %6 = llvm.call @negative_plus_negative() : () -> i32
    %7 = llvm.add %5, %6 overflow<nsw> : i32
    %8 = llvm.call @positive_plus_positive() : () -> i32
    %9 = llvm.add %7, %8 overflow<nsw> : i32
    %10 = llvm.call @negative_plus_positive() : () -> i32
    %11 = llvm.add %9, %10 overflow<nsw> : i32
    llvm.return %11 : i32
  }
}
