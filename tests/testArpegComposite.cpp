
#include "Arpeggiator.h"
#include "TestComposite.h"
#include "asserts.h"

using Comp = Arpeggiator<TestComposite>;
using ArpPtr = std::shared_ptr<Comp>;

ArpPtr make() {
    auto arp = std::make_shared<Comp>();
    return arp;
}

void connectInputs(ArpPtr arp, int numInputChannels) {
    assert(numInputChannels > 0);
    arp->inputs[Comp::CV_INPUT].channels = numInputChannels;
    arp->inputs[Comp::GATE_INPUT].channels = numInputChannels;
    arp->inputs[Comp::CLOCK_INPUT].channels = 1;
}

static void testNoGate() {
    auto arp = make();
    connectInputs(arp, 1);
    auto args = TestComposite::ProcessArgs();

    // set a cv input, no gate
    arp->inputs[Comp::CV_INPUT].value = 2;
    arp->inputs[Comp::CV2_INPUT].value = 3;
    arp->inputs[Comp::GATE_INPUT].value = 0;

    assertEQ(arp->outputs[Comp::CV_OUTPUT].value, 0);
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, 0);

    // Now generate a clock.
    arp->inputs[Comp::CLOCK_INPUT].value = 10;
    arp->process(args);

    // We should have no output, since no gate.
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, 0);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].value, 0);
    assertEQ(arp->outputs[Comp::CV2_OUTPUT].value, 0);
}

/**
 * @brief do a full clock cycle, leave click high, processed
 *
 * @param arp
 */
static void clockCycle(ArpPtr arp) {
    //  SQDEBUG("test cycle clock about to set clock low and process");
    auto args = TestComposite::ProcessArgs();
    arp->inputs[Comp::CLOCK_INPUT].value = 0;
    arp->process(args);
    //   SQDEBUG("test cycle clock about to set clock high and process");
    arp->inputs[Comp::CLOCK_INPUT].value = 10;
    assert(arp->inputs[Comp::CLOCK_INPUT].getVoltage(0) == 10);
    arp->process(args);
}

static void testGate() {
    auto arp = make();
    connectInputs(arp, 1);
    auto args = TestComposite::ProcessArgs();

    arp->inputs[Comp::CV_INPUT].value = 2;
    arp->inputs[Comp::CV2_INPUT].value = 22;
    arp->inputs[Comp::GATE_INPUT].value = 10;

    assertEQ(arp->outputs[Comp::CV_OUTPUT].value, 0);
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, 0);

    clockCycle(arp);

    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, cGateOutHi);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].value, 2);
    assertEQ(arp->outputs[Comp::CV2_OUTPUT].value, 22);
}

static void testHold() {
    auto arp = make();
    connectInputs(arp, 1);
    auto args = TestComposite::ProcessArgs();

    arp->params[Comp::HOLD_PARAM].value = 1;
    // SQDEBUG("Just set hold param to %f", arp->params[Comp::HOLD_PARAM].value);
    arp->inputs[Comp::CV_INPUT].value = 2;
    arp->inputs[Comp::CV2_INPUT].value = 22;
    arp->inputs[Comp::GATE_INPUT].value = 10;

    assertEQ(arp->outputs[Comp::CV_OUTPUT].value, 0);
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, 0);

    // Generate a clock.
    clockCycle(arp);

    // now lower the gate input
    arp->inputs[Comp::GATE_INPUT].value = 0;

    clockCycle(arp);

    // with hold on, should still have a gate.
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, cGateOutHi);

    arp->params[Comp::HOLD_PARAM].value = 0;  // turn hold off
    arp->process(args);
    arp->process(args);  // make sure it gets seen.

    clockCycle(arp);

    // with hold of, gate should go away
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, cGateOutLow);
}

