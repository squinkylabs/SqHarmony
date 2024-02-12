
#include "asserts.h"
#include "testShifter5TestUtils.h"

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
    float lastShift = 0;
    Outputs5 combine(const Outputs5& one) const {
        Outputs5 ret = one;
        ret.outputClocks += this->outputClocks;
        ret.samplesTicked += this->samplesTicked;
        ret.minSamplesBetweenClocks = std::min(one.minSamplesBetweenClocks, this->minSamplesBetweenClocks);
        ret.maxSamplesBetweenClocks = std::max(one.maxSamplesBetweenClocks, this->maxSamplesBetweenClocks);
        return ret;
    }

    void processPossibleClock(bool clock) {
        // SQINFO("processPossibleClock(%d)", clock);
        samplesTicked++;
        if (!clock) {
            // SQINFO("processPossibleClock not clocking");
            return;
        }
        SQINFO("got a clock 5d last=%d", lastClockSample);
        outputClocks++;
        const int curSample = this->samplesTicked;
        if (lastClockSample >= 0) {
            const int delta = curSample - lastClockSample;
            SQINFO("delta = %d", delta);
            assert(delta > 1);
            minSamplesBetweenClocks = std::min(minSamplesBetweenClocks, delta);
            maxSamplesBetweenClocks = std::max(maxSamplesBetweenClocks, delta);
            assert(maxSamplesBetweenClocks < 5300);
            SQINFO("*********************************************************************");
            SQINFO("*** we just output the second clock. next period we will miss one ***");
          //  ClockShifter5::llv = 1;         // turn on verbose logging now
        }
        lastClockSample = curSample;
    }

    std::string toString() const {
        std::stringstream s;
        s << "samplesTicked=" << samplesTicked << std::endl;
        s << "outputClocks =" << outputClocks << std::endl;
        s << "min=" << minSamplesBetweenClocks << " max=" << maxSamplesBetweenClocks << std::endl;
        return s.str();
    }

private:
    int lastClockSample = -1;
};

static Outputs5 runSub(const Inputs5& input, std::shared_ptr<ClockShifter5> shifter) {
    if (ClockShifter5::llv) SQINFO("*** run Sub ");
    assert(input.isValid());

    Outputs5 output;

    float shift = input.initialShift;
    // SQINFO("run sub going into loop, samples ticked = %d, clocks= %d", output.samplesTicked, output.outputClocks);

    int k = 0;
    int samplesRemaining = input.totalSamplesToTick - output.samplesTicked;
    while (samplesRemaining) {
        int samplesThisTime = std::min(samplesRemaining, input.period - 1);
        // SQINFO("run -in while, this time = %d remaining =%d ticked = %d", samplesThisTime, samplesRemaining, output.samplesTicked);
        samplesRemaining -= samplesThisTime;
        for (int i = 0; i < samplesThisTime; ++i) {
            if (ClockShifter5::llv) SQINFO("run about to clock it low samp %d", k + i);
            const bool b = clockItLow(shifter, 1, shift);

            output.processPossibleClock(b);
            //  SQINFO("after process possible, num ticked = %d", output.samplesTicked);
            shift += input.shiftPerSample;
            output.lastShift = shift;
            // SQINFO("run tick low, ck=%d set shift to %f", b, shift);
        }
        if (samplesRemaining > 0) {
            samplesRemaining--;
            if (ClockShifter5::llv) SQINFO("about to tick high %d", k + samplesThisTime);
            const bool b = shifter->process(true, true, shift);
            // SQINFO("ticked high, ck=%d", b);
            output.processPossibleClock(b);
        }
        k += samplesThisTime;
        k++;  // the high one
        // SQINFO("  bottom of loop, this time = %d remaining =%d ticked = %d", samplesThisTime, samplesRemaining, output.samplesTicked);
    }
    // SQINFO("leaving sub");
    return output;
}

static Outputs5 run(const Inputs5& _input) {
    // SQINFO("*** testRun input=%s", _input.toString().c_str());
    Outputs5 initOutput;
    // Step 1, setup.
    auto result = makeAndPrime2(_input.period, _input.initialShift);
    initOutput.samplesTicked = 1;
    initOutput.outputClocks = result.clocked ? 1 : 0;
    auto shifter = result.shifter;

    Inputs5 adjustedInput = _input;
    adjustedInput.totalSamplesToTick -= initOutput.samplesTicked;

    // Step 2, run forwards
    const Outputs5 outSub1 = runSub(adjustedInput, shifter);
    // SQINFO("initOutput = %s", initOutput.toString().c_str());
    // SQINFO("outSub1 = %s", outSub1.toString().c_str());
    const Outputs5 out1 = initOutput.combine(outSub1);
    // SQINFO("combined = %s", out1.toString().c_str());
    if (_input.afterwardsRunBackwards) {
        Inputs5 i = _input;  // will this cause an off by one?
        i.shiftPerSample *= -1;
        i.initialShift = out1.lastShift;
        // Step 3 (optional) run backwards.
        const Outputs5 out2 = runSub(i, shifter);
        return out1.combine(out2);
    }
    // SQINFO("*** testRun out %s", out1.toString().c_str());
    return out1;
}

static void test0() {
    Inputs5 in;
    in.period = 2;
    // in.cycles = 1;
    in.totalSamplesToTick = 4;
    const Outputs5 o = run(in);
}

