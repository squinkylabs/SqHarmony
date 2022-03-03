
#include "OneShot.h"
#include "SeqClock.h"
#include "asserts.h"

static void testOneShotInit() {
    OneShot o;
    // should always be fired, until triggered
    assert(o.hasFired());
    o.setDelayMs(1);
    assert(o.hasFired());
    o.setSampleTime(1.f / 44100.f);
    assert(o.hasFired());
    o.set();
    assert(!o.hasFired());
}

static void testOneShot2Ms() {
    OneShot o;
    o.setSampleTime(.001f);  // sample rate 1k
    o.setDelayMs(2);         // delay 2ms
    assert(o.hasFired());
    o.set();
    assert(!o.hasFired());

    o.step();  // 1 ms.
    assert(!o.hasFired());

    o.step();  // 2 ms.
    assert(o.hasFired());

    for (int i = 0; i < 10; ++i) {
        o.step();
        assert(o.hasFired());
    }

    o.set();
    assert(!o.hasFired());

    o.step();  // 1 ms.
    assert(!o.hasFired());

    o.step();  // 2 ms.
    assert(o.hasFired());
}

template <typename TClock>
static void testClockExtEdge() {
    // auto rate = TClock::ClockRate::Div1;
    const double metricTimePerClock = 1;
    TClock ck;
    typename TClock::ClockResults results;
    ck.setup(100);  // internal clock

    // send one clock (first low)
    for (int i = 0; i < 10; ++i) {
        results = ck.updateMulti(55, 0, true, 0);  // low clock
        assert(!results.didClock);                 // in start up
    }

    // then high once to start at time zero
    results = ck.updateMulti(55, 10, true, 0);
    assert(results.didClock);

    // then low once to get ready for next clock
    results = ck.updateMulti(55, 0, true, 0);
    assert(!results.didClock);

    // then high once
    results = ck.updateMulti(55, 10, true, 0);
    assert(results.didClock);

    // then high some more
    for (int i = 0; i < 10; ++i) {
        results = ck.updateMulti(55, 10, true, 0);  // low clock

        assert(!results.didClock);
        // assertEQ(results.totalElapsedTime, metricTimePerClock);
    }

    // low more
    for (int i = 0; i < 10; ++i) {
        results = ck.updateMulti(55, 0, true, 0);  // low clock

        assert(!results.didClock);
        // assertEQ(results.totalElapsedTime, metricTimePerClock);
    }

    // then high some more
    bool oneMoreClock = false;
    for (int i = 0; i < 10; ++i) {
        results = ck.updateMulti(55, 10, true, 0);  // low clock
                                                    //   assert(results.didClock);
        if (results.didClock) {
            oneMoreClock = true;
        }

        // assertEQ(results.totalElapsedTime, 2 * metricTimePerClock);
    }
    assert(oneMoreClock);
}

template <typename TClock>
static void testClockChangeWhileStopped() {
    const int sampleRateI = 44100;
    const float sampleRate = float(sampleRateI);
    const float sampleTime = 1.f / sampleRate;

    TClock ck;
    ck.setup(sampleTime);  // external clock

    // call with clock low,high,low whole running
    // to get to time zero, ready for first
    typename TClock::ClockResults results;
    ck.updateMulti(sampleRateI, 0, true, 0);
    ck.updateMulti(sampleRateI, 10, true, 0);
    results = ck.updateMulti(sampleRateI, 0, true, 0);

    assert(!results.didClock);

    // now stop
    results = ck.updateMulti(sampleRateI, 0, false, 0);
    assert(!results.didClock);

    // raise clock while stopped
    for (int i = 0; i < 10; ++i) {
        results = ck.updateMulti(sampleRateI, 10, false, 0);
    }
    assert(!results.didClock);

    // now run. see if we catch the edge
    results = ck.updateMulti(sampleRateI, 10, true, 0);
    assert(results.didClock);
}

