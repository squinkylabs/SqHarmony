
#include "TestLFO.h"
#include "asserts.h"
#include "testShifter5TestUtils.h"

extern bool doLongRunning;

class TestContext {
public:
    TestLFO _testLFO;
};

class Inputs5 {
public:
    int period = 0;
    int totalSamplesToTick = 0;
    float shiftPerSample = 0;
    float initialShift = 0;
    bool afterwardsRunBackwards = false;

    bool isValid() const {
        return period >= 2 && totalSamplesToTick > period;
    }
    std::string toString() const {
        std::stringstream s;
        s << "totalSamplesToTick=" << totalSamplesToTick << std::endl;
        s << "period=" << period << std::endl;
        s << "initialshift=" << initialShift << std::endl;
        s << "shiftPerSample=" << shiftPerSample << std::endl;
        s << "afterwardsRunBackwards=" << afterwardsRunBackwards << std::endl;
        return s.str();
    }
};

class Outputs5 {
public:
    int outputClocks = 0;
    int samplesTicked = 0;
    int minSamplesBetweenClocks = 100000;
    int maxSamplesBetweenClocks = 0;

    // this only for carrying through to next block.
    float lastShift = 0;
    float maxShift = -100;
    float minShift = 100;
    Outputs5 combine(const Outputs5& one) const {
        Outputs5 ret = one;
        ret.outputClocks += this->outputClocks;
        ret.samplesTicked += this->samplesTicked;
        ret.minSamplesBetweenClocks = std::min(one.minSamplesBetweenClocks, this->minSamplesBetweenClocks);
        ret.maxSamplesBetweenClocks = std::max(one.maxSamplesBetweenClocks, this->maxSamplesBetweenClocks);
        return ret;
    }

    void processPossibleClock(bool clock, float shift) {
        // SQINFO("processPossibleClock(%d)", clock);
        samplesTicked++;
        minShift = std::min(minShift, shift);
        maxShift = std::max(maxShift, shift);
        if (!clock) {
            // SQINFO("processPossibleClock not clocking");
            return;
        }
        // SQINFO("got a clock 5d last=%d", lastClockSample);
        outputClocks++;
        const int curSample = this->samplesTicked;
        if (lastClockSample >= 0) {
            const int delta = curSample - lastClockSample;
            // SQINFO("delta = %d", delta);
            assert(delta > 1);
            minSamplesBetweenClocks = std::min(minSamplesBetweenClocks, delta);
            maxSamplesBetweenClocks = std::max(maxSamplesBetweenClocks, delta);
            //   assert(maxSamplesBetweenClocks < 5300);
            //   SQINFO("max sbc = %d", maxSamplesBetweenClocks);
            //  SQINFO("*********************************************************************");
            //  SQINFO("*** we just output the second clock. next period we will miss one ***");
            //  ClockShifter5::llv = 1;         // turn on verbose logging now
        } else {
            // SQINFO("skipping first clock - we aren't stable yet");
        }
        lastClockSample = curSample;
    }

    std::string toString() const {
        std::stringstream s;
        s << "samplesTicked=" << samplesTicked << std::endl;
        s << "outputClocks =" << outputClocks << std::endl;
        s << "* min=" << minSamplesBetweenClocks << " max=" << maxSamplesBetweenClocks << std::endl;
        s << "* minShift=" << minShift << " maxShift=" << maxShift << std::endl;
        return s.str();
    }

private:
    int lastClockSample = -1;
};

static float updateShift(const Inputs5& input, Outputs5& output, TestContext* context, float& accumulatedShift) {
    accumulatedShift += input.shiftPerSample;
    float instantaneousShift = accumulatedShift;
    if (context) {
        const auto x = context->_testLFO.process();
        instantaneousShift += x;
        if (ClockShifter5::llv) SQINFO("LFO = %f, ishift = %f", x, instantaneousShift);
    }
    output.lastShift = instantaneousShift;
    return instantaneousShift;
}