static void testNotesBeforeClock() {
    auto arp = make();
    connectInputs(arp, 2);
    auto args = TestComposite::ProcessArgs();

    arp->inputs[Comp::CV_INPUT].value = 2;
    arp->inputs[Comp::CV2_INPUT].value = 200;
    arp->inputs[Comp::GATE_INPUT].value = 10;

    assertEQ(arp->outputs[Comp::CV_OUTPUT].value, 0);
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, 0);

    // add first note (gate high)
    arp->process(args);
    arp->process(args);

    // still no clock
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, 0);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].value, 0);

    // add second note
    arp->inputs[Comp::CV_INPUT].setVoltage(3.f, 1);
    arp->inputs[Comp::CV2_INPUT].setVoltage(303.f, 1);
    arp->inputs[Comp::GATE_INPUT].setVoltage(5.f, 1);
    arp->process(args);
    arp->process(args);

    // now two notes in arp, but still no clock
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, 0);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].value, 0);

    // first clock
    arp->inputs[Comp::CLOCK_INPUT].value = 10;
    arp->process(args);

    // should output first note
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, cGateOutHi);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].value, 2);

    //  clock low
    arp->inputs[Comp::CLOCK_INPUT].value = 0;
    arp->process(args);
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, 0);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].value, 2);

    // second clock
    arp->inputs[Comp::CLOCK_INPUT].value = 10;
    arp->process(args);

    // should output second note
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, cGateOutHi);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].value, 3);
}

static void testSort() {
    auto arp = make();
    connectInputs(arp, 2);
    auto args = TestComposite::ProcessArgs();

    arp->inputs[Comp::CV_INPUT].value = 2;
    arp->inputs[Comp::CV2_INPUT].value = 112;
    arp->inputs[Comp::GATE_INPUT].value = 10;

    assertEQ(arp->outputs[Comp::CV_OUTPUT].value, 0);
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, 0);

    // add first note (gate high)
    arp->process(args);

    // add second note of a lower pitch
    arp->inputs[Comp::CV_INPUT].setVoltage(1.f, 1);
    arp->inputs[Comp::CV2_INPUT].setVoltage(117.f, 1);
    arp->inputs[Comp::GATE_INPUT].setVoltage(5.f, 1);
    arp->process(args);

    // first clock
    arp->inputs[Comp::CLOCK_INPUT].value = 10;
    arp->process(args);

    // should output second note because it's lower pitch
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, cGateOutHi);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].value, 1.f);
}

