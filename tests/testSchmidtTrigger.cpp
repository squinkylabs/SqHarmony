
#include "SchmidtTrigger.h"
#include "asserts.h"

static void testCanConstruct() {
    SchmidtTrigger t;
    assertEQ(t.thhi(), cGateHi);
    assertEQ(t.thlo(), cGateLow);
    t.go(0);

    assertGT(cGateHi, cGateLow);
    assertGT(cGateLow, 0);
    assertLT(cGateHi, 10);
}

static void testBigInput() {
    SchmidtTrigger t;
    const auto x = t.go(10);
    assertEQ(x, true);
    const auto y = t.go(0);
    assertEQ(y, false);
}

static void testSmallerInput() {
    SchmidtTrigger t;
    const auto x = t.go(cGateHi + .0001);
    assertEQ(x, true);
    const auto y = t.go(cGateLow - .0001);
    assertEQ(y, false);
}

static void testHysteresis() {
    SchmidtTrigger t;

    // in between high and low
    const float inBetween = (cGateLow + cGateHi) / 2;
    const float over = 10;
    const float under = 0;

    auto x = t.go(inBetween);
    assertEQ(x, false);
    x = t.go(over);
    assertEQ(x, true);
    x = t.go(inBetween);
    assertEQ(x, true);
    x = t.go(under);
    assertEQ(x, false);
}

void testSchmidtTrigger() {
    testCanConstruct();
    testBigInput();
    testSmallerInput();
    testHysteresis();
}
