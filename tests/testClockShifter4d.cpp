#include "asserts.h"


class Inputs {
public:
    int period = 0;
    int cycles = 0;
    float shiftPerSample = 0;
};

class Outputs {
public:
    int outputClocks = 0;
    int samplesTicked = 0;
    int minSamplesBetweenClocks = 0;
    int maxSamplesBetweenClocks = 0;
};

static Outputs run(const Inputs&) {
    return Outputs();
}

static void test0() {
    const Outputs o = run(Inputs());
}

static void test1() {
    Inputs in;
    in.period = 10;
    in.cycles = 4;
    in.shiftPerSample = .1;
    const auto output = run(in);
}

void testClockShifter4d() {
    test0();
    test1();
}