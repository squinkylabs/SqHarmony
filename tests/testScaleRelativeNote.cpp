
#include "ScaleRelativeNote.h"
#include <assert.h>

static void test0() {
    ScaleRelativeNote srn;
    assert(!srn.isValid());
    srn.set(1);
    assert(srn.isValid());
}

static void testInterval() {
    ScaleRelativeNote srn;
    srn.set(1);     // set to C

    ScaleRelativeNote srn2;
    srn2.set(2);     // set to D

    const int interval = srn.interval(srn2);
    assert(interval == 2);

}

void testScaleRelativeNote() {
    test0();
    testInterval();

    // meaningless?
    // assert(false);          // test absolute pitch
}