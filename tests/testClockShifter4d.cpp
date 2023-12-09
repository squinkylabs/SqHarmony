
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
    int minSamplesBetweenClocks = 100000;
    int maxSamplesBetweenClocks = 0;

    void processPossibleClock(bool clock) {
        samplesTicked++;
        if (!clock) {
            return;
        }
        outputClocks++;
        const int curSample = this->samplesTicked;
        if (lastClockSample >= 0) {
            const int delta = curSample - lastClockSample;
            assert(delta > 1);
            minSamplesBetweenClocks = std::min(minSamplesBetweenClocks, delta);
            maxSamplesBetweenClocks = std::max(maxSamplesBetweenClocks, delta);
        }
        lastClockSample = curSample;
    }

    std::string toString() const {
        std::stringstream s;
        s << "samples=" << samplesTicked << std::endl;
        s << "clocks =" << outputClocks << std::endl;
        s << "min=" << minSamplesBetweenClocks << " max=" << maxSamplesBetweenClocks << std::endl;
        return s.str();
    }
private:
    int lastClockSample = -1;
};

// std::shared_ptr<ClockShifter4> shifter

static Outputs run(const Inputs& input) {
    assert(input.isValid());
    Outputs output;

    // prime will feed in the "first" clock, so expect "off by one" errors.
    auto shifter = makeAndPrime(input.period, input.initialShift);
    for (int cycle = 0; cycle < input.cycles; cycle++) {

        // First put out n-1 samples with no clock
        for (int sample = 0; sample < (input.period-1); ++sample) {
           // output.samplesTicked++;
            const bool b = clockItLow(shifter, 1);
            output.processPossibleClock(b);
        }

        // Then one with a clock.
        // Except at the very end. Don't output the last clock.
        if (cycle < input.cycles-1) {
            const bool b = shifter->process(true, true);
            output.processPossibleClock(b);
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
    SQINFO("end of test01, output = %s", output.toString().c_str());
    assertEQ(output.samplesTicked, 40 - 1);
    assertEQ(output.outputClocks, 3);       // we don't count the start or end
    assertEQ(output.maxSamplesBetweenClocks, 10);
    assertEQ(output.minSamplesBetweenClocks, 10);

   
}

void testClockShifter4d() {
    test0();
    test1();
}