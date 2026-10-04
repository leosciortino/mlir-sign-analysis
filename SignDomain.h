//===- SignDomain.h - The abstract domain ---------------------------------===//
//
// Extended Sign lattice
//
//                         ⊤
//                       /   \
//                     0-     0+
//                    /  \   /  \
//                   -    0    +
//                    \   |   |
//                     \  |   1
//                      \ |  /
//                        ⊥
//
//===----------------------------------------------------------------------===//

#ifndef Sign_DOMAIN_H
#define Sign_DOMAIN_H

#include "llvm/Support/raw_ostream.h"

namespace Sign {

enum class Kind { Bottom, Negative, Zero, Positive, One, ZeroNegative, ZeroPositive, Top };

inline const char *name(Kind kind) {
  switch (kind) {
  case Kind::Bottom:
    return "bottom";
  case Kind::Negative:
    return "Negative";
  case Kind::Zero:
    return "Zero";
  case Kind::Positive:
    return "Positive";
  case Kind::ZeroNegative:
    return "ZeroNegative";
  case Kind::ZeroPositive:
    return "ZeroPositive";
  case Kind::One:
    return "One";
  case Kind::Top:
    return "top";
  }
  return "top";
}

struct SignState {
  Kind kind = Kind::Bottom;

  SignState() = default;
  /* implicit */ SignState(Kind kind) : kind(kind) {}

  static SignState bottom() { return Kind::Bottom; }
  static SignState top() { return Kind::Top; }

  bool isBottom() const { return kind == Kind::Bottom; }

  /// Least upper bound.  Two disagreeing facts lose all information.
  static SignState join(const SignState &lhs, const SignState &rhs) {
    if (lhs.kind == Kind::Bottom)
      return rhs;
    if (rhs.kind == Kind::Bottom)
      return lhs;
    if (lhs.kind == rhs.kind)
      return lhs;

    // join of 0, 1
    if (lhs == Kind::Zero && rhs == Kind::One){
      return Kind::ZeroPositive;
    }
    if (rhs == Kind::Zero && lhs == Kind::One){
      return Kind::ZeroPositive;
    }

    // Join of +, 1
    if (lhs == Kind::Positive && rhs == Kind::One) {
      return Kind::Positive;
    }
    if (rhs == Kind::Positive && lhs == Kind::One) {
      return Kind::Positive;
    }

    // Join of 1, 0+
    if (lhs == Kind::One && rhs == Kind::ZeroPositive) {
      return Kind::ZeroPositive;
    }
    if (rhs == Kind::One && lhs == Kind::ZeroPositive) {
      return Kind::ZeroPositive;
    }

    // Join of 0, +
    if (lhs == Kind::Zero && rhs == Kind::Positive) {
      return Kind::ZeroPositive;
    }
    if (rhs == Kind::Zero && lhs == Kind::Positive) {
      return Kind::ZeroPositive;
    }

    //Join of 0, 0+
    if (lhs == Kind::Zero && rhs == Kind::ZeroPositive) {
      return Kind::ZeroPositive;
    }
    if (rhs == Kind::Zero && lhs == Kind::ZeroPositive) {
      return Kind::ZeroPositive;
    }

    // Join of +, 0+
    if (lhs == Kind::Positive && rhs == Kind::ZeroPositive) {
      return Kind::ZeroPositive;
    }
    if (rhs == Kind::Positive && lhs == Kind::ZeroPositive) {
      return Kind::ZeroPositive;
    }

    // Join of 1, 0
    if (lhs == Kind::One && rhs == Kind::Zero) {
      return Kind::ZeroPositive;
    }
    if (rhs == Kind::One && lhs == Kind::Zero) {
      return Kind::ZeroPositive;
    }

    //Join of -, 0
    if (lhs == Kind::Negative && rhs == Kind::Zero) {
      return Kind::ZeroNegative;
    }
    if (rhs == Kind::Negative && lhs == Kind::Zero) {
      return Kind::ZeroNegative;
    }

    // Join of -, 0-
    if (lhs == Kind::Negative && rhs == Kind::ZeroNegative) {
      return Kind::ZeroNegative;
    }
    if (rhs == Kind::Negative && lhs == Kind::ZeroNegative) {
      return Kind::ZeroNegative;
    }



    return top();
  }

  bool operator==(const SignState &other) const { return kind == other.kind; }
  bool operator!=(const SignState &other) const { return kind != other.kind; }

  void print(llvm::raw_ostream &os) const { os << name(kind); }
};

inline llvm::raw_ostream &operator<<(llvm::raw_ostream &os,
                                     const SignState &state) {
  state.print(os);
  return os;
}

} // namespace Sign

#endif
