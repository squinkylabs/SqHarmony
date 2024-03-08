

#include "asserts.h"

#include "Filter4pButter.h"

static void testCanCall() {
    Filter4PButter<float> filt;
    filt.setCutoffFreq(.1);
    filt.process(.1);

    Filter4PButter<float_4> filt2;
    filt2.setCutoffFreq(.1f);
    filt2.process(float_4(.1));
}

void testButter() {
    testCanCall();
}