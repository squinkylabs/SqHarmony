
#include "asserts.h"
#include "testShifter4TestUtils.h"

class Inputs {
public:
    int period = 0;
    int totalSamplesToTick = 0;
    float shiftPerSample = 0;
    float initialShift = 0;

    bool isValid() const {
        return period >= 2 && totalSamplesToTick > period;
    }
    std::string toString() const {
        std::stringstream s;
        s << "totalSamplesToTick=" << totalSamplesToTick << std::endl;
        s << "period=" << period << std::endl;
        s << "initialshift=" << initialShift << std::endl;
        s << "shiftPerSample=" << shiftPerSample << std::endl;
        return s.str();
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

/**
 * The second clock in prime "counts" it is part of input.totalSamplesToTick,
 * and if it generates a clock is will be in Outputs.outputClocks.
 */
static Outputs run(const Inputs& input) {
    assert(input.isValid());
    Outputs output;
    SQINFO("enter run, input = %s", input.toString().c_str());

    // prime will feed in the "first" clock, so expect "off by one" errors.
    auto result = makeAndPrime2(input.period, input.initialShift);
    output.samplesTicked = 1;
    output.outputClocks = result.clocked ? 1 : 0;
    auto shifter = result.shifter;

   // assertEQ(input.shiftPerSample, 0);

    
    float shift = input.initialShift;
    SQINFO("run going into loop, samples ticked = %d, clocks= %d", output.samplesTicked, output.outputClocks);

    int samplesRemaining = input.totalSamplesToTick - output.samplesTicked;
    while (samplesRemaining) {
        int samplesThisTime = std::min(samplesRemaining, input.period - 1);
        SQINFO("run -in while, this time = %d remaining =%d ticked = %d", samplesThisTime, samplesRemaining, output.samplesTicked);
        samplesRemaining -= samplesThisTime;
        for (int i = 0; i < samplesThisTime; ++i) {
            SQINFO("run about to clock it low samp %d this time=%d", i, samplesThisTime);
            const bool b = clockItLow(shifter, 1);
         
            output.processPossibleClock(b);
            shift += input.shiftPerSample;
            shifter->setShift(shift);
            SQINFO("run tick low, ck=%d set shift to %f", b, shift);
        }
        if (samplesRemaining > 0) {
            samplesRemaining--;
            SQINFO("about to tick high");
            const bool b = shifter->process(true, true);
            SQINFO("tick high, ck=%d", b);
            output.processPossibleClock(b);
        }
        SQINFO("  bottom of loop, this time = %d remaining =%d ticked = %d", samplesThisTime, samplesRemaining, output.samplesTicked);
    }

    return output;
}

static void test0() {
    Inputs in;
    in.period = 2;
    // in.cycles = 1;
    in.totalSamplesToTick = 4;
    const Outputs o = run(in);
}

// This tests the case where we emit extra clocks at the bottom, to
// be sure they don't generate clocks.
static void testNoShift() {
    Inputs in;
    const int cycles = 12345;
    in.period = 10;
    in.totalSamplesToTick = (cycles + in.initialShift) * in.period;
    const auto output = run(in);
    // SQINFO("end of test01, output = %s", output.toString().c_str());
    assertEQ(output.samplesTicked, in.totalSamplesToTick);
    assertEQ(output.outputClocks, cycles);
    assertEQ(output.maxSamplesBetweenClocks, 10);
    assertEQ(output.minSamplesBetweenClocks, 10);
}

static void testShift2() {
    const int cycles = 54321;
    Inputs in;
    in.period = 10;
    in.initialShift = .2;
    in.totalSamplesToTick = (cycles + in.initialShift) * in.period;

    const auto output = run(in);
    //  SQINFO("end of testShift2, output = %s", output.toString().c_str());
    assertEQ(output.samplesTicked, in.totalSamplesToTick);
    assertEQ(output.outputClocks, cycles);
    assertEQ(output.maxSamplesBetweenClocks, 10);
    assertEQ(output.minSamplesBetweenClocks, 10);
}

static void testSlowDown() {
    SQINFO("------- testSlowDown");
    Inputs in;
    const int cycles = 5;
    in.period = 10;
    in.totalSamplesToTick = (cycles + in.initialShift) * in.period;
    in.shiftPerSample = .1;
    const auto output = run(in);
    // SQINFO("end of test01, output = %s", output.toString().c_str());
    assertEQ(output.samplesTicked, in.totalSamplesToTick);
    assertEQ(output.outputClocks, cycles);
    assertEQ(output.maxSamplesBetweenClocks, 10);
    assertEQ(output.minSamplesBetweenClocks, 10);
}

void testClockShifter4d() {
    test0();
    // testNoShift();
   // testShift2();
    SQWARN("testSlowDown should work");
    //testSlowDown();
}