static void testReset(bool resetMode) {
    auto arp = make();
    connectInputs(arp, 3);
    arp->params[Comp::RESET_MODE_PARAM].value = resetMode ? 1.f : 0.f;
    auto args = TestComposite::ProcessArgs();
    arp->process(args);

    arp->inputs[Comp::RESET_INPUT].value = 0;
    arp->inputs[Comp::CLOCK_INPUT].value = 0;

    // send in two notes, 2v and 3v
    arp->inputs[Comp::CV_INPUT].setVoltage(2, 0);
    arp->inputs[Comp::CV2_INPUT].setVoltage(223, 0);
    arp->inputs[Comp::GATE_INPUT].setVoltage(10, 0);

    arp->inputs[Comp::CV_INPUT].setVoltage(3, 1);
    arp->inputs[Comp::CV2_INPUT].setVoltage(33, 1);
    arp->inputs[Comp::GATE_INPUT].setVoltage(10, 1);

    arp->inputs[Comp::CV_INPUT].setVoltage(4, 2);
    arp->inputs[Comp::CV2_INPUT].setVoltage(44, 2);
    arp->inputs[Comp::GATE_INPUT].setVoltage(10, 2);

    // clock in the voltages
    arp->process(args);
    // now buffer has 2, 3, 4

    // clock
    arp->inputs[Comp::CLOCK_INPUT].value = 10;
    arp->process(args);
    float x = arp->outputs[Comp::CV_OUTPUT].getVoltage(0);
    assertEQ(x, 2);

    arp->inputs[Comp::CLOCK_INPUT].value = 0;
    arp->process(args);

    arp->inputs[Comp::CLOCK_INPUT].value = 10;
    arp->process(args);
    x = arp->outputs[Comp::CV_OUTPUT].getVoltage(0);
    assertEQ(x, 3);

    arp->inputs[Comp::CLOCK_INPUT].value = 0;
    arp->process(args);

    arp->inputs[Comp::CLOCK_INPUT].value = 10;
    arp->process(args);
    x = arp->outputs[Comp::CV_OUTPUT].getVoltage(0);
    assertEQ(x, 4);

    arp->inputs[Comp::CLOCK_INPUT].value = 0;
    arp->process(args);

    // now it comes back to first step, v=2
    arp->inputs[Comp::CLOCK_INPUT].value = 10;
    arp->process(args);
    x = arp->outputs[Comp::CV_OUTPUT].getVoltage(0);
    assertEQ(x, 2);

    arp->inputs[Comp::CLOCK_INPUT].value = 0;
    arp->process(args);

    // and second step, v=3
    arp->inputs[Comp::CLOCK_INPUT].value = 10;
    arp->process(args);
    x = arp->outputs[Comp::CV_OUTPUT].getVoltage(0);
    assertEQ(x, 3);

    // Q a reset
    arp->inputs[Comp::RESET_INPUT].value = 10;
    arp->process(args);

    // now different expectations for different modes
    if (resetMode) {
        // in nord mode, to output doesn't change yet, the reset is queued for next clock
        // same output
        x = arp->outputs[Comp::CV_OUTPUT].getVoltage(0);
        assertEQ(x, 3);

        // clock cycle
        arp->inputs[Comp::CLOCK_INPUT].value = 0;
        arp->process(args);
        arp->inputs[Comp::CLOCK_INPUT].value = 10;
        arp->process(args);

        x = arp->outputs[Comp::CV_OUTPUT].getVoltage(0);
        assertEQ(x, 2);
    } else {
        // in classic reset, we immediately go back to the first step
        x = arp->outputs[Comp::CV_OUTPUT].getVoltage(0);
        assertEQ(x, 2);

        // clock cycle
        arp->inputs[Comp::CLOCK_INPUT].value = 0;
        arp->process(args);
        arp->inputs[Comp::CLOCK_INPUT].value = 10;
        arp->process(args);

        // since this clock is right after the reset, it's supresssed
        x = arp->outputs[Comp::CV_OUTPUT].getVoltage(0);
        assertEQ(x, 2);

        // clock cycle
        arp->inputs[Comp::CLOCK_INPUT].value = 0;
        arp->process(args);
        arp->inputs[Comp::CLOCK_INPUT].value = 10;
        arp->process(args);

        // since this clock is right after the reset, it's supresssed
        x = arp->outputs[Comp::CV_OUTPUT].getVoltage(0);
        assertEQ(x, 2);

        for (int i = 0; i < 100; ++i) {
            arp->process(args);
        }
        // clock cycle
        arp->inputs[Comp::CLOCK_INPUT].value = 0;
        arp->process(args);
        arp->inputs[Comp::CLOCK_INPUT].value = 10;
        arp->process(args);

        // since this clock is right after the reset, it's supresssed
        x = arp->outputs[Comp::CV_OUTPUT].getVoltage(0);
        assertEQ(x, 3);
    }

#if 0

    assert(false);

    assertEQ(arp->outputs[Comp::CV_OUTPUT].value, 0);
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, 0);

    arp->inputs[Comp::CLOCK_INPUT].value = 10;
    arp->process(args);

    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, 5);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].value, 2);
#endif
}

static void testNoDelay2() {
    auto arp = make();
    connectInputs(arp, 1);
    auto args = TestComposite::ProcessArgs();

    // set clock and gate low, cv = 7/77
    arp->params[Comp::GATE_DELAY_PARAM].value = 0.f;
    arp->inputs[Comp::CLOCK_INPUT].setVoltage(0, 0);
    // 7v in
    arp->inputs[Comp::CV_INPUT].setVoltage(7, 0);
    arp->inputs[Comp::CV2_INPUT].setVoltage(77, 0);
    arp->inputs[Comp::GATE_INPUT].setVoltage(0, 0);

    arp->process(args);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 0);

    // now clock high, gate high, new CV
    arp->inputs[Comp::CLOCK_INPUT].setVoltage(10, 0);
    arp->inputs[Comp::CV_INPUT].setVoltage(8, 0);
    arp->inputs[Comp::CV2_INPUT].setVoltage(88, 0);
    arp->inputs[Comp::GATE_INPUT].setVoltage(10, 0);  // set the mono gate high, let in the notes

    arp->process(args);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 8);
    assertEQ(arp->outputs[Comp::CV2_OUTPUT].getVoltage(0), 88);
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].getVoltage(0), 10);
}