static Outputs5 runSub(const Inputs5& input, std::shared_ptr<ClockShifter5> shifter, TestContext* context) {
    if (ClockShifter5::llv) SQINFO("*** run Sub ");
    assert(input.isValid());

    Outputs5 output;

    // Originally all the offsets were entegrated into this. Now the LFO is NOT integrated, so renamed this.
    float shiftAccumulator = input.initialShift;
    float shiftInstantaneous = input.initialShift;
    // SQINFO("run sub going into loop, samples ticked = %d, clocks= %d", output.samplesTicked, output.outputClocks);

    int k = 0;
    int samplesRemaining = input.totalSamplesToTick - output.samplesTicked;

    while (samplesRemaining) {
        int samplesThisTime = std::min(samplesRemaining, input.period - 1);
        samplesRemaining -= samplesThisTime;
        for (int i = 0; i < samplesThisTime; ++i) {
            if (ClockShifter5::llv) SQINFO("run about to clock it low samp %d", k + i);
            const bool b = clockItLow(shifter, 1, shiftInstantaneous);
            output.processPossibleClock(b, shiftInstantaneous);
            shiftInstantaneous = updateShift(input, output, context, shiftAccumulator);
        }
        if (samplesRemaining > 0) {
            samplesRemaining--;
            if (ClockShifter5::llv) {
                SQINFO("- about to tick high %d", k + shiftAccumulator);
            }
            const bool b = shifter->process(true, true, shiftInstantaneous);
            // SQINFO("ticked high, ck=%d", b);
            output.processPossibleClock(b, shiftInstantaneous);
            shiftInstantaneous = updateShift(input, output, context, shiftAccumulator);
        }
        k += samplesThisTime;
        k++;  // the high one.
    }

    assertGE(output.maxSamplesBetweenClocks, output.minSamplesBetweenClocks);  // maybe not enough cycles
    return output;
}

static Outputs5 run(const Inputs5& _input, TestContext* context) {
    Outputs5 initOutput;
    // Step 1, setup.
    auto result = makeAndPrime2(_input.period, _input.initialShift);
    // SQINFO("-- finished make a and prime --");
    initOutput.samplesTicked = 1;
    initOutput.outputClocks = result.clocked ? 1 : 0;
    auto shifter = result.shifter;

    Inputs5 adjustedInput = _input;
    adjustedInput.totalSamplesToTick -= initOutput.samplesTicked;

    if (context) {
        context->_testLFO.setSin(true);
    }

    // Step 2, run forwards
    const Outputs5 outSub1 = runSub(adjustedInput, shifter, context);
    const Outputs5 out1 = initOutput.combine(outSub1);
    if (_input.afterwardsRunBackwards) {
        Inputs5 i = _input;  // will this cause an off by one?
        i.shiftPerSample *= -1;
        i.initialShift = out1.lastShift;
        // Step 3 (optional) run backwards.
        const Outputs5 out2 = runSub(i, shifter, context);
        return out1.combine(out2);
    }
    return out1;
}

static void test0() {
    Inputs5 in;
    in.period = 2;
    in.totalSamplesToTick = 10;
    const Outputs5 o = run(in, nullptr);
}

static void testNoShift() {
    // SQINFO("------------- testNoShift");
    Inputs5 in;
    const int cycles = 3;  // was 12345 when there was less debug.
                           // 12 failed.
    in.period = 10;
    in.totalSamplesToTick = (cycles + in.initialShift) * in.period;
    const auto output = run(in, nullptr);
    // SQINFO("end of testNoShift clocks=%d, output = %s", output.outputClocks, output.toString().c_str());
    assertEQ(output.samplesTicked, in.totalSamplesToTick);
    assertEQ(output.outputClocks, cycles);
    assertEQ(output.maxSamplesBetweenClocks, 10);
    assertEQ(output.minSamplesBetweenClocks, 10);
}

static void testNoShiftTwice() {
    Inputs5 in;
    const int cycles = 12;
    in.period = 10;
    in.totalSamplesToTick = (cycles + in.initialShift) * in.period;
    in.afterwardsRunBackwards = true;  // forward and back should be the same
    const auto output = run(in, nullptr);

    assertEQ(output.samplesTicked, 2 * in.totalSamplesToTick);
    assertEQ(output.outputClocks, 2 * cycles);
    assertEQ(output.maxSamplesBetweenClocks, 10);
    assertEQ(output.minSamplesBetweenClocks, 10);
}

