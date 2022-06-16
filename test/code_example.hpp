#include <string>

const std::string codeExample = R"(
var gvar: i32 = 123;

fn foo() i32 {
    return 1.0;
}

fn main() void {
    var x: i32 = foo();
    var y: i32 = 123 + (1 * x);
    y = 1;
}
)";
