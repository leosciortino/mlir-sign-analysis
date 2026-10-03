//===- SignAnalysis.cpp - Transfer functions ------------------------------===//
//
// The transfer function: given what is known about an operation's operands,
// state what is known about its results.  This file and SignDomain.h are the
// two to replace when building a different analysis; the rest of the project
// is scaffolding.
//
// There are deliberately only two rules here, one of each kind an analysis
// needs: one that introduces facts out of nothing (constants), and one that
// propagates facts it was given (`and`).  Everything else is unknown.  Adding
// a third rule should be a matter of adding a third `if`.
//
//===----------------------------------------------------------------------===//

#include "SignAnalysis.h"

#include "SignDomain.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/IR/Matchers.h"
#include <llvm/Support/LogicalResult.h>

using namespace mlir;

namespace Sign {

static SignState evalAdd(SignState lhs, SignState rhs);
static SignState evalNeg(SignState state);
void SignAnalysis::setToEntryState(SignLattice *lattice) {
  propagateIfChanged(lattice, lattice->join(SignState::top()));
}

static constexpr Kind addTable[8][8] = {
{Kind::Bottom, Kind::Bottom, Kind::Bottom, Kind::Bottom, Kind::Bottom, Kind::Bottom, Kind::Bottom, Kind::Bottom}
};
LogicalResult
SignAnalysis::visitOperation(Operation *op,
                             ArrayRef<const SignLattice *> operands,
                             ArrayRef<SignLattice *> results) {
  // Raising a result to top says "this operation could produce anything",
  // which is always a sound answer and is what every unhandled case does.
  auto unknown = [&] {
    setAllToEntryStates(results);
    return success();
  };

  // Only single-result integer operations are interesting here.  Calls, loads,
  // floats, and vectors all land in `unknown`.
  if (op->getNumResults() != 1 || !op->getResult(0).getType().isIntOrIndex())
    return unknown();
  SignLattice *result = results[0];

  // Rule 1: a constant is Sign or nonSign according to what it says.
  // This is the only rule that does not consult its operands, and without some
  // rule of this kind the analysis would have no facts to propagate at all.
  IntegerAttr value;
  if (matchPattern(op, m_Constant(&value))) {
    SignState state;
    if (value.getValue().isOne()) {
      state = Kind::One;
    } else if (value.getValue().isZero()) {
      state = Kind::Zero;
    } else if (value.getValue().isStrictlyPositive()) {
      state = Kind::Positive;
    } else if (value.getValue().isNegative()) {
      state = Kind::Negative;
    } else {
      state = Kind::Top;
    }
    propagateIfChanged(result, result->join(state));
    return success();
  }

  // Rule 2: `x & y` is Sign if either operand is Sign, since a Sign operand
  // clears every bit.  Note what this rule does *not* say: two nonSign
  // operands tell us nothing, because 1 & 2 is 0.
  if (isa<LLVM::AddOp>(op)) {
    SignState lhs = operands[0]->getValue();
    SignState rhs = operands[1]->getValue();
    SignState tranfser_info = evalAdd(lhs, rhs);
    propagateIfChanged(result, result->join(tranfser_info));
    return success();
  }

  if (isa<LLVM::SubOp>(op)) {
    SignState lhs = operands[0]->getValue();
    SignState rhs = operands[1]->getValue();

    // using a - b = a + (-b)
    SignState tranfser_info = evalAdd(lhs, evalNeg(rhs));
    propagateIfChanged(result, result->join(tranfser_info));
    return success();
  }

  return unknown();
}

static bool check_symmetric(SignState lhs, SignState rhs,
                           Kind first, Kind second) {
  return (lhs.kind == first && rhs.kind == second) ||
         (lhs.kind == second && rhs.kind == first);
}

static SignState evalAdd(SignState lhs, SignState rhs) {
  if (lhs == Kind::Bottom || rhs == Kind::Bottom) {
    return Kind::Bottom;
  }
  if (lhs == Kind::Top || rhs == Kind::Top) {
    return Kind::Top;
  }

  // 1 + 1 = +
  if (check_symmetric(lhs, rhs, Kind::One, Kind::One)) {
    return Kind::Positive;
  }

  //1 + + = +
  if (check_symmetric(lhs, rhs, Kind::One, Kind::Positive)) {
    return Kind::Positive;
  }

  // 1 + 0 = 1
  if (check_symmetric(lhs, rhs, Kind::One, Kind::Zero)) {
    return Kind::One;
  }

  // 1 + - = 0-
  if (check_symmetric(lhs, rhs, Kind::One, Kind::Negative)) {
    return Kind::ZeroNegative;
  }

  // 1 + 0+ = +
  if (check_symmetric(lhs, rhs, Kind::One, Kind::ZeroPositive)) {
    return Kind::Positive;
  }

  // 1 + 0- = top
  if (check_symmetric(lhs, rhs, Kind::One, Kind::ZeroNegative)) {
    return Kind::Top;
  }

  // + + +  = +
  if (check_symmetric(lhs, rhs, Kind::Positive, Kind::Positive)) {
    return Kind::Positive;
  }

  // + + 0 = +
  if (check_symmetric(lhs, rhs, Kind::Positive, Kind::Zero)) {
    return Kind::Positive;
  }

  // + + - = top
  if (check_symmetric(lhs, rhs, Kind::Positive, Kind::Negative)) {
    return Kind::Top;
  }

  // + + 0+ = +
  if (check_symmetric(lhs, rhs, Kind::Positive, Kind::ZeroPositive)) {
    return Kind::Positive;
  }

  // + + 0- = top
  if (check_symmetric(lhs, rhs, Kind::Positive, Kind::ZeroNegative)) {
    return Kind::Top;
  }

  // 0 + 0 = 0
  if (check_symmetric(lhs, rhs, Kind::Zero, Kind::Zero)) {
    return Kind::Zero;
  }

  // 0 + - = -
  if (check_symmetric(lhs, rhs, Kind::Zero, Kind::Negative)) {
    return Kind::Negative;
  }

  // 0 + 0+ = 0+
  if (check_symmetric(lhs, rhs, Kind::Zero, Kind::ZeroPositive)) {
    return Kind::ZeroPositive;
  }

  // 0 + 0- = 0-
  if (check_symmetric(lhs, rhs, Kind::Zero, Kind::ZeroNegative)) {
    return Kind::ZeroNegative;
  }

  // - + - = -
  if (check_symmetric(lhs, rhs, Kind::Negative, Kind::Negative)) {
    return Kind::Negative;
  }

  // - + 0+ = Top
  if (check_symmetric(lhs, rhs, Kind::Negative, Kind::ZeroPositive)) {
    return Kind::Top;
  }

  // - + 0- = -
  if (check_symmetric(lhs, rhs, Kind::Negative, Kind::ZeroNegative)) {
    return Kind::Negative;
  }

  // 0+ + 0+ = 0+
  if (check_symmetric(lhs, rhs, Kind::ZeroPositive, Kind::ZeroPositive)) {
    return Kind::ZeroPositive;
  }

  //0+ + 0- = top
  if (check_symmetric(lhs, rhs, Kind::ZeroPositive, Kind::ZeroNegative)) {
    return Kind::Top;
  }

  // 0- + 0- = 0-
  if (check_symmetric(lhs, rhs, Kind::ZeroNegative, Kind::ZeroNegative)) {
    return Kind::ZeroNegative;
  }

  return Kind::Top;

}

static SignState evalNeg(SignState state) {
  switch (state.kind) {
  case Kind::Bottom:
    return Kind::Bottom;
  case Kind::Top:
    return Kind::Top;
  case Kind::Negative:
    return Kind::Positive;
  case Kind::Zero:
    return Kind::Zero;
  case Kind::Positive:
    return Kind::Negative;
  case Kind::One:
    return Kind::Negative;
  case Kind::ZeroNegative: // negative or zero
    return Kind::ZeroPositive;
  case Kind::ZeroPositive: // positive or zero
    return Kind::ZeroNegative;
  }

  return Kind::Top;
}
} // namespace Sign
