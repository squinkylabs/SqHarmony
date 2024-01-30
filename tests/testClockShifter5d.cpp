
#include "asserts.h"
#include "testShifter5TestUtils.h"

class Inputs {
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

class Outputs {
public:
    int outputClocks = 0;
    int samplesTicked = 0;
    int minSamplesBetweenClocks = 100000;
    int maxSamplesBetweenClocks = 0;
    float lastShift = 0;
    Outputs combine(const Outputs& one) const {
        Outputs ret = one;
        ret.outputClocks += this->outputClocks;
        ret.samplesTicked += this->samplesTicked;
        ret.minSamplesBetweenClocks = std::min(one.minSamplesBetweenClocks, this->minSamplesBetweenClocks);
        ret.maxSamplesBetweenClocks = std::max(one.maxSamplesBetweenClocks, this->maxSamplesBetweenClocks);
        return ret;
    }

    void processPossibleClock(bool clock) {
        //SQINFO("processPossibleClock(%d)", clock);
        samplesTicked++;
        if (!clock) {
            //SQINFO("processPossibleClock not clocking");
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
        s << "samplesTicked=" << samplesTicked << std::endl;
        s << "outputClocks =" << outputClocks << std::endl;
        s << "min=" << minSamplesBetweenClocks << " max=" << maxSamplesBetweenClocks << std::endl;
        return s.str();
    }

private:
    int lastClockSample = -1;
};

static Outputs runSub(const Inputs& input, std::shared_ptr<ClockShifter5> shifter) {
    SQINFO("*** run Sub ");
    assert(input.isValid());

    Outputs output;

    float shift = input.initialShift;
    //SQINFO("run sub going into loop, samples ticked = %d, clocks= %d", output.samplesTicked, output.outputClocks);

    int samplesRemaining = input.totalSamplesToTick - output.samplesTicked;
    while (samplesRemaining) {
        int samplesThisTime = std::min(samplesRemaining, input.period - 1);
        // SQINFO("run -in while, this time = %d remaining =%d ticked = %d", samplesThisTime, samplesRemaining, output.samplesTicked);
        samplesRemaining -= samplesThisTime;
        for (int i = 0; i < samplesThisTime; ++i) {
            //SQINFO("run about to clock it low samp %d this time=%d", i, samplesThisTime);
            const bool b = clockItLow(shifter, 1, shift);

            output.processPossibleClock(b);
          //  SQINFO("after process possible, num ticked = %d", output.samplesTicked);
            shift += input.shiftPerSample;
            output.lastShift = shift;
            // SQINFO("run tick low, ck=%d set shift to %f", b, shift);
        }
        if (samplesRemaining > 0) {
            samplesRemaining--;
            //SQINFO("about to tick high");
            const bool b = shifter->process(true, true, shift);
           // SQINFO("ticked high, ck=%d", b);
            output.processPossibleClock(b);
        }
        // SQINFO("  bottom of loop, this time = %d remaining =%d ticked = %d", samplesThisTime, samplesRemaining, output.samplesTicked);
    }
    //SQINFO("leaving sub");
    return output;
}

static Outputs run(const Inputs& _input) {
    SQINFO("*** testRun %s", _input.toString().c_str());
    Outputs initOutput;
    // Step 1, setup.
    auto result = makeAndPrime2(_input.period, _input.initialShift);
    initOutput.samplesTicked = 1;
    initOutput.outputClocks = result.clocked ? 1 : 0;
    auto shifter = result.shifter;

    Inputs adjustedInput = _input;
    adjustedInput.totalSamplesToTick -= initOutput.samplesTicked;

    // Step 2, run forwards
    const Outputs outSub1 = runSub(adjustedInput, shifter);
   // SQINFO("initOutput = %s", initOutput.toString().c_str());
   // SQINFO("outSub1 = %s", outSub1.toString().c_str());
    const Outputs out1 = initOutput.combine(outSub1);
   // SQINFO("combined = %s", out1.toString().c_str());
    if (_input.afterwardsRunBackwards) {
        Inputs i = _input;          // will this cause an off by one?
        i.shiftPerSample *= -1;
        i.initialShift = out1.lastShift;
        // Step 3 (optional) run backwards.
        const Outputs out2 = runSub(i, shifter);
        return out1.combine(out2);
    }
    return out1;
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
    SQINFO("------------- testNoShift");
    Inputs in;
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
    Inputs in;
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

static void testStop() {
    assertEQ(SqLog::errorCount, 0);
    Inputs in;
    const int cycles = 5;
    in.period = 10;
    in.totalSamplesToTick = (cycles + in.initialShift) * in.period;
    in.shiftPerSample = .1;  // not that this is has the clock just about stopping - it can't really to that.
    const auto output = run(in);

    SQINFO("what's up with errors in testStop?");
   // assertGT(SqLog::errorCount, 0);
    SqLog::errorCount = 0;
}

static void testSlowDown() {
    Inputs in;
    const int cycles = 5;
    in.period = 10;
    in.totalSamplesToTick = (cycles + in.initialShift) * in.period;
    in.shiftPerSample = .05f;
    const auto output = run(in);
    assertEQ(output.samplesTicked, in.totalSamplesToTick);

    // We should have slowed down the clock output
    assertLT(output.outputClocks, cycles);
    assertGT(output.maxSamplesBetweenClocks, 10);

    // rate should have been steady.
    assertEQ(output.minSamplesBetweenClocks, output.maxSamplesBetweenClocks);
}

static void testSpeedUp() {
    SQINFO("------- testSpeedUp");
    Inputs in;
    const int cycles = 5;
    in.period = 10;
    in.totalSamplesToTick = (cycles + in.initialShift) * in.period;
    in.shiftPerSample = -.05f;
    const auto output = run(in);
    assertEQ(output.samplesTicked, in.totalSamplesToTick);

   
    // rate should have been steady.
    assertEQ(output.minSamplesBetweenClocks, output.maxSamplesBetweenClocks);

    // We should have slowed down the clock output
    assertLT(output.outputClocks, cycles);
    assertGT(output.maxSamplesBetweenClocks, 10);
}
static void testSlowDownAndSpeedUp() {
   // SQINFO("\n\n\n--------------------------------------- testSlowDownAndSpeedUp");
    Inputs in;
    const int cycles = 5;
    in.period = 10;
    in.totalSamplesToTick = (cycles + in.initialShift) * in.period;
    in.shiftPerSample = .05f;
    in.afterwardsRunBackwards = true;
    const auto output = run(in);
  //  SQINFO("end of testSlowDownAndSpeedUp, output = %s", output.toString().c_str());
  //  SQINFO("input was %s", in.toString().c_str());
  //  SQINFO("Need to add more assertions----");
  //  assert(false);      // finish with assertions
    //  SQINFO("real end");
    assertEQ(output.samplesTicked, 2 * in.totalSamplesToTick);

    // We should have slowed down the clock output
    // assertLT(output.outputClocks, cycles);
    //  assertGT(output.maxSamplesBetweenClocks, 10);

    // rate should have been steady.
    // assertEQ(output.minSamplesBetweenClocks, output.maxSamplesBetweenClocks);
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

void testFirst() {
    testSpeedUp();
}