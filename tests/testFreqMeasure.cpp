
#include "asserts.h"
#include "FreqMeasure.h"

// test that we can call the api
static void test1() {
    FreqMeasure x;
    x.onSample(false);
}

static void test2() {
    FreqMeasure x;
    assert(!x.freqValid());
}

void testFreqMeasure() {
    test1();
     test2();
}