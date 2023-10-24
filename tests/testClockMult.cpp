#include "ClockMult.h"
#include "asserts.h"

static void testCanCall() {
    ClockMult c;
    const bool b = c.go(true);
    c.setMul(3.3);
}

static int clockAndCountOutput(ClockMult& c, int lowPeriod) {
    int count = 0;
    bool b = c.go(true);  // one high clock
    if (b) {
        ++count;
    }
    for (int i = 0; i < lowPeriod; ++i) {
        b = c.go(false);
        if (b) {
            ++count;
        }
    }
    return count;
}

static int clockAndCountOutput(ClockMult& c, int lowPeriod, int periods) {
    int count = 0;
    for (int i = 0; i < periods; ++i) {
        count += clockAndCountOutput(c, lowPeriod);
    }
    return count;
}

static void testMulThree() {
    SQINFO("testMulThree");
    ClockMult c;
    c.setMul(3.0);
    const int count = clockAndCountOutput(c, 9, 100);
    SQINFO("count = %d", count);
    assertGT(count, 0);
}

class TestClockMult {
public:
    static void testTime() {
        ClockMult c;
        assertEQ(c._currentTime._clocks, 0);
        assertEQ(c._currentTime._samples, 0);

        c.go(true);
        assertEQ(c._currentTime._clocks, 1);
        assertEQ(c._currentTime._samples, 0);

        c.go(false);
        c.go(false);
        assertEQ(c._currentTime._clocks, 1);
        assertEQ(c._currentTime._samples, 2);
    }

    static void testFreq() {
        ClockMult c;
        assertEQ(c._freqMeasure.freqValid(), false);
        c.go(false);
        c.go(false);
        c.go(false);
        assertEQ(c._freqMeasure.freqValid(), false);
        c.go(true);
        assertEQ(c._freqMeasure.freqValid(), false);
        c.go(false);
        assertEQ(c._freqMeasure.freqValid(), false);
        c.go(true);
        assertEQ(c._freqMeasure.freqValid(), true);
        assertEQ(c._freqMeasure.getPeriod(), 2);
    }
};

void testClockMult() {
    testCanCall();
    TestClockMult::testTime();
    TestClockMult::testFreq();

    // not working yet
    // testMulThree();
}