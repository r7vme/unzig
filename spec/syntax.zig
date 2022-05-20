fn foo() u64 {
    return 1;
}

fn main() void {
    var x: u64 = foo();
    var y: u64 = 123 + (1 * x);
    y = 1;
}