template <typename TClock>
static void testSimpleResetIgnoreClock() {
    const int sampleRateI = 44100;
    const float sampleRate = float(sampleRateI);
    const float sampleTime = 1.f / sampleRate;

    TClock ck;
    SeqClock::ClockResults results;
    ck.setup(SeqClock::ClockRate::Div16, 120, sampleTime);  // external clock tempo 120

    // run external clock high
    results = ck.update(sampleRateI, 10, true, 0);
    assert(!results.didReset);
    // const double t0 = results.totalElapsedTime;
    const bool ck0 = results.didClock;

    // clock low and high
    ck.update(sampleRateI, 0, true, 0);
    results = ck.update(sampleRateI, 10, true, 0);
    const bool ck1 = results.didClock;

    assert(false);
    // assertGT(t1, t0);  // we are clocking now

    // now reset
    // Note that this resets clock while it is running, then runs
    results = ck.update(sampleRateI, 10, true, 10);
    assert(results.didReset);
    assertEQ(results.didClock, false);  // reset should set clock back to waiting

    results = ck.update(sampleRateI, 0, true, 0);
    assert(!results.didReset);

    //   ClockResults update(int samplesElapsed, float externalClock, bool runStop, float reset);

    // clock should be locked out now
    results = ck.update(1, 10, true, 0);
    assert(!results.didReset);
}

template <typename TClock>
static double getExpectedResetTime() {
    assert(false);
    return -10;
}

template <>
static double getExpectedResetTime<SeqClock>() {
    return -1;
}

template <typename TClock>
static void testResetIgnoreClock(bool holdClockHigh) {
    const int sampleRateI = 44100;
    const float sampleRate = float(sampleRateI);
    const float sampleTime = 1.f / sampleRate;

    int samplesInOneMs = int(44100.f / 1000.f);

    TClock ck;
    typename TClock::ClockResults results;
    ck.setup(sampleTime);  // external clock = quarter

    // run external clock high
    results = ck.updateMulti(sampleRateI, 10, true, 0);
    assert(!results.didReset);

    //  assert(false);
    // const double t0 = results.totalElapsedTime;

    // clock low and high
    ck.updateMulti(sampleRateI, 0, true, 0);
    results = ck.updateMulti(sampleRateI, 10, true, 0);
    assert(results.didClock);

    // now reset
    results = ck.updateOnce(10, true, 10);
    assert(results.didReset);
    assert(!results.didClock);

    // step for a little under one ms
    int errorMargin = 10;
    for (int i = 0; i < (samplesInOneMs - errorMargin); ++i) {
        results = ck.updateOnce(0, true, 0);
        assert(!results.didClock);
    }

    // this clock should be ignored
    results = ck.updateOnce(10, true, 0);
    assert(!results.didClock);

    if (!holdClockHigh) {
        // step for a little more with clock low
        for (int i = 0; i < 2 * errorMargin; ++i) {
            results = ck.updateOnce(0, true, 0);
            assert(!results.didClock);
        }

        // this clock should NOT be ignored
        results = ck.updateMulti(sampleRateI, 10, true, 0);
        assert(results.didClock);  // first clock after reset advance to start
    }
    else {
        // coming into here, we ignored the low to high clock transition.
        // not let's hold it high - should still ignore
        for (int i = 0; i < 2 * errorMargin; ++i) {
            results = ck.updateOnce(10, true, 0);
            assert(!results.didClock);
        }

        // now real low to high
        results = ck.updateOnce(0, true, 0);
        assert(!results.didClock);
         results = ck.updateOnce(10, true, 0);
        assert(results.didClock);
        
    }
}

template <typename TClock>
static void testNoNoteAfterReset() {
    const int sampleRateI = 44100;
    const float sampleRate = float(sampleRateI);
    const float sampleTime = 1.f / sampleRate;

    TClock ck;
    typename TClock::ClockResults results;
    ck.setup(sampleTime);

    // clock it a bit
    for (int j = 0; j < 10; ++j) {
        results = ck.updateMulti(100, 0, true, 0);
        results = ck.updateMulti(100, 10, true, 0);
    }
    //    ClockResults updateMulti(int samplesElapsed, float externalClock, bool runStop, float reset);

    // stop it
    results = ck.updateOnce(0, false, 0);
    assert(!results.didReset);

    // reset it
    results = ck.updateOnce(0, false, 10);
    assert(results.didReset);
    assert(!results.didClock);

    // while we are stopped, time should still not pass, even if clocked
    for (int i = 0; i < 100; ++i) {
        // clock low
        results = ck.updateMulti(100, 0, false, 0);
        assert(!results.didReset);
        assert(!results.didClock);
        //  assertLT(results.totalElapsedTime, getExpectedResetTime<TClock>() + 1);

        // clock high
        results = ck.updateMulti(100, 10, false, 0);
        assert(!results.didReset);
        assert(!results.didClock);
    }

    // now let it run (but no clock)
    for (int i = 0; i < 100; ++i) {
        results = ck.updateMulti(100, 0, true, 0);
        assert(!results.didReset);
        assert(!results.didClock);
    }
}

