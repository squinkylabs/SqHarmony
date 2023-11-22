
#include "ScaleRelativeNote.h"
#include "asserts.h"

static void test0() {
    ScaleRelativeNote srn;
    assert(!srn.isValid());
    srn.setScaleDegree(1);
    assert(srn.isValid());
}

static void testInterval() {
    ScaleRelativeNote srn;
    srn.setScaleDegree(1);     // set to C

    ScaleRelativeNote srn2;
    srn2.setScaleDegree(2);     // set to D
    const int interval = srn.interval(srn2);
    assert(interval == 2);
}

static void testLeadingTone() {
    ScaleRelativeNote srn;
    for (int i=1; i<8; ++i) {
        srn.setScaleDegree(i);
        const bool expectLeadingTone = (i == 7);
        assertEQ(srn.isLeadingTone(), expectLeadingTone);
    }
}

void testScaleRelativeNote() {
    test0();
    testInterval();
    testLeadingTone();
}