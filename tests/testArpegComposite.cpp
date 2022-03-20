
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
    SQDEBUG("---- testNoGate");
    auto arp = make();
    connectInputs(arp, 1);
    auto args = TestComposite::ProcessArgs();

    // set a cv input, no gate
    arp->inputs[Comp::CV_INPUT].value = 2;
    arp->inputs[Comp::CV2_INPUT].value = 3;
    arp->inputs[Comp::GATE_INPUT].value = 0;

    assertEQ(arp->outputs[Comp::CV_OUTPUT].value, 0);
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, 0);

    // now click in the gate
    arp->inputs[Comp::CLOCK_INPUT].value = 10;
    SQDEBUG("test about to call proc");
    arp->process(args);

    // we should have no output, since no gate
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, 0);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].value, 0);
    assertEQ(arp->outputs[Comp::CV2_OUTPUT].value, 0);
}

static void clockCycle(ArpPtr arp) {
    SQDEBUG("test cycle clock about to set clock low and process");
    auto args = TestComposite::ProcessArgs();
    arp->inputs[Comp::CLOCK_INPUT].value = 0;
    arp->process(args);
    SQDEBUG("test cycle clock about to set clock higth and process");
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
    //SQNFO("------------ testHold()");
    auto arp = make();
    connectInputs(arp, 1);
    auto args = TestComposite::ProcessArgs();

    arp->params[Comp::HOLD_PARAM].value = 1;
    SQDEBUG("Just set hold param to %f", arp->params[Comp::HOLD_PARAM].value);
    arp->inputs[Comp::CV_INPUT].value = 2;
    arp->inputs[Comp::CV2_INPUT].value = 22;
    arp->inputs[Comp::GATE_INPUT].value = 10;

    assertEQ(arp->outputs[Comp::CV_OUTPUT].value, 0);
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, 0);

    SQDEBUG("test about to fire clock, have gate input set high");
    // clock in the gate
    clockCycle(arp);

    // now lower the gate input
    arp->inputs[Comp::GATE_INPUT].value = 0;

    SQDEBUG("test about to fire clock, have gate input set low");
    clockCycle(arp);

    // with hold on, should still have a gate.
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].value, cGateOutHi);
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

static void testMonoGate() {
    auto arp = make();
    auto args = TestComposite::ProcessArgs();

    // set up a mono gate, but poly cv input
    arp->inputs[Comp::CV_INPUT].channels = 4;
    arp->inputs[Comp::CV2_INPUT].channels = 4;
    arp->inputs[Comp::GATE_INPUT].channels = 1;
    arp->inputs[Comp::CLOCK_INPUT].channels = 1;

    // poly pitch
    arp->inputs[Comp::CV_INPUT].setVoltage(10, 0);
    arp->inputs[Comp::CV_INPUT].setVoltage(11, 1);
    arp->inputs[Comp::CV_INPUT].setVoltage(12, 2);
    arp->inputs[Comp::CV_INPUT].setVoltage(13, 3);

    arp->inputs[Comp::CV2_INPUT].setVoltage(10, 0);
    arp->inputs[Comp::CV2_INPUT].setVoltage(11, 1);
    arp->inputs[Comp::CV2_INPUT].setVoltage(12, 2);
    arp->inputs[Comp::CV2_INPUT].setVoltage(13, 3);

    arp->inputs[Comp::GATE_INPUT].setVoltage(10, 0);  // set the mono gate high, let in the notes
    arp->inputs[Comp::CLOCK_INPUT].setVoltage(0, 0);
    arp->process(args);
    assertEQ(arp->outputs[Comp::GATE_OUTPUT].getVoltage(0), 0);

    // clock in 4 notes?
    arp->inputs[Comp::CLOCK_INPUT].setVoltage(10, 0);
    arp->process(args);

    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 10);

    // clock again
    arp->inputs[Comp::CLOCK_INPUT].setVoltage(0, 0);
    arp->process(args);
    arp->inputs[Comp::CLOCK_INPUT].setVoltage(10, 0);
    arp->process(args);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 11);

    // clock again
    arp->inputs[Comp::CLOCK_INPUT].setVoltage(0, 0);
    arp->process(args);
    arp->inputs[Comp::CLOCK_INPUT].setVoltage(10, 0);
    arp->process(args);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 12);

    // clock again
    arp->inputs[Comp::CLOCK_INPUT].setVoltage(0, 0);
    arp->process(args);
    arp->inputs[Comp::CLOCK_INPUT].setVoltage(10, 0);
    arp->process(args);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 13);

    // clock again
    arp->inputs[Comp::CLOCK_INPUT].setVoltage(0, 0);
    arp->process(args);
    arp->inputs[Comp::CLOCK_INPUT].setVoltage(10, 0);
    arp->process(args);
    assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 10);
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

    for (int i = 0; i < 3; ++i) {
        arp->process(args);
        arp->inputs[Comp::CLOCK_INPUT].setVoltage(10, 0);
        arp->process(args);
        arp->inputs[Comp::CLOCK_INPUT].setVoltage(0, 0);
    }

    // should have caught the first voltage
    if (delayOn) {
        // with  delay, we saw the second, stable CV
        assertEQ(arp->outputs[Comp::CV_OUTPUT].getVoltage(0), 2);
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

    arp->inputs[Comp::CV_INPUT].channels = 4;       // make this poly pitch for easier test
    arp->inputs[Comp::SHUFFLE_TRIGGER_INPUT].channels = 1;      // connect the new shuffle trigger
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
    //SQINFO("--- test will process once to pick up the notes");
    arp->process(args);

    //SQINFO("--- about to clock 4");
    float output[4] = { 0 };
    for (int i = 0; i < 4; ++i) {
        clockCycle(arp);
        output[i] = arp->outputs[Comp::CV_OUTPUT].getVoltage(0);
        //SQINFO("just clocked %d", i);
    }
    //SQINFO("--- second time clock 4");

    // second time should be the same, since the shuffle input is connected,
    // but not active.
    bool allTheSame = true;
    for (int i = 0; i < 4; ++i) {
        clockCycle(arp);
        if (output[i] != arp->outputs[Comp::CV_OUTPUT].getVoltage(0)) {
            allTheSame = false;
        }
        //SQINFO("just clocked %d", i);
    }

    // since trigger input connected, should not re-shuffle each time
    assert(allTheSame);


    // This time we will activate the trigger, but
    // it won't take effect until next cycle
    //NFO("\n--- third time clock 4, with trigger");
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
        //SQINFO("just clocked %d", i);
       
       // const float trigV = (i == 0) ? 10.f : 0.f;
       // arp->inputs[Comp::SHUFFLE_TRIGGER_INPUT].setVoltage(trigV, 0);
  
    }
    assert(!allTheSame);
}

void testArpegComposite() {
    testShuffleCV();
    // printf("imp testNoGate\n");
    testNoGate();
    testGate();
    testHold();
    testNotesBeforeClock();
    testSort();

    testReset(false);
    testReset(true);  // nord mode
    testMonoGate();
    testTriggerDelay(false);
    testTriggerDelay(true);

    testReleaseMidClock(false);
    testReleaseMidClock(true);

    testStartMidClock(false);

    testShuffleCV();
}