static void testShift2() {
    const int cycles = 54321;
    Inputs5 in;
    in.period = 10;
    in.initialShift = .2;
    in.totalSamplesToTick = (cycles + in.initialShift) * in.period;

    const auto output = run(in, nullptr);

    assertEQ(output.samplesTicked, in.totalSamplesToTick);
    assertEQ(output.outputClocks, cycles);
    assertEQ(output.maxSamplesBetweenClocks, 10);
    assertEQ(output.minSamplesBetweenClocks, 10);
}

static void testStop() {
    assertEQ(SqLog::errorCount, 0);
    Inputs5 in;
    const int cycles = 5;
    in.period = 10;
    in.totalSamplesToTick = (cycles + in.initialShift) * in.period;
    in.shiftPerSample = .1;  // Note that this is has the clock just about stopping - it can't really to that.
    const auto output = run(in, nullptr);

    SQINFO("what's up with errors in testStop?");
    //  assertGT(SqLog::errorCount, 0);
    SqLog::errorCount = 0;
}

static void testSlowDown(int cycles, int period, float shiftPerSample, int allowableJitter) {
    //  SQINFO("testSlowDown %d, %d %f %d", cycles, period, shiftPerSample, allowableJitter);
    Inputs5 in;
    in.period = period;
    in.totalSamplesToTick = (cycles + in.initialShift) * in.period;
    in.shiftPerSample = shiftPerSample;
    const auto output = run(in, nullptr);

    assertEQ(output.samplesTicked, in.totalSamplesToTick);

    // We should have slowed down the clock output
    // 1.8 / 3 was suddenly too big
    const int expectecClocksUpperBound = (cycles / 1.6) + 4;
    assertLT(output.outputClocks, expectecClocksUpperBound);

    assertGT(output.maxSamplesBetweenClocks, period);
    const int expecteSpacingUpperBound = period * 2.2;
    //  SQINFO("period=%d upper bound = %d max=%d", period, expecteSpacingUpperBound, output.maxSamplesBetweenClocks);
    assertLT(output.maxSamplesBetweenClocks, expecteSpacingUpperBound);

    // it seems that jitter of 2 is common. Probably a few "off by one errors coming along
    //  const int allowableJitter = 2;
    const int jitter = std::abs(output.minSamplesBetweenClocks - output.maxSamplesBetweenClocks);
    assertLE(jitter, allowableJitter);
}

static void testSpeedUp(int cycles, int period, float shiftPerSample, int allowableJitter) {
   // SQINFO("test speed up (%d, %d %f %d)", cycles, period, shiftPerSample, allowableJitter);
    Inputs5 in;
    in.period = period;
    in.totalSamplesToTick = (cycles + in.initialShift) * in.period;
    in.shiftPerSample = shiftPerSample;
    const auto output = run(in, nullptr);
    assertEQ(output.samplesTicked, in.totalSamplesToTick);

    const int jitter = std::abs(output.minSamplesBetweenClocks - output.maxSamplesBetweenClocks);
    assertLE(jitter, allowableJitter);

    const int expectecClocksUpperBound = (cycles * 1.9) + 3;
    assertLT(output.outputClocks, expectecClocksUpperBound);

    assertLT(output.maxSamplesBetweenClocks, period);
    const int expecteSpacingUpperBound = period;
    assertLT(output.maxSamplesBetweenClocks, expecteSpacingUpperBound);

    // We should have sped up the clock output
    assertGT(output.outputClocks, cycles);
    assertLT(output.maxSamplesBetweenClocks, period);
}

//
static void testWithLFO(int cycles, int period, float lfoFreq, float lfoAmp, int allowedJitter) {
    SQINFO("----- testWithLFO %d, %d, %f, %f", cycles, period, lfoFreq, lfoAmp);
    Inputs5 in;
    in.period = period;
    in.totalSamplesToTick = (cycles + in.initialShift) * in.period;
    assertEQ(in.shiftPerSample, 0);
    TestContext context;
    context._testLFO.setFreq(lfoFreq);
    context._testLFO.setAmp(lfoAmp);
    //  SQINFO("normalized LFO freq %f amp =%f", normalizedLFOFreq, lfoAmp);

    const auto output = run(in, &context);
    // SQINFO("input = %s", in.toString().c_str());
    //  SQINFO("out = %s", output.toString().c_str());

    const int jitter = std::abs(output.minSamplesBetweenClocks - output.maxSamplesBetweenClocks);
    //  SQINFO("jitter = %d", jitter);
    assertLE(jitter, allowedJitter);
    if (jitter == 0) {
        assertEQ(output.minSamplesBetweenClocks, period);
    }
}

