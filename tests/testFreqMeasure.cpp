
#include "FreqMeasure.h"
#include "asserts.h"

// test that we can call the api
static void test1() {
    FreqMeasure x;
    x.onSample(false);
}

static void test2() {
    FreqMeasure x;
    assert(!x.freqValid());
}

static void test3() {
    FreqMeasure x;
    x.onSample(true);
    x.onSample(false);
    x.onSample(false);
    x.onSample(true);
    assert(x.freqValid());
    assert(x.getPeriod() == 3);
}

void testFreqMeasure() {
    test1();
    test2();
    test3();
}