static void testDelay2() {
    // clock, then gate after 1, then cv after another.
    // should still see correct  output, as above
    auto arp = make();
    connectInputs(arp, 1);
    auto args = TestComposite::ProcessArgs();

    arp->params[Comp::GATE_DELAY_PARAM].value = 1.f;

    // set clock and gate low, cv = 7/77
    arp->inputs[Comp::CLOCK_INPUT].setVoltage(0, 0);
    arp->inputs[Comp::CV_INPUT].setVoltage(1, 0);
    arp->inputs[Comp::CV2_INPUT].setVoltage(11, 0);
    arp->inputs[Comp::GATE_INPUT].setVoltage(0, 0);

    // process 0: all low
    arp->process(args);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 0);

    // now raise the clock, and set CV to 2
    arp->inputs[Comp::CLOCK_INPUT].setVoltage(10, 0);
    arp->inputs[Comp::CV_INPUT].setVoltage(2, 0);
    arp->inputs[Comp::CV2_INPUT].setVoltage(22, 0);

    // process 1: clock high, cv=2
    arp->process(args);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 0);

    // now raise the  gate
    arp->inputs[Comp::GATE_INPUT].setVoltage(10, 0);
    arp->inputs[Comp::CV_INPUT].setVoltage(3, 0);
    arp->inputs[Comp::CV2_INPUT].setVoltage(33, 0);

    // process 2: clock high, gate high, cv to 3
    arp->process(args);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 0);

    // now set CV to desired 4
    arp->inputs[Comp::CV_INPUT].setVoltage(4, 0);
    arp->inputs[Comp::CV2_INPUT].setVoltage(44, 0);

    // process three more times
    // process 3,4,5
    arp->process(args);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 0);
    arp->process(args);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 0);
    arp->process(args);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 0);

// at i == 5 be finally get the right pitch
#if 1
    for (int i = 0; i < 10; ++i) {
        arp->process(args);
        const float x = arp->outputs[Comp::CV_OUTPUT].getVoltage(0);
        const float y = arp->outputs[Comp::GATE_OUTPUT].getVoltage(0);
        // SQINFO("i = %d, x = %f", i, x);
        const float expected = (i >= 5) ? 4.f : 0.f;
        assertEQ(x, expected);

        const float expectedGate = (i >= 5) ? 10.f : 0.f;
        assertEQ(y, expectedGate);
    }
#else
    arp->process(args);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 4);
#endif
}

static void testTriggerDelay(bool delayOn) {
    auto arp = make();
    connectInputs(arp, 16);
    auto args = TestComposite::ProcessArgs();

    arp->params[Comp::GATE_DELAY_PARAM].value = delayOn ? 1.f : 0.f;
    arp->inputs[Comp::CLOCK_INPUT].setVoltage(0, 0);
    // 7v in
    arp->inputs[Comp::CV_INPUT].setVoltage(7, 0);
    arp->inputs[Comp::CV2_INPUT].setVoltage(77, 0);
    arp->inputs[Comp::GATE_INPUT].setVoltage(10, 0);  // set the mono gate high, let in the notes
    arp->process(args);                               // proc once to see that 7v
    // now input to 2
    arp->inputs[Comp::CV_INPUT].setVoltage(2, 0);

    // make clock high, then low, three cycles
    for (int i = 0; i < 3; ++i) {
        arp->process(args);
        arp->inputs[Comp::CLOCK_INPUT].setVoltage(10, 0);
        arp->process(args);
        arp->inputs[Comp::CLOCK_INPUT].setVoltage(0, 0);
    }

    if (delayOn) {
        for (int i = 0; i < 8; ++i) {
            arp->process(args);
            const float x = arp->outputs[Comp::CV_OUTPUT].getVoltage(0);
            const float expected = i < 5 ? 0.f : 2.f;
            assertEQ(x, expected);
        }
        return;
    }

    // should have caught the first voltage
    if (delayOn) {
    } else {
        // with no delay, we saw the get right before the clock.
        assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 7);
    }
}