template <typename TClock>
static void testRunGeneratesClock() {
    const int sampleRateI = 44100;
    const float sampleRate = float(sampleRateI);
    const float sampleTime = 1.f / sampleRate;

    TClock ck;  // freq new clock
    typename TClock::ClockResults results;
    ck.setup(sampleTime);  // external clock = quarter

    // clock with run off, clock high. Nothing happens
    for (int j = 0; j < 10; ++j) {
        results = ck.updateMulti(100, 10, false, 0);
        assert(!results.didReset);
        assert(!results.didClock);
    }

    // now run. rising run signal should gen a clock with clock high
    results = ck.updateMulti(1, 10, true, 0);
    assert(!results.didReset);
    assert(results.didClock);
}

#if 0   // I don't think this test is valid. We don't want this behavior
        // 2/15/22
template <typename TClock>
static void testResetRetriggersClock() {
    const int sampleRateI = 44100;
    const float sampleRate = float(sampleRateI);
    const float sampleTime = 1.f / sampleRate;

    TClock ck;  // freq new clock
    typename TClock::ClockResults results;
    ck.setup(sampleTime);  // external clock = quarter

    // clock with run off, clock high. Nothing happens
    for (int j = 0; j < 10; ++j) {
        results = ck.updateMulti(100, 10, false, 0);
        assert(!results.didReset);
        assert(!results.didClock);
    }

    // now run. rising run signal should gen a clock with clock high
    results = ck.updateOnce(10, true, 0);
    assert(!results.didReset);
    assert(results.didClock);

    // stay high should not generate more clocks
    for (int j = 0; j < 10; ++j) {
        results = ck.updateMulti(100, 10, true, 0);
        assert(!results.didReset);
        assert(!results.didClock);
    }

    // reset should re-trigger clock, but only after lock-out interval
    results = ck.updateOnce(10, true, 10);
    assert(results.didReset);
    assert(!results.didClock);

    // now wait through the lock-out interval
    const int lockOutSamples = 1000;  // TODO: expose magic num from clock

    // assert(false); // following stuff ng
    results = ck.updateMulti(lockOutSamples, 10, true, 0);
    assert(results.didClock);
    // assertEQ(results.totalElapsedTime, getExpectedResetTime<TClock>() + 1);
}
#endif

static void testResetNord() {
    const int sampleRateI = 44100;
    const float sampleRate = float(sampleRateI);
    const float sampleTime = 1.f / sampleRate;

    SeqClock ck;
    SeqClock::ClockResults results;
    ck.setup(sampleTime);  // external clock = quarter
    ck.setResetMode(true);

    // run clock high to get rid of spurious clock
    ck.updateMulti(1, 10, true, 0);
    // run clock high a long time
    results = ck.updateMulti(2000, 10, true, 0);
    assert(!results.didReset);
    assert(!results.didClock);

    //  ClockResults updateMulti(int samplesElapsed, float externalClock, bool runStop, float reset);

    // now reset low to high - request reset
    results = ck.updateMulti(2000, 10, true, 10);
    assert(!results.didReset);
    assert(!results.didClock);

    // clock low (reset sill queued)
    results = ck.updateOnce(0, true, 10);
    assert(!results.didReset);
    assert(!results.didClock);

    // clock rising edge (reset sill queued)
    results = ck.updateOnce(10, true, 10);
    assert(results.didReset);
    assert(results.didClock);

    //  assert(false);
}

