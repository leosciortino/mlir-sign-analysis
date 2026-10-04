#include "SignAnalysis.h"

#include "SignDomain.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/IR/Matchers.h"
#include <llvm/Support/LogicalResult.h>

using namespace mlir;

namespace Sign {

static SignState evalAdd(SignState lhs, SignState rhs);
static SignState evalSub(SignState lhs, SignState rhs);
static SignState evalMul(SignState lhs, SignState rhs);
static SignState evalDiv(SignState lhs, SignState rhs);
static SignState evalGt(SignState lhs, SignState rhs);
static SignState evalEq(SignState lhs, SignState rhs);
void SignAnalysis::setToEntryState(SignLattice *lattice) {
  propagateIfChanged(lattice, lattice->join(SignState::top()));
}

// The row and column order is the order of Kind in SignDomain.h:
// Bottom, Negative, Zero, Positive, One, ZeroNegative, ZeroPositive, Top.
// Addition is commutative, so this table is symmetric.
static constexpr Kind addTable[8][8] = {
    //                  Bottom,             Negative,            Zero,                Positive
    //                  One,                ZeroNegative,        ZeroPositive,        Top
    /* Bottom */       {Kind::Bottom,       Kind::Bottom,        Kind::Bottom,        Kind::Bottom,
                        Kind::Bottom,       Kind::Bottom,        Kind::Bottom,        Kind::Bottom},
    /* Negative */     {Kind::Bottom,       Kind::Negative,      Kind::Negative,      Kind::Top,
                        Kind::ZeroNegative, Kind::Negative,      Kind::Top,           Kind::Top},
    /* Zero */         {Kind::Bottom,       Kind::Negative,      Kind::Zero,          Kind::Positive,
                        Kind::One,          Kind::ZeroNegative,  Kind::ZeroPositive,  Kind::Top},
    /* Positive */     {Kind::Bottom,       Kind::Top,           Kind::Positive,      Kind::Positive,
                        Kind::Positive,     Kind::Top,           Kind::Positive,      Kind::Top},
    /* One */          {Kind::Bottom,       Kind::ZeroNegative,  Kind::One,           Kind::Positive,
                        Kind::Positive,     Kind::Top,           Kind::Positive,      Kind::Top},
    /* ZeroNegative */ {Kind::Bottom,       Kind::Negative,      Kind::ZeroNegative,  Kind::Top,
                        Kind::Top,          Kind::ZeroNegative,  Kind::Top,           Kind::Top},
    /* ZeroPositive */ {Kind::Bottom,       Kind::Top,           Kind::ZeroPositive,  Kind::Positive,
                        Kind::Positive,     Kind::Top,           Kind::ZeroPositive,  Kind::Top},
    /* Top */          {Kind::Bottom,       Kind::Top,           Kind::Top,           Kind::Top,
                        Kind::Top,          Kind::Top,           Kind::Top,           Kind::Top},
};

static constexpr Kind subTable[8][8] = {
    //                  Bottom,             Negative,            Zero,                Positive
    //                  One,                ZeroNegative,        ZeroPositive,        Top
    /* Bottom */       {Kind::Bottom,       Kind::Bottom,        Kind::Bottom,        Kind::Bottom,
                        Kind::Bottom,       Kind::Bottom,        Kind::Bottom,        Kind::Bottom},
    /* Negative */     {Kind::Bottom,       Kind::Top,           Kind::Negative,      Kind::Negative,
                        Kind::Negative,     Kind::Top,           Kind::Negative,      Kind::Top},
    /* Zero */         {Kind::Bottom,       Kind::Positive,      Kind::Zero,          Kind::Negative,
                        Kind::Negative,     Kind::ZeroPositive,  Kind::ZeroNegative,  Kind::Top},
    /* Positive */     {Kind::Bottom,       Kind::Positive,      Kind::Positive,      Kind::Top,
                        Kind::ZeroPositive, Kind::Positive,      Kind::Top,           Kind::Top},
    /* One */          {Kind::Bottom,       Kind::Positive,      Kind::One,           Kind::ZeroNegative,
                        Kind::Zero,         Kind::Positive,      Kind::Top,           Kind::Top},
    /* ZeroNegative */ {Kind::Bottom,       Kind::Top,           Kind::ZeroNegative,  Kind::Negative,
                        Kind::Negative,     Kind::Top,           Kind::ZeroNegative,  Kind::Top},
    /* ZeroPositive */ {Kind::Bottom,       Kind::Positive,      Kind::ZeroPositive,  Kind::Top,
                        Kind::Top,          Kind::ZeroPositive,  Kind::Top,           Kind::Top},
    /* Top */          {Kind::Bottom,       Kind::Top,           Kind::Top,           Kind::Top,
                        Kind::Top,          Kind::Top,           Kind::Top,           Kind::Top},
};

static constexpr Kind mulTable[8][8] = {
    //                  Bottom,             Negative,            Zero,                Positive
    //                  One,                ZeroNegative,        ZeroPositive,        Top
    /* Bottom */       {Kind::Bottom,       Kind::Bottom,        Kind::Bottom,        Kind::Bottom,
                        Kind::Bottom,       Kind::Bottom,        Kind::Bottom,        Kind::Bottom},
    /* Negative */     {Kind::Bottom,       Kind::Positive,      Kind::Zero,          Kind::Negative,
                        Kind::Negative,     Kind::ZeroPositive,  Kind::ZeroNegative,  Kind::Top},
    /* Zero */         {Kind::Bottom,       Kind::Zero,          Kind::Zero,          Kind::Zero,
                        Kind::Zero,         Kind::Zero,          Kind::Zero,          Kind::Zero},
    /* Positive */     {Kind::Bottom,       Kind::Negative,      Kind::Zero,          Kind::Positive,
                        Kind::Positive,     Kind::ZeroNegative,  Kind::ZeroPositive,  Kind::Top},
    /* One */          {Kind::Bottom,       Kind::Negative,      Kind::Zero,          Kind::Positive,
                        Kind::One,          Kind::ZeroNegative,  Kind::ZeroPositive,  Kind::Top},
    /* ZeroNegative */ {Kind::Bottom,       Kind::ZeroPositive,  Kind::Zero,          Kind::ZeroNegative,
                        Kind::ZeroNegative, Kind::ZeroPositive,  Kind::ZeroNegative,  Kind::Top},
    /* ZeroPositive */ {Kind::Bottom,       Kind::ZeroNegative,  Kind::Zero,          Kind::ZeroPositive,
                        Kind::ZeroPositive, Kind::ZeroNegative,  Kind::ZeroPositive,  Kind::Top},
    /* Top */          {Kind::Bottom,       Kind::Top,           Kind::Zero,          Kind::Top,
                        Kind::Top,          Kind::Top,           Kind::Top,           Kind::Top},
};

static constexpr Kind divTable[8][8] = {
    //                  Bottom,             Negative,            Zero,                Positive
    //                  One,                ZeroNegative,        ZeroPositive,        Top
    /* Bottom */       {Kind::Bottom,       Kind::Bottom,        Kind::Bottom,        Kind::Bottom,
                        Kind::Bottom,       Kind::Bottom,        Kind::Bottom,        Kind::Bottom},
    /* Negative */     {Kind::Bottom,       Kind::Positive,      Kind::Bottom,        Kind::Negative,
                        Kind::Negative,     Kind::Bottom,        Kind::Bottom,        Kind::Top},
    /* Zero */         {Kind::Bottom,       Kind::Zero,          Kind::Bottom,        Kind::Zero,
                        Kind::Zero,         Kind::Bottom,        Kind::Bottom,        Kind::Bottom},
    /* Positive */     {Kind::Bottom,       Kind::Negative,      Kind::Bottom,        Kind::Positive,
                        Kind::Positive,     Kind::Bottom,        Kind::Bottom,        Kind::Top},
    /* One */          {Kind::Bottom,       Kind::Negative,      Kind::Bottom,        Kind::ZeroPositive,
                        Kind::One,          Kind::Bottom,        Kind::Bottom,        Kind::Top},
    /* ZeroNegative */ {Kind::Bottom,       Kind::ZeroPositive,  Kind::Bottom,        Kind::ZeroNegative,
                        Kind::ZeroNegative, Kind::Bottom,        Kind::Bottom,        Kind::Top},
    /* ZeroPositive */ {Kind::Bottom,       Kind::ZeroNegative,  Kind::Bottom,        Kind::ZeroPositive,
                        Kind::ZeroPositive, Kind::Bottom,        Kind::Bottom,        Kind::Top},
    /* Top */          {Kind::Bottom,       Kind::Top,           Kind::Bottom,        Kind::Top,
                        Kind::Top,          Kind::Bottom,        Kind::Bottom,        Kind::Top},
};

static constexpr Kind gtTable[8][8] = {
    //                  Bottom,             Negative,            Zero,                Positive
    //                  One,                ZeroNegative,        ZeroPositive,        Top
    /* Bottom */       {Kind::Bottom,       Kind::Bottom,        Kind::Bottom,        Kind::Bottom,
                        Kind::Bottom,       Kind::Bottom,        Kind::Bottom,        Kind::Bottom},
    /* Negative */     {Kind::Bottom,       Kind::Top,           Kind::Zero,          Kind::Zero,
                        Kind::Zero,         Kind::Top,           Kind::Zero,          Kind::Top},
    /* Zero */         {Kind::Bottom,       Kind::One,           Kind::Zero,          Kind::Zero,
                        Kind::Zero,         Kind::Top,           Kind::Top,           Kind::Top},
    /* Positive */     {Kind::Bottom,       Kind::One,           Kind::One,           Kind::Top,
                        Kind::Top,          Kind::One,           Kind::Top,           Kind::Top},
    /* One */          {Kind::Bottom,       Kind::One,           Kind::One,           Kind::Top,
                        Kind::Zero,         Kind::One,           Kind::Top,           Kind::Top},
    /* ZeroNegative */ {Kind::Bottom,       Kind::Top,           Kind::Top,           Kind::Zero,
                        Kind::Zero,         Kind::Top,           Kind::Zero,          Kind::Top},
    /* ZeroPositive */ {Kind::Bottom,       Kind::One,           Kind::Top,           Kind::Top,
                        Kind::Top,          Kind::Top,           Kind::Top,           Kind::Top},
    /* Top */          {Kind::Bottom,       Kind::Top,           Kind::Top,           Kind::Top,
                        Kind::Top,          Kind::Top,           Kind::Top,           Kind::Top},
};

static constexpr Kind eqTable[8][8] = {
    //                  Bottom,             Negative,            Zero,                Positive
    //                  One,                ZeroNegative,        ZeroPositive,        Top
    /* Bottom */       {Kind::Bottom,       Kind::Bottom,        Kind::Bottom,        Kind::Bottom,
                        Kind::Bottom,       Kind::Bottom,        Kind::Bottom,        Kind::Bottom},
    /* Negative */     {Kind::Bottom,       Kind::Top,           Kind::Zero,          Kind::Zero,
                        Kind::Zero,         Kind::Top,           Kind::Zero,          Kind::Top},
    /* Zero */         {Kind::Bottom,       Kind::Zero,          Kind::One,           Kind::Zero,
                        Kind::Zero,         Kind::Top,           Kind::Top,           Kind::Top},
    /* Positive */     {Kind::Bottom,       Kind::Zero,          Kind::Zero,          Kind::Top,
                        Kind::Top,          Kind::Zero,          Kind::Top,           Kind::Top},
    /* One */          {Kind::Bottom,       Kind::Zero,          Kind::Zero,          Kind::Top,
                        Kind::One,          Kind::Zero,          Kind::Top,           Kind::Top},
    /* ZeroNegative */ {Kind::Bottom,       Kind::Top,           Kind::Top,           Kind::Zero,
                        Kind::Zero,         Kind::Top,           Kind::Top,           Kind::Top},
    /* ZeroPositive */ {Kind::Bottom,       Kind::Zero,          Kind::Top,           Kind::Top,
                        Kind::Top,          Kind::Top,           Kind::Top,           Kind::Top},
    /* Top */          {Kind::Bottom,       Kind::Top,           Kind::Top,           Kind::Top,
                        Kind::Top,          Kind::Top,           Kind::Top,           Kind::Top},
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

    SignState tranfser_info = evalSub(lhs, rhs);
    propagateIfChanged(result, result->join(tranfser_info));
    return success();
  }