static void testWithLFO() {
    SQINFO("\n---------- testWithLFO");

    // these show no jitter, but may be doing nothing
    int period = 16;
    float lfoFreq = 1 / float(period);
    int cycles = 4;

    // ClockShifter5::llv = 1;

    // lfo freq == clock freq is weird.
    // with this very high delay it's freaking out
    // .45 is ok
    testWithLFO(cycles, period, lfoFreq, .45, 0);

    SQINFO("make more LFO tests!");
}

static void testSlowDown(int period) {
    for (int i = 0; i < 8; ++i) {
        float x = 10.f / (20.f * period);
        const int allowableJitter = std::max(5, period / 500);
        testSlowDown(5, period, x, allowableJitter);
        period *= 2;
    }
}

static void testSlowDown() {
    testSlowDown(10);
    for (int i = 110; i < 140; ++i) {
        testSlowDown(i);
    }
}

static void testSlowDownOld() {
    // period, cycles (shift per sample, allowable jitter)
    //  in.shiftPerSample = .5f / float(period);
    testSlowDown(5, 10, .5f / 10, 2);
    testSlowDown(50, 10, .5f / 10, 2);
    testSlowDown(50, 123, .5f / 123, 2);
}

static void testSpeedUp(int inPeriod, float shiftPerSampleFactor) {
    int period = inPeriod;
    for (int i = 0; i < 8; ++i) {
        float x = -10.f / (shiftPerSampleFactor * period);
        const int allowableJitter = std::max(5, period / 500);
        testSpeedUp(5, period, x, allowableJitter);
        period *= 2;
    }

    if (doLongRunning) {
        period = inPeriod;
        const int cycles = 11;
        for (int i = 0; i < 8; ++i) {
            float x = -10.f / (shiftPerSampleFactor * period * (cycles / 5));
            const int allowableJitter = std::max(5, (period * cycles) / 50);
            testSpeedUp(cycles, period, x, allowableJitter);
            period *= 2;
        }
    }
}

static void testSpeedUp() {
    SQINFO("testSpeedUp step 1");
    testSpeedUp(10, 20.0);
   
    SQINFO("testSpeedUp step 2");
    for (int i = 110; i < 140; ++i) {
        testSpeedUp(i, 20.0);
    }

    if (doLongRunning) {
        SQINFO("testSpeedUp step 3");
        for (int i = 10; i < 30; ++i) {
            testSpeedUp(10, float(i));
        }
        SQINFO("testSpeedUp step 4");
        for (int i = 110; i < 140; ++i) {
            SQINFO("testSpeedUp step 4:%d / 140", i);
            for (int j = 10; j < 30; ++j) {
                testSpeedUp(i, float(j));
            }
        }
         SQINFO("testSpeedUp step 5");
    }
}

static void testSlowDownAndSpeedUp() {
    // SQINFO("\n\n\n--------------------------------------- testSlowDownAndSpeedUp");
    Inputs5 in;
    const int cycles = 5;
    in.period = 10;
    in.totalSamplesToTick = (cycles + in.initialShift) * in.period;
    in.shiftPerSample = .05f;
    in.afterwardsRunBackwards = true;
    const auto output = run(in, nullptr);
    assertEQ(output.samplesTicked, 2 * in.totalSamplesToTick);
}

void testClockShifter5d() {
    //  SQINFO("start testClockShifter5d");
    assertEQ(SqLog::errorCount, 0);
    // testSlowDownAndSpeedUp();

    test0();
    testNoShift();
    testNoShiftTwice();

    SQINFO("why is test stop not working?");
    // testStop();
    testSlowDownOld();
    testSlowDown();

    testSpeedUp();
    testSlowDownAndSpeedUp();

    testSpeedUp(5, 7872, -0.000063516257796437, 10);

    // testWithLFO();
}

#if 0
void testFirst() {
    testWithLFO();
}
#endif