
#include "GateDelay.h"
#include "TestComposite.h"
#include "asserts.h"


static void testInit() {
    GateDelay gd;
    Input input;

    for (int i=0; i<16; ++i) {
        assert(!gd.getGate(i));
    }   
}

static void testNoDelay(unsigned channel, unsigned otherChannel) {
    assert(channel < 16);
    assert(otherChannel < 16);
    assert(channel != otherChannel);

    GateDelay gd;
    Input input;

    input.setVoltage(10, channel);
    gd.process(input, 16);
    assert(gd.getGate(channel));
    assert(!gd.getGate(otherChannel));
}

static void testNoDelay() {
     for (int i=0; i<16; ++i) {
        const unsigned other = 15 - i;
        testNoDelay(i, other);
     }
}

static void testWithDelay(unsigned channel, unsigned otherChannel) {
 //   SQINFO("---- testWithDelay(%d, %d)", channel, otherChannel);
    assert(channel < 16);
    assert(otherChannel < 16);
    assert(channel != otherChannel);

    GateDelay gd;
    gd.enableDelay(true);
    Input input;

    input.setVoltage(10, channel);
    gd.process(input, 16);
    assert(!gd.getGate(channel));
    assert(!gd.getGate(otherChannel));

     gd.process(input, 16);
    assert(!gd.getGate(channel));
    assert(!gd.getGate(otherChannel));

     gd.process(input, 16);
    assert(!gd.getGate(channel));
    assert(!gd.getGate(otherChannel));

     gd.process(input, 16);
    assert(!gd.getGate(channel));
    assert(!gd.getGate(otherChannel));

    gd.process(input, 16);
    assert(!gd.getGate(channel));
    assert(!gd.getGate(otherChannel));

    gd.process(input, 16);
    assert(gd.getGate(channel));
    assert(!gd.getGate(otherChannel));

}

static void testWithDelay() {
     for (int i=0; i<16; ++i) {
        const unsigned other = 15 - i;
        testWithDelay(i, other);
     }
}

void testGateDelay() {
    testInit();
    testNoDelay();
    testWithDelay();
}