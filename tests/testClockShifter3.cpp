
#include "ClockShifter3.h"
#include "asserts.h"

static void testCanCall() {
    ClockShifter3 c;
    c.setShift(.5);
    c.run(2);
}

class ClockShifter3Test {
private:
    // clocks high once, then low
    static void clockIt(ClockShifter3& c, int cyclesLow) {
        c.run(10);
        for (int i = 0; i < cyclesLow; ++i) {
            c.run(0);
        }
    }

    //  Sends two clocks, with two non-clocks in-between.
    static void prime(ClockShifter3& c) {
        clockIt(c, 2);
        c.run(10);
    }

public:
    // This tests that internal state responds to clocking as expected.
    static void testCounters() {
        ClockShifter3 c;

        prime(c);

        assertEQ(c._currentClockCount, 0);
        assertEQ(c._currentSampleCountSinceLastClock, 0);

        c.run(0);
        assertEQ(c._currentClockCount, 0);
        assertEQ(c._currentSampleCountSinceLastClock, 1);
        assertEQ(c._lastClock, false);

        c.run(0);
        assertEQ(c._currentClockCount, 0);
        assertEQ(c._currentSampleCountSinceLastClock, 2);

        c.run(10);
        assertEQ(c._currentClockCount, 1);
        assertEQ(c._currentSampleCountSinceLastClock, 0);

        c.run(10);
        assertEQ(c._currentClockCount, 1);
        assertEQ(c._currentSampleCountSinceLastClock, 1);

        c.run(0);
        assertEQ(c._currentClockCount, 1);
        assertEQ(c._currentSampleCountSinceLastClock, 2);
    }

    static void testDelay() {
        ClockShifter3 c;

        // because this is the first clock, it should not log anything
        clockIt(c, 2);
        assertEQ(c._clockDelayLine.size(), 0);

        c.run(10);
        assertEQ(c._clockDelayLine.size(), 1)

            auto ev = c._clockDelayLine.pop();
        assertEQ(int(ev._type), int(ClockShifter3::EventType::lowToHigh));
        assertEQ(int(ev._clocks), 1);
        assertEQ(int(ev._samples), 0);

        c.run(10);
        assertEQ(c._clockDelayLine.size(), 0);
        c.run(10);
        assertEQ(c._clockDelayLine.size(), 0);
        c.run(0);
        assertEQ(c._clockDelayLine.size(), 1);

        ev = c._clockDelayLine.pop();
        assertEQ(int(ev._type), int(ClockShifter3::EventType::highToLow));
        assertEQ(int(ev._clocks), 1);
        assertEQ(int(ev._samples), 0);

        c.run(10);
        assertEQ(c._clockDelayLine.size(), 1);
        ev = c._clockDelayLine.pop();
        assertEQ(int(ev._type), int(ClockShifter3::EventType::highToLow));
        assertEQ(int(ev._clocks), 1);
        assertEQ(int(ev._samples), 0);
    }

    // Checks that _freqMeasure is hooked up. It's
    // tested in its own unit test suite.
    static void testFreqMeasure() {
        ClockShifter3 c;
        c.run(0);
        c.run(0);
        assertEQ(c._freqMeasure.freqValid(), false);
        c.run(10);
        assertEQ(c._freqMeasure.freqValid(), false);
        c.run(0);
        c.run(0);
        assertEQ(c._freqMeasure.freqValid(), false);
        c.run(10);
        assertEQ(c._freqMeasure.freqValid(), true);
        assertEQ(c._freqMeasure.getPeriod(), 3);
    }

    static void testSetShift() {
        ClockShifter3 c;
        assertEQ(c._requestedShiftAmountClocks, 0);
        assertEQ(c._requestedFractionShiftAmount, 0);

        c.setShift(0);
        assertEQ(c._requestedShiftAmountClocks, 0);
        assertEQ(c._requestedFractionShiftAmount, 0);

        c.setShift(1);
        assertEQ(c._requestedShiftAmountClocks, 1);
        assertEQ(c._requestedFractionShiftAmount, 0);


        assert(false);
    }
};

void testClockShifter3() {
    testCanCall();
    // TODO: test Counters needs to prime the clock
    ClockShifter3Test::testCounters();
    ClockShifter3Test::testSetShift();
    ClockShifter3Test::testFreqMeasure();
    //  ClockShifter3Test::testDelay();
}