static void testGet(bool resetMode) {
    const int sampleRateI = 44100;
    const float sampleRate = float(sampleRateI);
    const float sampleTime = 1.f / sampleRate;

    SeqClock ck;
    SeqClock::ClockResults results;
    ck.setup(sampleTime);  // external clock = quarter
    ck.setResetMode(resetMode);

    auto x = ck.updateOnce(0, true, 0);
    assert(!x.didClock);
    assert(!ck.getClockValue());

    x = ck.updateOnce(0, true, 0);
    assert(!x.didClock);
    assert(!ck.getClockValue());

    x = ck.updateOnce(10, true, 0);
    assert(x.didClock);
    assert(ck.getClockValue());

    x = ck.updateOnce(10, true, 0);
    assert(!x.didClock);
    assert(ck.getClockValue());
}

// cGateLow
static void testHysteresis() {
    const int sampleRateI = 44100;
    const float sampleRate = float(sampleRateI);
    const float sampleTime = 1.f / sampleRate;

    SeqClock ck;
    SeqClock::ClockResults results;
    ck.setup(sampleTime);  // external clock = quarter

    const float indeterminateClock = .5f * (cGateLow + cGateHi);

    auto x = ck.updateOnce(0, true, 0);
    assert(!x.didClock);
    assert(!ck.getClockValue());

    x = ck.updateOnce(indeterminateClock, true, 0);
    assert(!ck.getClockValue());
    assert(!x.didClock);

    x = ck.updateOnce(indeterminateClock, true, 0);
    assert(!ck.getClockValue());
    assert(!x.didClock);

    x = ck.updateOnce(10, true, 0);
    assert(ck.getClockValue());
    assert(x.didClock);

    x = ck.updateOnce(indeterminateClock, true, 0);
    assert(ck.getClockValue());
    assert(!x.didClock);

    x = ck.updateOnce(cGateLow - .1f, true, 0);
    assert(!ck.getClockValue());
    assert(!x.didClock);
}

static void testResetGoesAway(bool nordMode) {
    const int sampleRateI = 44100;
    const float sampleRate = float(sampleRateI);
    const float sampleTime = 1.f / sampleRate;

    SeqClock ck;
    SeqClock::ClockResults results;
    ck.setup(sampleTime);  // external clock = quarter
    ck.setResetMode(nordMode);

    // run clock high to get rid of spurious clock
    ck.updateMulti(1, 10, true, 0);
    // run clock high a long time
    results = ck.updateMulti(2000, 10, true, 0);
    assert(!results.didReset);
    assert(!results.didClock);

    // then low
    results = ck.updateMulti(2000, 0, true, 0);
    assert(!results.didReset);
    assert(!results.didClock);

    //  ClockResults updateMulti(int samplesElapsed, float externalClock, bool runStop, float reset);
    
    if (!nordMode) {

        // now reset low to high - request reset
        results = ck.updateOnce(0, true, 10);
        assert(results.didReset);
        assert(!results.didClock);

        // reset again
        results = ck.updateOnce(0, true, 10);
        //  assert(results.didReset);
        assert(!results.didClock);

        // now reset low. shouldhave no more
        results = ck.updateOnce(0, true, 0);
        assert(!results.didReset);
        assert(!results.didClock);
    }
    else {
        // now reset low to high - request reset
        results = ck.updateOnce(0, true, 10);
        assert(!results.didReset);
        assert(!results.didClock);

        // now clock it in
        results = ck.updateOnce(10, true, 10);
        assert(results.didReset);
       // assert(!results.didClock);

        // second time
        results = ck.updateOnce(10, true, 10);
        assert(!results.didReset);
    }

    // now keep reset low - should have no more
    // bug was making more resets come out here.
    for (int i=0; i<100; ++i) {
        // clock high
        results = ck.updateOnce(10, true, 0);
        assert(!results.didReset);
         // clock low
        results = ck.updateOnce(0, true, 0);
        assert(!results.didReset);

    }
}


template <typename T>
void testSeqClock2() {
    testClockExtEdge<T>();
    testClockChangeWhileStopped<T>();
    testResetIgnoreClock<T>(false);
    testResetIgnoreClock<T>(true);
    testNoNoteAfterReset<T>();
    testRunGeneratesClock<T>();
  //  testResetRetriggersClock<T>();
}

void testSeqClock() {
    testSeqClock2<SeqClock>();
    testOneShotInit();
    testOneShot2Ms();
    testResetNord();
    testGet(false);
    testGet(true);
    testHysteresis();
    testResetGoesAway(false);
    testResetGoesAway(true);
}