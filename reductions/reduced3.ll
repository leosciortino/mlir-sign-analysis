
define i1 @equal_known_values() {
entry:
  %one_eq_one = icmp eq i32 0, 0
  ret i1 %one_eq_one
}