static void testReleaseMidClock(bool gatedClock) {
    auto arp = make();
    connectInputs(arp, 1);
    auto args = TestComposite::ProcessArgs();

    // high gate
    arp->inputs[Comp::CV_INPUT].value = 2;
    arp->inputs[Comp::CV2_INPUT].value = 22;
    arp->inputs[Comp::GATE_INPUT].value = 10;

    arp->params[Comp::GATE_CLOCKED_PARAM].value = gatedClock ? 1.f : 0.f;

    // clock l then h
    clockCycle(arp);

    // we expect new note (already tested);
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, cGateOutHi);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].value, 2);

    // clock still high, take gate down.
    arp->inputs[Comp::GATE_INPUT].value = 0;
    arp->process(args);

    // now expect gate stays high. even without the extra gate logic, it does this.
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, cGateOutHi);

    // clock goes down, should shut off
    arp->inputs[Comp::CLOCK_INPUT].value = 0;
    arp->process(args);
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, cGateOutLow);

    // clock goes high again, should stay off
    arp->inputs[Comp::CLOCK_INPUT].value = 10;
    arp->process(args);
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, cGateOutLow);
}

static void testStartMidClock(bool gatedClock) {
    auto arp = make();
    connectInputs(arp, 1);
    auto args = TestComposite::ProcessArgs();

    // start with low gate.
    arp->inputs[Comp::CV_INPUT].value = 2;
    arp->inputs[Comp::CV2_INPUT].value = 22;
    arp->inputs[Comp::GATE_INPUT].value = cGateLow;

    arp->params[Comp::GATE_CLOCKED_PARAM].value = gatedClock ? 1.f : 0.f;

    // clock l cycle, expect still low out.
    clockCycle(arp);
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, cGateOutLow);
    clockCycle(arp);
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, cGateOutLow);

    // make sure clock is low
    arp->inputs[Comp::CLOCK_INPUT].value = cGateOutLow;
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, cGateOutLow);

    // now raise gate
    arp->inputs[Comp::GATE_INPUT].value = cGateHi;
    clockCycle(arp);

    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, cGateOutLow);
}

static void testShuffleCV() {
    // SQINFO("\n\n***** testShuffleCV");
    auto arp = make();
    connectInputs(arp, 1);

    arp->inputs[Comp::CV_INPUT].channels = 4;               // make this poly pitch for easier test
    arp->inputs[Comp::SHUFFLE_TRIGGER_INPUT].channels = 1;  // connect the new shuffle trigger
    auto args = TestComposite::ProcessArgs();

    // start with gate high
    arp->inputs[Comp::CV_INPUT].setVoltage(10, 0);  // 10 v first channel
    arp->inputs[Comp::CV_INPUT].setVoltage(11, 1);
    arp->inputs[Comp::CV_INPUT].setVoltage(12, 2);
    arp->inputs[Comp::CV_INPUT].setVoltage(13, 3);
    arp->inputs[Comp::CV2_INPUT].setVoltage(101, 0);
    arp->inputs[Comp::CV2_INPUT].setVoltage(111, 1);
    arp->inputs[Comp::CV2_INPUT].setVoltage(121, 2);
    arp->inputs[Comp::CV2_INPUT].setVoltage(131, 3);
    arp->inputs[Comp::GATE_INPUT].setVoltage(cGateOutHi, 0);
    arp->params[Comp::MODE_PARAM].value = float(ArpegPlayer::Mode::SHUFFLE);
    // SQINFO("--- test will process once to pick up the notes");
    arp->process(args);

    // SQINFO("--- about to clock 4");
    float output[4] = {0};
    for (int i = 0; i < 4; ++i) {
        clockCycle(arp);
        output[i] = arp->outputs[Comp::CV_OUTPUT].getVoltage(0);
        // SQINFO("just clocked %d", i);
    }
    // SQINFO("--- second time clock 4");

    // second time should be the same, since the shuffle input is connected,
    // but not active.
    bool allTheSame = true;
    for (int i = 0; i < 4; ++i) {
        clockCycle(arp);
        if (output[i] != arp->outputs[Comp::CV_OUTPUT].getVoltage(0)) {
            allTheSame = false;
        }
        // SQINFO("just clocked %d", i);
    }

    // since trigger input connected, should not re-shuffle each time
    assert(allTheSame);

    // This time we will activate the trigger, but
    // it won't take effect until next cycle
    // NFO("\n--- third time clock 4, with trigger");
    allTheSame = true;
    for (int i = 0; i < 4; ++i) {
        clockCycle(arp);
        if (output[i] != arp->outputs[Comp::CV_OUTPUT].getVoltage(0)) {
            allTheSame = false;
        }
        // SQINFO("just clocked %d", i);

        const float trigV = (i == 0) ? 10.f : 0.f;
        arp->inputs[Comp::SHUFFLE_TRIGGER_INPUT].setVoltage(trigV, 0);
    }
    assert(allTheSame);

    allTheSame = true;
    for (int i = 0; i < 4; ++i) {
        clockCycle(arp);
        if (output[i] != arp->outputs[Comp::CV_OUTPUT].getVoltage(0)) {
            allTheSame = false;
        }
        // SQINFO("just clocked %d", i);

        // const float trigV = (i == 0) ? 10.f : 0.f;
        // arp->inputs[Comp::SHUFFLE_TRIGGER_INPUT].setVoltage(trigV, 0);
    }
    assert(!allTheSame);
}

