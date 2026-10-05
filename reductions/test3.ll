; Equality facts derived from the extended sign domain.
;
; Run from this directory with:
;   ./test3.sh test3.ll
;
; The useful facts are on the `icmp eq` instructions themselves:
;   * equal known values are One (true),
;   * values with incompatible signs are Zero (false), and
;   * two merely-positive values remain unknown (not annotated).

define i1 @equal_known_values() {
entry:
  ; Both expressions are exactly one.
  %one_a = add i32 0, 1
  %one_b = add i32 0, 1
  %one_eq_one = icmp eq i32 %one_a, %one_b
  ret i1 %one_eq_one
}

define i1 @opposite_signs_cannot_be_equal() {
entry:
  ; The computed operands, rather than the comparisons, carry the sign facts.
  %positive = add nsw i32 40, 2
  %negative = sub nsw i32 0, 7
  %positive_eq_zero = icmp eq i32 %positive, 0
  %negative_eq_positive = icmp eq i32 %negative, %positive
  %both_false = and i1 %positive_eq_zero, %negative_eq_positive
  ret i1 %both_false
}

define i1 @positive_does_not_imply_equal() {
entry:
  ; Same abstract sign alone is insufficient to prove equality.
  %two = add nsw i32 1, 1
  %three = add nsw i32 1, 2
  %unknown = icmp eq i32 %two, %three
  ret i1 %unknown
}