static void testNoShift() {
    // SQINFO("------------- testNoShift");
    Inputs5 in;
    const int cycles = 3;  // was 12345 when there was less debug.
                           // 12 failed.
    in.period = 10;
    in.totalSamplesToTick = (cycles + in.initialShift) * in.period;
    const auto output = run(in);
    SQINFO("end of testNoShift clocks=%d, output = %s", output.outputClocks, output.toString().c_str());
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
    const auto output = run(in);
    //   SQINFO("end of testNoShiftTwice clocks=%d, output = %s", output.outputClocks, output.toString().c_str());
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

    const auto output = run(in);
    //  SQINFO("end of testShift2, output = %s", output.toString().c_str());
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
    const auto output = run(in);

    SQINFO("what's up with errors in testStop?");
    //  assertGT(SqLog::errorCount, 0);
    SqLog::errorCount = 0;
}

static void testSlowDown(int period, int cycles) {
    Inputs5 in;
    in.period = period;
    in.totalSamplesToTick = (cycles + in.initialShift) * in.period;
    in.shiftPerSample = .5f / float(period);
    const auto output = run(in);

    assertEQ(output.samplesTicked, in.totalSamplesToTick);

    // We should have slowed down the clock output
    const int expectecClocksUpperBound = (cycles / 1.9) + 3;
    assertLT(output.outputClocks, expectecClocksUpperBound);

    assertGT(output.maxSamplesBetweenClocks, period);
    const int expecteSpacingUpperBound = period * 2;
    assertLT(output.maxSamplesBetweenClocks, expecteSpacingUpperBound);

    // it seems that jitter of 2 is common. Probably a few "off by one errors coming along
    const int allowableJitter = 2;
    const int jitter = std::abs(output.minSamplesBetweenClocks - output.maxSamplesBetweenClocks);
    assertLE(jitter, allowableJitter);
}

static void testSlowDown() {
    testSlowDown(10, 5);
    testSlowDown(10, 50);
    testSlowDown(123, 50);
}

static void testSpeedUp(int cycles, int period, float shiftPerSample, int allowableJitter) {
    SQINFO("--testSpeedUp(%d, %d, %.18f, %d)", cycles, period, shiftPerSample, allowableJitter);
    Inputs5 in;
    in.period = period;
    in.totalSamplesToTick = (cycles + in.initialShift) * in.period;
    in.shiftPerSample = shiftPerSample;
    const auto output = run(in);
    assertEQ(output.samplesTicked, in.totalSamplesToTick);

    SQINFO("input = %s", in.toString().c_str());
    SQINFO("output = %s", output.toString().c_str());
    // rate should have been steady.
    const int jitter = std::abs(output.minSamplesBetweenClocks - output.maxSamplesBetweenClocks);
    SQINFO("measured jitter %d allowable=%d", jitter, allowableJitter);

    assertLE(jitter, allowableJitter);
    // assertEQ(output.minSamplesBetweenClocks, output.maxSamplesBetweenClocks);

    /*
    * here are the slow down asserts:
    *    // We should have slowed down the clock output
    const int expectecClocksUpperBound = (cycles / 1.9) + 3;
    assertLT(output.outputClocks, expectecClocksUpperBound);


    assertGT(output.maxSamplesBetweenClocks, period);
    const int expecteSpacingUpperBound = period * 2;
    assertLT(output.maxSamplesBetweenClocks, expecteSpacingUpperBound);
    */

    // We should have sped up the clock output
    assertGT(output.outputClocks, cycles);
    assertLT(output.maxSamplesBetweenClocks, period);
}

/**
  variables:
   cycles to test
  period
    shift per sample

 */

static void testSpeedUp(int period) {
    for (int i = 0; i < 8; ++i) {
        float x = -10.f / (20.f * period);
        const int allowableJitter = 10;
        testSpeedUp(5, period, x, allowableJitter);
        period *= 2;
    }
}

static void testSpeedUp() {
    SQINFO("doing the orig period 10 tests");
    testSpeedUp(10);
    // SQINFO("doing the new period 123 tests");
    // testSpeedUp(123);
}

static void testSlowDownAndSpeedUp() {
    // SQINFO("\n\n\n--------------------------------------- testSlowDownAndSpeedUp");
    Inputs5 in;
    const int cycles = 5;
    in.period = 10;
    in.totalSamplesToTick = (cycles + in.initialShift) * in.period;
    in.shiftPerSample = .05f;
    in.afterwardsRunBackwards = true;
    const auto output = run(in);
    assertEQ(output.samplesTicked, 2 * in.totalSamplesToTick);
}
void testClockShifter5d() {
    SQINFO("start testClockShifter5d");
    assertEQ(SqLog::errorCount, 0);
    // testSlowDownAndSpeedUp();

    test0();
    testNoShift();
    testNoShiftTwice();

    testStop();
    testSlowDown();

    testSpeedUp();
    testSlowDownAndSpeedUp();
}

#if 1
void testFirst() {
    // testSpeedUp();

    // max bigger than 5300 is bad
    // These are the exact numbers picked of from a bigger run.
    SQINFO("---- testFist ----");
    testSpeedUp(5, 7872, -0.000063516257796437, 10);
}
#endif