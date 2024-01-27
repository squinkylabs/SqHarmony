
#include "FreqMeasure2.h"
#include "asserts.h"

static void testCanCall() {
    FreqMeasure2 x;
    // x.onSample(false);
    x.process(false, false);
    const bool f = x.freqValid();
    if (f) {
        x.getPeriod();
    }
}

static void testInitialConditions() {
    FreqMeasure2 x;
    assert(!x.freqValid());
}

static void testRequiresTwo() {
    FreqMeasure2 x;
    x.process(true, true);
    assertEQ(x.freqValid(), false);
}

static void testRequiresTwoB() {
    FreqMeasure2 x;
    x.process(true, true);
    x.process(false, false);

    assertEQ(x.freqValid(), false);
}

static void testCanSample() {
    FreqMeasure2 x;
    x.process(true, true);
    x.process(false, false);
    x.process(false, false);
    x.process(true, true);

    assert(x.freqValid());
    assert(x.getPeriod() == 3);
}

static void testCanRemember() {
    FreqMeasure2 x;
    x.process(true, true);
    x.process(false, false);
    x.process(false, false);
    x.process(true, true);
    assert(x.freqValid());
    assert(x.getPeriod() == 3);

    // should remember last full period
    x.process(false, false);
    assert(x.freqValid());
    assert(x.getPeriod() == 3);
}

static void testHold() {
    FreqMeasure2 x;

    // two clock high in a row with no triggers should not count.
    x.process(false, true);
    x.process(false, true);

    // const int y = x.getPeriod();
    assert(!x.freqValid());
}

static void testMeasurePeriod1() {
    // SQINFO("inter testMeasurePeriod1");
    FreqMeasure2 x;
    x.process(true, true);
    x.process(false, true);
    x.process(false, true);
    x.process(false, false);
    x.process(false, false);
    x.process(true, true);
    assert(x.freqValid());
    assertEQ(x.getPeriod(), 5);
    // SQINFO("measured %d", x.getPeriod());
    assertEQ(x.getHighDuration(), 3);
}

static void testCanFollow() {
    FreqMeasure2 x;
    x.process(true, true);
    x.process(false, false);
    x.process(false, false);
    x.process(true, true);
    assert(x.freqValid());
    assertEQ(x.getPeriod(), 3);

    for (int i = 0; i < 13; ++i) {
        x.process(false, false);
    }
    x.process(true, true);
    assert(x.freqValid());
    assertEQ(x.getPeriod(), 14);
}

void testFreqMeasure2() {
    testCanCall();
    testInitialConditions();
    testRequiresTwo();
    testRequiresTwoB();
    testCanSample();
    testCanRemember();
    testHold();

    testMeasurePeriod1();
    testCanFollow();
}