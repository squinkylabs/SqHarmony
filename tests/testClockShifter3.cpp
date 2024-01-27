
#include "ClockShifter3.h"
#include "asserts.h"

static void testCanCall() {
    ClockShifter3 c;
    c.setShift(.5);
    c.run(2);
}

class TestX {
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
        assert(c._freqMeasure.freqValid()); 
    }

public:
    static void testInit() {
        ClockShifter3 c;
        assertEQ(c._gateOut, false);
        assertEQ(c._clockDelayLine.empty(), true);
        assertEQ(c._currentTime._clocks, 0);
        assertEQ(c._currentTime._samples, 0);
        assertEQ(c._freqMeasure.freqValid(), false);
        assertEQ(c._lastClock, false);
        assertEQ(c._requestedShift._clocks, 0);
        assertEQ(c._requestedShift._phase, 0);
    }

    // This tests that internal state responds to clocking as expected.
    static void testCounters() {
        ClockShifter3 c;

        prime(c);

        assertEQ(c._currentTime._clocks, 0);
        assertEQ(c._currentTime._samples, 0);

        c.run(0);
        assertEQ(c._currentTime._clocks, 0);
        assertEQ(c._currentTime._samples, 1);
        assertEQ(c._lastClock, false);

        c.run(0);
        assertEQ(c._currentTime._clocks, 0);
        assertEQ(c._currentTime._samples, 2);

        c.run(10);
        assertEQ(c._currentTime._clocks, 1);
        assertEQ(c._currentTime._samples, 0);

        c.run(10);
        assertEQ(c._currentTime._clocks, 1);
        assertEQ(c._currentTime._samples, 1);

        c.run(0);
        assertEQ(c._currentTime._clocks, 1);
        assertEQ(c._currentTime._samples, 2);
    }

    static void testDelay() {
        ClockShifter3 c;

        // because this is the first clock, it should not log anything
        prime(c);
        assertEQ(c._clockDelayLine.size(), 0);

        // this clock will log something, but it will get serviced
        c.setShift(20);     // set a huge shift, so nothing is processed from the queue.
        c.run(10);
        assertEQ(c._clockDelayLine.size(), 1);

        auto ev = c._clockDelayLine.pop();
        assertEQ(int(ev._type), int(ClockShifter3::EventType::lowToHigh));
        assertEQ(int(ev._timeStamp._clocks), 1);
        assertEQ(int(ev._timeStamp._samples), 0);

        // now c3 more high clock in a row (total 4)
        const int c3 = 7;
        for (int i = 0; i < c3; ++i) {
            c.run(10);
        }
        assertEQ(c._clockDelayLine.size(), 0);

        // now high to low should trigger an event
        c.run(0);
        assertEQ(c._clockDelayLine.size(), 1);

        ev = c._clockDelayLine.pop();
        assertEQ(int(ev._type), int(ClockShifter3::EventType::highToLow));
        assertEQ(int(ev._timeStamp._clocks), 1);
        assertEQ(int(ev._timeStamp._samples), c3);

        c.run(10);
        assertEQ(c._clockDelayLine.size(), 1);
        ev = c._clockDelayLine.pop();
        assertEQ(int(ev._type), int(ClockShifter3::EventType::lowToHigh));
        assertEQ(int(ev._timeStamp._clocks), 2);
        assertEQ(int(ev._timeStamp._samples), 0);
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
        assertEQ(c._requestedShift._clocks, 0);
        assertEQ(c._requestedShift._phase, 0);

        c.setShift(0);
        assertEQ(c._requestedShift._clocks, 0);
        assertEQ(c._requestedShift._phase, 0);

        c.setShift(1);
        assertEQ(c._requestedShift._clocks, 1);
        assertEQ(c._requestedShift._phase, 0);

        c.setShift(.1);
        assertEQ(c._requestedShift._clocks, 0);
        assertClose(c._requestedShift._phase, .1, .00001);

        c.setShift(3.712);
        assertEQ(c._requestedShift._clocks, 3);
        assertClose(c._requestedShift._phase, .712, .00001);
    }

    static void testServiceDelay() {
        ClockShifter3 c;
        prime(c);
        assert(c._freqMeasure.freqValid());     // test assumes this
        c._currentTime.reset();
        c.serviceDelayLine();


        // put in a low to high with 3 clocks, no samples
        ClockShifter3::ClockEvent ev(ClockShifter3::EventType::lowToHigh, 3, 0);
        c._clockDelayLine.push(ev);
        c.serviceDelayLine();
        // since current time is still 0, nothing should have happened.
        assertEQ(c._gateOut, false);
        assertEQ(c._clockDelayLine.empty(), false);

        c._currentTime._clocks = 3;
         c.serviceDelayLine();
        assertEQ(c._gateOut, true);
        assertEQ(c._clockDelayLine.empty(), true);

       // clockIt(ClockShifter3& c, 10); 
       // clockIt(ClockShifter3& c, 10); 

    }
};

void testClockShifter3() {
    testCanCall();
    // TODO: test Counters needs to prime the clock
    TestX::testInit();
    TestX::testCounters();
    TestX::testSetShift();
    TestX::testFreqMeasure();
    TestX::testDelay();
    TestX::testServiceDelay();
}