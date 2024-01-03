
#include "asserts.h"
#include "NumberFormatter.h"

static void test0() {
    std::string s("123.456");
    auto s2 = NumberFormatter::formatFloat(2, s);
    assertEQ(s2, "123.45");
}
void testNumberFormatter() {
    test0();
}