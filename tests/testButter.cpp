

#include "asserts.h"

#include "Filter4pButter.h"

static void testCanCall() {
    Filter4PButter filt;
    filt.setCutoffFreq(.1);
    filt.process(.1);
}

void testButter() {
    testCanCall();
}