#include "asserts.h"
#include "OneShot.h"

static void testExists() {
    OneShot o;
    o.setSampleTime(.1);
    o.setDelayMs(1);
    o.hasFired();
    o.step();
    o.step(.001);
}

static void testInit() {
    OneShot o;
    o.setDelayMs(1);
    assertEQ(o.hasFired(), true);
}

static void testReset() {
    OneShot o;
    o.setDelayMs(1);
    assertEQ(o.hasFired(), true);
    o.set();
    assertEQ(o.hasFired(), false);
}

static void testFiresEarly() {
    OneShot o;
    o.setDelayMs(1);
    o.set();
    o.step(.0009);      // .9 ms;
    assertEQ(o.hasFired(), false);
}

static void testFires() {
    OneShot o;
    o.setDelayMs(1);
    o.set();
    o.step(.0011);      // 1.1 ms;
    assertEQ(o.hasFired(), true);
}

static void testFiresIncrementally() {
    OneShot o;
    o.setDelayMs(13);
    o.set();
    
    const float deltaMs = 1.01;
    const float deltaSec = deltaMs / 1000.0;
    assertEQ(o.hasFired(), false);   
    for (int i=0; i < 12; ++i) {
        o.step(deltaSec);
        assertEQ(o.hasFired(), false); 
    }
     o.step(deltaSec);
    assertEQ(o.hasFired(), true);
}

void testOneShot() {
    testExists();
    testInit();
    testReset();
    testFiresEarly();
    testFires();
    testFiresIncrementally();
}