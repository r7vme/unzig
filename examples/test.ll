; ModuleID = 'unzig'
source_filename = "unzig"

define i32 @main() {
entry:
  %y = alloca float, align 4
  store float 0.000000e+00, float* %y, align 4
  store float add (float 1.000000e+00, float 1.000000e+00), float* %y, align 4
  ret i32 0
}
