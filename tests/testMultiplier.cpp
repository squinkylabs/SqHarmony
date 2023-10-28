
#include "Multiplier.h"
#include "TestComposite.h"

#include "asserts.h"
#include <memory>

using Comp = Multiplier<TestComposite>;
using CompPtr = std::shared_ptr<Comp>;

static int clockItLow(Comp& c, int numTimes) {
    const auto args = TestComposite::ProcessArgs();
    int x = 0;
    c.inputs[Comp::CK_INPUT].setVoltage(0);
    for (int i = 0; i < numTimes; ++i) {
        c.process(args);
        if (c.outputs[Comp::CK_OUTPUT].getVoltage(0) > 5) {
            x++;
        }
        // if (expectedOutput >= 0) {
        //     assertEQ(c.outputs[Comp::CK_OUTPUT].getVoltage(0), expectedOutput);
        // }
    }
    return x;
}
static int clockItHigh(Comp& c) {
    const auto args = TestComposite::ProcessArgs();
    c.inputs[Comp::CK_INPUT].setVoltage(10);
    c.process(args);
    return c.outputs[Comp::CK_OUTPUT].getVoltage(0) > 5 ? 1 : 0;
}

static int clockItHighLow(Comp& c, int numLow) {
    int x = clockItHigh(c);
    x += clockItLow(c, numLow);
    return x;
}

static void test0() {
    Comp p;
}

static void testSomeOutput() {
    Comp c;
    int x = clockItHighLow(c, 4);
    assertEQ(x, 0);
    x = clockItHigh(c);
    assertEQ(x, 1);
}




void testMultiplier() {
    test0();
    testSomeOutput();
}