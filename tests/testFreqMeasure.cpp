
#include "FreqMeasure.h"
#include "asserts.h"


static void testCanCall() {
    FreqMeasure x;
    x.onSample(false);
    const bool f = x.freqValid();
    if (f) {
        x.getPeriod();
    }
}

static void testInitialConditions() {
    FreqMeasure x;
    assert(!x.freqValid());
}

static void testRequiresTwo() {
    FreqMeasure x;
    x.onSample(true);
    assertEQ(x.freqValid(), false);
}

static void testRequiresTwoB() {
    FreqMeasure x;
    x.onSample(false);
    x.onSample(true);
    assertEQ(x.freqValid(), false);
}

static void testCanSample() {
    FreqMeasure x;
    x.onSample(true);
    x.onSample(false);
    x.onSample(false);
    x.onSample(true);
    assert(x.freqValid());
    assert(x.getPeriod() == 3);
}

static void testCanRemember() {
    FreqMeasure x;
    x.onSample(true);
    x.onSample(false);
    x.onSample(false);
    x.onSample(true);
    assert(x.freqValid());
    assert(x.getPeriod() == 3);

    // should remember last full period
    x.onSample(false);
    assert(x.freqValid());
    assert(x.getPeriod() == 3);
}

static void testHold() {
    FreqMeasure x;

    // two clock high in a row with no transition should not count.
    x.onSample(true);
    x.onSample(true);
    assert(!x.freqValid());
}

void testFreqMeasure() {
    testCanCall();
    testInitialConditions();
    testRequiresTwo();
    testRequiresTwoB();
    testCanSample();
    testCanRemember();
    testHold();
}