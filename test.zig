const print = @import("std").debug.print;

var y: u64 = 1234;

pub fn main() void {
  y = 1;
  print("{d}\n", .{y});
}
