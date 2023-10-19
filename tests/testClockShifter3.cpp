
#include "ClockShifter3.h"
#include "asserts.h"

static void testCanCall() {
    ClockShifter3 c;
    c.setShift(.5);
    c.run(2);
}

static void testCounters() {
    ClockShifter3 c;
    assertEQ(c._clockCount, 0);
    assertEQ(c._sampleCountSinceLastClock, 0);
    assertEQ(int(c._lastClock), int(ClockShifter3::ClockStates::unknown));

    c.run(0);
    assertEQ(c._clockCount, 0);
    assertEQ(c._sampleCountSinceLastClock, 1);
    assertEQ(int(c._lastClock), int(ClockShifter3::ClockStates::low));

    c.run(0);
    assertEQ(c._clockCount, 0);
    assertEQ(c._sampleCountSinceLastClock, 2);

    c.run(10);
    assertEQ(c._clockCount, 1);
    assertEQ(c._sampleCountSinceLastClock, 0);

    c.run(10);
    assertEQ(c._clockCount, 1);
    assertEQ(c._sampleCountSinceLastClock, 1);

    c.run(0);
    assertEQ(c._clockCount, 1);
    assertEQ(c._sampleCountSinceLastClock, 2);
}

static void testDelay() {
    ClockShifter3 c;
    assertEQ(c._clockDelayLine.size(), 0);
    c.run(0);
    assertEQ(c._clockDelayLine.size(), 0);
    c.run(10);
    assertEQ(c._clockDelayLine.size(), 1);

    const auto ev = c._clockDelayLine.pop();
    assertEQ(int(ev._type), int(ClockShifter3::EventType::lowToHigh));
    assertEQ(int(ev._clocks), 1);
    assertEQ(int(ev._samples), 0);

    c.run(10);
    assertEQ(c._clockDelayLine.size(), 0);
    c.run(10);
    assertEQ(c._clockDelayLine.size(), 0);
    c.run(0);
    assertEQ(c._clockDelayLine.size(), 1);

    const auto ev2 = c._clockDelayLine.pop();
    assertEQ(int(ev2._type), int(ClockShifter3::EventType::highToLow));
    assertEQ(int(ev._clocks), 1);
    assertEQ(int(ev._samples), 0);
}

void testClockShifter3() {
    testCanCall();
    testCounters();
    testDelay();
}