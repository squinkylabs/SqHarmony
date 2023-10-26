#include "ClockMult.h"
#include "asserts.h"

static void testCanCall() {
    ClockMult c;
    const bool b = c.run(true);
    c.setMul(3.3);
}

static int clockAndCountOutput(ClockMult& c, int lowPeriod) {
    int count = 0;
    bool b = c.run(true);  // one high clock
    if (b) {
        ++count;
    }
    for (int i = 0; i < lowPeriod; ++i) {
        b = c.run(false);
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

// test with input period == 10,
// mult == 1
static void testMul1() {
    ClockMult c;
    c.setMul(1);
    const int lowPeriod = 10 - 1;
    auto count = clockAndCountOutput(c, lowPeriod);
    assertEQ(count, 0);  // no clocks during prime

    bool b = c.run(true);
    assert(b);
    count = clockAndCountOutput(c, lowPeriod);
    assertEQ(count, 0);
}

class TestClockMult {
public:
    static void testSetMul() {
        ClockMult c;
        assertEQ(c._mul, 1);
        c.setMul(1);
        assertEQ(c._mul, 1);
        c.setMul(3.3);
        assertEQ(c._mul, 3.3);
    }

    static void testTime0() {
        ClockMult c;
        assertEQ(c._currentTime._clocks, 0);
        assertEQ(c._currentTime._samples, 0);

        c.run(true);  // one clock to start period measure
        c.run(false);
        c.run(false);
        c.run(false);

        c.run(true);  // second clock counts: will establish period 4 (not that it matters)

        // After clock that start us, we should be on clock zero
        assertEQ(c._currentTime._clocks, 0);
        assertEQ(c._currentTime._samples, 0);
        assertEQ(c._nextClockToOutput, 1);
    }

    static void testFreq() {
        ClockMult c;
        assertEQ(c._freqMeasure.freqValid(), false);
        c.run(false);
        c.run(false);
        c.run(false);
        assertEQ(c._freqMeasure.freqValid(), false);
        c.run(true);
        assertEQ(c._freqMeasure.freqValid(), false);
        c.run(false);
        assertEQ(c._freqMeasure.freqValid(), false);
        c.run(true);
        assertEQ(c._freqMeasure.freqValid(), true);
        assertEQ(c._freqMeasure.getPeriod(), 2);
    }

    static void testTargetTime() {
        ClockMult c;
        c.setMul(2.5);
        // first target will be at the same time
        assert(c._nextOutTime == ShiftMath::ClockWithSamples(0, 0));

        assertEQ(c._freqMeasure.freqValid(), false);  // this is just an internal consistency check

        // prime clock to period == 3
        c.run(true);
        c.run(false);
        c.run(false);
        assertEQ(c._freqMeasure.freqValid(), false);  // this is just an internal consistency check

        // the the next target will be advanced

        const bool b = c.run(true);
        assertEQ(c._freqMeasure.freqValid(), true);  // this is just an internal consistency check
        assert(c._nextOutTime != ShiftMath::ClockWithSamples(0, 0));
        assertEQ(c._nextOutTime._clocks, 0);
        assertEQ(c._nextOutTime._samples, 1);  // one sample is about the rate of the multiplied clock
    }
};

void testClockMult() {
    testCanCall();
    TestClockMult::testSetMul();
    // TODO: fix it
    SQINFO("bgf: fix the other tests");
    TestClockMult::testTime0();
    //  TestClockMult::testFreq();
   // TestClockMult::testTargetTime();

    // not working yet
    testMul1();
   // testMulThree();
}