static void testPullCable() {
    auto arp = make();
    connectInputs(arp, 1);

    arp->inputs[Comp::CV_INPUT].channels = 2;
    auto args = TestComposite::ProcessArgs();

    // put in some voltages
    arp->inputs[Comp::CV_INPUT].setVoltage(11, 0);
    arp->inputs[Comp::CV_INPUT].setVoltage(12, 1);

    // get all the voltages in
    arp->process(args);

    arp->inputs[Comp::GATE_INPUT].setVoltage(10, 0);

    // clock should play something
    clockCycle(arp);
    assertGT(arp->outputs[Comp::GATE_OUTPUT].getVoltage(0), 5);

    // now pull the gates. should turn off.
    arp->inputs[Comp::GATE_INPUT].channels = 0;
    arp->process(args);
    assertLT(arp->outputs[Comp::GATE_OUTPUT].getVoltage(0), 5);

    assert(false);
}

ArpPtr makeMonoGate4Voice() {
    auto arp = make();
    auto args = TestComposite::ProcessArgs();

    // set up a mono gate, but poly cv input
    arp->inputs[Comp::CV_INPUT].channels = 4;
    arp->inputs[Comp::CV2_INPUT].channels = 4;
    arp->inputs[Comp::GATE_INPUT].channels = 1;
    arp->inputs[Comp::CLOCK_INPUT].channels = 1;
    return arp;
}

// Puts 8 CV into all 4 voices and gates them in.
void setup4Pitches(ArpPtr arp, const TestComposite::ProcessArgs& args) {
    // set up the poly pitch
    arp->inputs[Comp::CV_INPUT].setVoltage(10, 0);
    arp->inputs[Comp::CV_INPUT].setVoltage(11, 1);
    arp->inputs[Comp::CV_INPUT].setVoltage(12, 2);
    arp->inputs[Comp::CV_INPUT].setVoltage(13, 3);

    arp->inputs[Comp::CV2_INPUT].setVoltage(100, 0);
    arp->inputs[Comp::CV2_INPUT].setVoltage(101, 1);
    arp->inputs[Comp::CV2_INPUT].setVoltage(102, 2);
    arp->inputs[Comp::CV2_INPUT].setVoltage(103, 3);

    // gate in the input
    arp->inputs[Comp::GATE_INPUT].setVoltage(cGateOutHi, 0);  // set the mono gate high, let in the notes
    arp->inputs[Comp::CLOCK_INPUT].setVoltage(0, 0);
    arp->process(args);

    // We don't expect a gate out until there is a clock.
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].getVoltage(0), 0);
}

static void clockCycles(int cycles, ArpPtr arp, const TestComposite::ProcessArgs& args) {
    assertGT(cycles, 0);
    for (int i = 0; i < cycles; ++i) {
        arp->inputs[Comp::CLOCK_INPUT].setVoltage(cGateOutLow, 0);
        arp->process(args);

        // Does the gate always go low on clock down? I think so?
        assertEQ(arp->outputs[Comp::GATE_OUTPUT].getVoltage(0), cGateOutLow);
        arp->inputs[Comp::CLOCK_INPUT].setVoltage(cGateOutHi, 0);
        arp->process(args);
        assertEQ(arp->outputs[Comp::GATE_OUTPUT].getVoltage(0), cGateOutHi);
    }
}

static void clockOneCycle(ArpPtr arp, const TestComposite::ProcessArgs& args) {
    clockCycles(1, arp, args);
}

