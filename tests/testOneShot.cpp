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


void testOneShot() {
    testExists();
    testInit();
    testReset();
    testFiresEarly();
    testFires();
}