  if (isa<LLVM::MulOp>(op)) {
    SignState lhs = operands[0]->getValue();
    SignState rhs = operands[1]->getValue();

    SignState tranfser_info = evalMul(lhs, rhs);
    propagateIfChanged(result, result->join(tranfser_info));
    return success();
  }

  if (isa<LLVM::SDivOp>(op)) {
    SignState lhs = operands[0]->getValue();
    SignState rhs = operands[1]->getValue();

    SignState tranfser_info = evalDiv(lhs, rhs);
    propagateIfChanged(result, result->join(tranfser_info));
    return success();
  }
  if (auto cmp = dyn_cast<LLVM::ICmpOp>(op)) {
    if (cmp.getPredicate() == LLVM::ICmpPredicate::sgt) {
      SignState lhs = operands[0]->getValue();
      SignState rhs = operands[1]->getValue();

      SignState tranfser_info = evalGt(lhs, rhs);
      propagateIfChanged(result, result->join(tranfser_info));
      return success();
    }

    if (cmp.getPredicate() == LLVM::ICmpPredicate::eq) {
      SignState lhs = operands[0]->getValue();
      SignState rhs = operands[1]->getValue();

      SignState tranfser_info = evalEq(lhs, rhs);
      propagateIfChanged(result, result->join(tranfser_info));
      return success();
    }
  }


  return unknown();
}

static SignState evalAdd(SignState lhs, SignState rhs) {
  return addTable[static_cast<unsigned>(lhs.kind)]
                 [static_cast<unsigned>(rhs.kind)];
}

static SignState evalSub(SignState lhs, SignState rhs) {
  return subTable[static_cast<unsigned>(lhs.kind)]
                 [static_cast<unsigned>(rhs.kind)];
}

static SignState evalMul(SignState lhs, SignState rhs) {
  return mulTable[static_cast<unsigned>(lhs.kind)]
                 [static_cast<unsigned>(rhs.kind)];
}

static SignState evalDiv(SignState lhs, SignState rhs) {
  return divTable[static_cast<unsigned>(lhs.kind)]
                 [static_cast<unsigned>(rhs.kind)];
}

static SignState evalGt(SignState lhs, SignState rhs) {
  return gtTable[static_cast<unsigned>(lhs.kind)]
                [static_cast<unsigned>(rhs.kind)];
}

static SignState evalEq(SignState lhs, SignState rhs) {
  return eqTable[static_cast<unsigned>(lhs.kind)]
                [static_cast<unsigned>(rhs.kind)];
}

} // namespace Sign
