
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

static void testOneShot() {
    Comp c;

    // prime the clock with a long period
    int x = clockItHighLow(c, 4000);
    assertEQ(x, 0);
    x = clockItHigh(c);
    assertEQ(x, 1);

    auto args = TestComposite::ProcessArgs();
    args.sampleTime = .0001;        // tenth of a milliseconds

    // now clock low and see how long the output it
    int measuredDuration = 0;
    c.inputs[Comp::CK_INPUT].setVoltage(0);
    for (int i = 0; i < 400; ++i) {
        c.process(args);
        const bool output = c.outputs[Comp::CK_OUTPUT].getVoltage(0) > 5;
        if (i == 0) assertEQ(output, true);
        if (output == false) {
            measuredDuration = i;
            break;
        }
    }
  
    assertClose(measuredDuration, 10, 2);
    
    //assert(false);
}

void testMultiplier() {
    test0();
    testSomeOutput();
    testOneShot();
}