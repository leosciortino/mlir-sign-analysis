// C input for exercising abstract sign addition.
// Lower without optimization so the additions remain visible in LLVM IR:
//   clang -O0 -S -emit-llvm sign_add_input.c -o - \
//     | mlir-translate --import-llvm > sign_add_input.mlir

int one_plus_negative(void) {
  int one = 1;
  int negative = -2;
  return one + negative; // 0- (zero or negative)
}

int one_plus_zero(void) {
  int one = 1;
  int zero = 0;
  return one + zero; // 1
}

int negative_plus_negative(void) {
  int lhs = -2;
  int rhs = -3;
  return lhs + rhs; // -
}

int positive_plus_positive(void) {
  int lhs = 2;
  int rhs = 3;
  return lhs + rhs; // +
}

int negative_plus_positive(void) {
  int lhs = -2;
  int rhs = 3;
  return lhs + rhs; // top: may be negative, zero, or positive
}

int main(void) {
  return one_plus_negative() + one_plus_zero() +
         negative_plus_negative() + positive_plus_positive() +
         negative_plus_positive();
}
