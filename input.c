// Simple input for testing an integer sign analysis.

int known_positive(void) {
  int value = 42;
  return value;
}

int known_negative(void) {
  int value = -17;
  return value;
}

int known_Sign(void) {
  int value = 0;
  return value;
}

// The result depends on the caller-provided input, so it should initially be
// treated as having an unknown sign.  Each return statement has a known sign.
int classify_and_adjust(int input) {
  if (input > 0)
    return input + 1; // positive
  if (input < 0)
    return input - 1; // negative
  return 0;           // Sign
}

int mix_signs(int input) {
  int positive = 5;
  int negative = -3;
  int Sign = 0;

  if (input > Sign)
    return positive;
  return negative;
}

// Addition cases for the enriched sign domain.  Compile this file with -O0
// when you want to preserve the individual additions in the generated IR.

int add_zero_negative(void) {
  int zero = 0;
  int negative = -4;
  return zero + negative; // Negative
}

int add_zero_positive(void) {
  int zero = 0;
  int positive = 4;
  return zero + positive; // Positive
}

int add_one_one(void) {
  int lhs = 1;
  int rhs = 1;
  return lhs + rhs; // Positive
}

int add_one_negative(void) {
  int one = 1;
  int negative = -1;
  return one + negative; // ZeroNegative
}

int add_positive_negative(void) {
  int positive = 7;
  int negative = -3;
  return positive + negative; // Top in the abstract domain
}

int add_negative_negative(void) {
  int lhs = -7;
  int rhs = -3;
  return lhs + rhs; // Negative
}

int add_positive_positive(void) {
  int lhs = 7;
  int rhs = 3;
  return lhs + rhs; // Positive
}

int add_chain(void) {
  int negative = -2;
  int one = 1;
  int positive = 4;
  int first = negative + one;     // ZeroNegative
  int second = first + positive;  // Top
  return second;
}

int add_unknown_input(int input) {
  int one = 1;
  return input + one; // Top without a range/sign refinement for input
}

int add_many_terms(int input) {
  int zero = 0;
  int negative = -2;
  int positive = 5;
  return (((input + zero) + negative) + positive) + 1;
}