static void testMonoGateSub(bool changeVoices) {
    auto arp = makeMonoGate4Voice();
    auto args = TestComposite::ProcessArgs();

    setup4Pitches(arp, args);

    // clock with 4 notes loaded?
    arp->inputs[Comp::CLOCK_INPUT].setVoltage(cGateOutHi, 0);
    arp->process(args);

    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 10);
    assertEQ(arp->outputs[Comp::CV2_OUTPUT].getVoltage(0), 100);
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].getVoltage(0), cGateOutHi);

    // clock again
    clockOneCycle(arp, args);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 11);
    assertEQ(arp->outputs[Comp::CV2_OUTPUT].getVoltage(0), 101);

    // clock again
    clockOneCycle(arp, args);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 12);
    assertEQ(arp->outputs[Comp::CV2_OUTPUT].getVoltage(0), 102);

    // clock again
    clockOneCycle(arp, args);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 13);
    assertEQ(arp->outputs[Comp::CV2_OUTPUT].getVoltage(0), 103);

    // clock again
    clockOneCycle(arp, args);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 10);
    assertEQ(arp->outputs[Comp::CV2_OUTPUT].getVoltage(0), 100);

    if (!changeVoices) {
        return;
    }

    assert(false);
#if 0
        // set poly to 3
    arp->inputs[Comp::CV_INPUT].channels = 3;

#endif
}

static void testMonoGate() {
    testMonoGateSub(false);
}

enum PolyphonyChange {
    ChangeNotPlayingTo3,
    ChangePlayingTo3,
    ChangeNotPlayingTo5,
    ChangeNotPlayingTo3Clock

};

static void testMonoGateSub2(PolyphonyChange e) {
 //   assertEQ(e, ChangeNotPlayingTo3);
    auto arp = makeMonoGate4Voice();
    auto args = TestComposite::ProcessArgs();

    // first clock around until we have the "right" note playing
    // One clock to get first voice playing, 4 more to cycle around
    const int initialClocks = (e != ChangePlayingTo3) ? 1 + 4 : 1 +3;
    setup4Pitches(arp, args);
    clockCycles(initialClocks, arp, args);

    const float expectedCV = (e != ChangePlayingTo3) ? 10 : 13;
    const float expectedCV2 = (e != ChangePlayingTo3) ? 100 : 103;
    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), expectedCV);
    assertEQ(arp->outputs[Comp::CV2_OUTPUT].getVoltage(0), expectedCV2);

    // set poly to 3
    arp->inputs[Comp::CV_INPUT].channels = 3;
    // clock till playing the third note
    clockCycles(2, arp, args);

    // If we aren't playing the disconnected pitch, then we stay were we are and cycle,
    // so 10 stays, and two more cycles go to 12.
    // But if we hav2 to move off it, then we would be moving from 3, down to 2 (12), 
    // then first clock goes to 0 (10), then 1 (11)
    const float expectedCVAfterPolyChange = (e != ChangePlayingTo3) ? 12 : 11;
    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), expectedCVAfterPolyChange);

    // if was (12 or 11) Should not play 13 (vx 3), since it should be removed.
    const int expectedClocksToComeAround = (e != ChangePlayingTo3) ? 1 : 2;
    clockCycles(expectedClocksToComeAround, arp, args);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 10);
}

static void testMonoGateChangeNotPlayingTo3() {
    testMonoGateSub2(ChangeNotPlayingTo3);
}

static void testMonoGateChangePlayingTo3() {
    testMonoGateSub2(ChangePlayingTo3);
}

static void testMonoGateChangeNotPlayingTo5() {
    testMonoGateSub2(ChangeNotPlayingTo5);
}

void testArpegComposite() {
    // TODO: remove this early case
    testDelay2();

    testShuffleCV();
    // printf("imp testNoGate\n");
    testNoGate();
    testGate();
    testHold();
    testNotesBeforeClock();
    testSort();

    testReset(false);
    testReset(true);  // nord mode

    testReleaseMidClock(false);
    testReleaseMidClock(true);
    testStartMidClock(false);
    testShuffleCV();

    testNoDelay2();
    testDelay2();
    testTriggerDelay(false);
    testTriggerDelay(true);

    testMonoGate();
    testMonoGateChangeNotPlayingTo3();
    testMonoGateChangePlayingTo3();
    testMonoGateChangeNotPlayingTo5();

    SQWARN("!!!! put back the pull cable test");
    // testPullCable();
}