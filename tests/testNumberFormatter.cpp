

#include "NumberFormatter.h"
#include "asserts.h"

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

static void testSmall() {
    std::string s(".123");
    auto s2 = NumberFormatter::formatFloat(2, s);
    assertEQ(s2, ".12");
}

static void testFull1() {
    float x = 1;
    std::string s = NumberFormatter::formatFloat2(2, x);
    assertEQ("1.00", s);
}

static void testFull100() {
    float x = 100;
    std::string s = NumberFormatter::formatFloat2(2, x);
    assertEQ("100", s);
}

static void testFullem5() {
    float x = 1e-5;
    std::string s = NumberFormatter::formatFloat2(2, x);
    assertEQ(s, "0.00");
}

static void testFulle5() {
    float x = 1e5;
    std::string s = NumberFormatter::formatFloat2(2, x);
    assertEQ(s, "xxx");
}
static void testFull1000() {
    float x = 1000;
    std::string s = NumberFormatter::formatFloat2(2, x);
    assertEQ(s, "1000");
}

static void testFullSmall() {
    float x = .0001;
    std::string s = NumberFormatter::formatFloat2(2, x);
    assertEQ("0.00", s);
}

void testNumberFormatter() {
    test0();
    test1();
    test2();
    test3();
    testSmall();

    testFull1();
    testFull100();
    testFullem5();
    testFull1000();
    testFulle5();
    testFullSmall();
}

#if 0
void testFirst() {
    // testFulle5();
    testNumberFormatter();
    testFullSmall();
}
#endif
