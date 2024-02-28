
#include "FreqMeasure2.h"
#include "FreqMeasure3.h"
#include "asserts.h"

/**
 * @brief Tests FreqMeasure2 and 3
 * 
 */
static void testCanCall() {
    FreqMeasure2 x;
    // x.onSample(false);
    x.process(false, false);
    const bool f = x.freqValid();
    if (f) {
        x.getPeriod();
    }
}

static void testCanCall3() {
    FreqMeasure3 x;
    // x.onSample(false);
    x.process(false);
    const bool f = x.freqValid();
    if (f) {
        x.getPeriod();
    }
}

static void testInitialConditions() {
    FreqMeasure2 x;
    assert(!x.freqValid());
}

static void testInitialConditions3() {
    FreqMeasure3 x;
    assert(!x.freqValid());
}

static void testRequiresTwoClocks() {
    FreqMeasure2 x;
    x.process(true, true);
    assertEQ(x.freqValid(), false);
}

static void testRequiresTwoClocks3() {
    FreqMeasure3 x;
    x.process(true);
    assertEQ(x.freqValid(), false);
}

static void testRequiresTwoClocksB() {
    FreqMeasure2 x;
    x.process(true, true);
    x.process(false, false);

    assertEQ(x.freqValid(), false);
}

static void testRequiresTwoClocks3B() {
    FreqMeasure3 x;
    x.process(true);
    x.process(false);

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

static void testCanSample3() {
    FreqMeasure3 x;
    x.process(true);
    x.process(false);
    x.process(false);
    x.process(true);

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

static void testCanRemember3() {
    FreqMeasure3 x;
    x.process(true);
    x.process(false);
    x.process(false);
    x.process(true);
    assert(x.freqValid());
    assert(x.getPeriod() == 3);

    // should remember last full period
    x.process(false);
    assert(x.freqValid());
    assert(x.getPeriod() == 3);
}

static void testHold() {
    FreqMeasure2 x;
    // two clock high in a row with no triggers should not count.
    x.process(false, true);
    x.process(false, true);
    assert(!x.freqValid());
}

static void testMeasurePeriod() {
    FreqMeasure2 x;
    x.process(true, true);
    x.process(false, true);
    x.process(false, true);
    x.process(false, false);
    x.process(false, false);
    x.process(true, true);
    assert(x.freqValid());
    assertEQ(x.getPeriod(), 5);
    assertEQ(x.getHighDuration(), 3);
}

static void testMeasurePeriod3() {
    FreqMeasure3 x;
    x.process(true);
    x.process(false);
    x.process(false);
    x.process(false);
    x.process(false);
    x.process(true);
    assert(x.freqValid());
    assertEQ(x.getPeriod(), 5);
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

static void testCanFollow3() {
    FreqMeasure3 x;
    x.process(true);
    x.process(false);
    x.process(false);
    x.process(true);
    assert(x.freqValid());
    assertEQ(x.getPeriod(), 3);

    for (int i = 0; i < 13; ++i) {
        x.process(false);
    }
    x.process(true);
    assert(x.freqValid());
    assertEQ(x.getPeriod(), 14);
}

void testFreqMeasure2() {
    testCanCall();
    testCanCall3();
    testInitialConditions();
    testInitialConditions3();
    testRequiresTwoClocks();
    testRequiresTwoClocksB();
    testRequiresTwoClocks3();
    testRequiresTwoClocks3B();
    testCanSample();
    testCanSample3();
    testCanRemember();
    testCanRemember3();
    testHold();

    testMeasurePeriod();
    testMeasurePeriod3();
    testCanFollow();
    testCanFollow3();
}

#if 1
void testFirst() {
    testFreqMeasure2();
}
#endif