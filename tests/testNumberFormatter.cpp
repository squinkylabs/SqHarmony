
#include "asserts.h"
#include "NumberFormatter.h"

static void test0() {
    std::string s("123.456");
    auto s2 = NumberFormatter::formatFloat(2, s);
    assertEQ(s2, "123.45");
}

static void test1() {
    std::string s("123.4");
    auto s2 = NumberFormatter::formatFloat(2, s);
    assertEQ(s2, "123.40");
}

static void test2() {
    std::string s("123.45");
    auto s2 = NumberFormatter::formatFloat(2, s);
    assertEQ(s2, "123.45");
}

static void test3() {
    std::string s("123");
    auto s2 = NumberFormatter::formatFloat(2, s);
    assertEQ(s2, "123.00");
}

void testNumberFormatter() {
    test0();
    test1();
    test2();
    test3();
}