fn foo() u64 {
    return 1;
}

pub fn main() void {
    var x: u64 = foo();
    var y: u64 = 123 + (1 * 2);
    y = 1;
}
