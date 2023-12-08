
#include "testShifter4TestUtils.h"
#include "asserts.h"


class Inputs {
public:
    int period = 0;
    int cycles = 0;
    float shiftPerSample = 0;
    float initialShift = 0;

    bool isValid() const {
        return period >= 2 && cycles > 0;
    }
};

class Outputs {
public:
    int outputClocks = 0;
    int samplesTicked = 0;
    int minSamplesBetweenClocks = 0;
    int maxSamplesBetweenClocks = 0;
};

static Outputs run(const Inputs& input) {
    assert(input.isValid());
    Outputs output;
    auto shifter = makeAndPrime(input.period, input.initialShift);
    for (int cycle = 0; cycle < input.cycles; cycle++) {
        for (int sample = 0; sample < input.period; ++sample) {
            output.samplesTicked++;
            const bool b = clockItLow(shifter, 1);
            if (b) {

            }
        }
    }
    return output;
}

static void test0() {
    Inputs in;
    in.period = 2;
    in.cycles = 1;
    const Outputs o = run(in);
}

static void test1() {
    Inputs in;
    in.period = 10;
    in.cycles = 4;
    const auto output = run(in);
    assertEQ(output.samplesTicked, 40);
    assertEQ(output.outputClocks, 4);
    assertEQ(output.maxSamplesBetweenClocks, 10);
    assertEQ(output.minSamplesBetweenClocks, 10);
}

void testClockShifter4d() {
    test0();
    test1();
}