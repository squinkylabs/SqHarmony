
#include <tuple>

#include "TestLFO.h"
#include "asserts.h"
#include "testShifter6TestUtils.h"

/**
 * These tests are almost all tests the run over a longer period
 * of time, and tend to exercise "real world" scenarios.
 * Every time the show a bug, a more traditional unit tests should be
 * made to test that particular function.
 */
extern bool doLongRunning;

class SignalSourceInterface {
public:
    virtual bool getClock() const = 0;
    virtual float getDelay() const = 0;
    virtual bool isMoreData() const = 0;
    virtual void next() = 0;
    virtual unsigned getPeriod() = 0;
    virtual std::tuple<float, float> getMinMaxDelay() = 0;
};

class SignalSource : public SignalSourceInterface {
public:
    bool getClock() const override {
        const bool ret = _clockAcc < _clockHighSamples;
        return ret;
    }
    float getDelay() const override {
        return _currentDelay;
    }
    bool isMoreData() const override {
        return _samplesRemaining > 0;
    }
    void next() override {
        assert(isMoreData());
        _generateNextSample();
    }
    unsigned getPeriod() override {
        return _clockPeriod;
    }

    std::tuple<float, float> getMinMaxDelay() override {
        return std::make_tuple(_minDelay, _maxDelay);
    }

    void initClock(unsigned period, float dutycycle, unsigned duration) {
        assert(dutycycle > 0);
        assert(dutycycle < 100);

        _clockHighSamples = (period * dutycycle) / 100;
        _clockLowSamples = period - _clockHighSamples;
        assert(period == (_clockLowSamples + _clockHighSamples));
        assert(_samplesRemaining == 0 || _samplesRemaining == duration);
        _samplesRemaining = duration;
        _clockPeriod = period;
    }

    void initForDelayRamp(float initial, float final, unsigned duration) {
        assert(initial != final);
        _sampleCounter = 0;
        assert(_samplesRemaining == 0 || _samplesRemaining == duration);
        _samplesRemaining = duration;

        _currentDelay = initial;
        _updateDelay();

        _deltaDelayPerSample = (final - initial) / duration;
        assert(_deltaDelayPerSample != 0);
    }

    void initForConstant(float value, unsigned duration) {
        _sampleCounter = 0;
        _samplesRemaining = duration;
        _currentDelay = value;
        _updateDelay();
        _deltaDelayPerSample = 0;
    }

private:
    // These bars for clock
    unsigned _clockHighSamples = 0;
    unsigned _clockLowSamples = 0;
    unsigned _clockAcc = 0;
    // These vars for implementing ramp / const
    double _currentDelay = 0;
    unsigned _sampleCounter = 0;
    int _samplesRemaining = 0;
    double _deltaDelayPerSample = 0;
    unsigned _clockPeriod = 0;
    float _minDelay = 10000;
    float _maxDelay = -10000;

    void _generateNextSample() {
        _sampleCounter++;
        _samplesRemaining--;
        _currentDelay += _deltaDelayPerSample;
        _updateDelay();
        ++_clockAcc;
        if (_clockAcc >= (_clockHighSamples + _clockLowSamples)) {
            _clockAcc = 0;
        }
    }

    void _updateDelay() {
        _minDelay = std::min(_minDelay, float(_currentDelay));
        _maxDelay = std::max(_maxDelay, float(_currentDelay));
    }
};

class Output6 {
public:
    int outputClocks = 0;
    int minSamplesBetweenClocks = 100000;
    int maxSamplesBetweenClocks = 0;
    int previousMinSamplesBetweenClocks = 100000;
    int previousMaxSamplesBetweenClocks = 0;

    bool lastClock = false;
    int lastTriggerSample = -1;
    void onClock(bool ck, unsigned sample) {
        const bool risingEdge = ck && !lastClock;
        if (risingEdge) {
            if (lastTriggerSample >= 0) {
                int delta = sample - lastTriggerSample;
                minSamplesBetweenClocks = std::min(minSamplesBetweenClocks, delta);
                maxSamplesBetweenClocks = std::max(maxSamplesBetweenClocks, delta);
                if (logLevel) {
                    SQINFO("delta = %d, min=%u max=%u", delta, minSamplesBetweenClocks, maxSamplesBetweenClocks);

                    if ((maxSamplesBetweenClocks != previousMaxSamplesBetweenClocks) || (minSamplesBetweenClocks != previousMinSamplesBetweenClocks)) {
                        SQINFO("!! new delay value: %d and %d delta=%d",
                               minSamplesBetweenClocks,
                               maxSamplesBetweenClocks,
                               maxSamplesBetweenClocks - minSamplesBetweenClocks);
                        previousMaxSamplesBetweenClocks = maxSamplesBetweenClocks;
                        previousMinSamplesBetweenClocks = minSamplesBetweenClocks;
                    }
                }
            }
            lastTriggerSample = sample;
        }
        lastClock = ck;
    }
};

static void run(SignalSourceInterface* source, Output6* output) {
    unsigned period = source->getPeriod();
    float delay = source->getDelay();
    auto shifter = makeAndPrime(period, delay);

    bool lastClock = false;
    unsigned sample = 0;
    for (bool done = false; !done;) {
        if (source->isMoreData()) {
            const bool clock = source->getClock();
            //   const bool trigger = clock && !lastClock;
            float delay = source->getDelay();
            ClockShifter6::Errors err;
            const bool newClock = shifter->process(clock, delay, &err);
            assert(err == ClockShifter6::Errors::NoError);
            lastClock = clock;

            source->next();
            output->onClock(newClock, sample);
            ++sample;
        } else {
            done = true;
        }
    }
    assert(output->maxSamplesBetweenClocks >= output->minSamplesBetweenClocks);
    auto const x = source->getMinMaxDelay();
    assert(std::get<1>(x) >= std::get<0>(x));

    // SQINFO("at end of run, min/max=%f, %f", std::get<0>(source->getMinMaxDelay()), std::get<1>(source->getMinMaxDelay()));
}

static void testCanCall() {
    SignalSource s;
    s.initForDelayRamp(0, 1, 100);
    s.initClock(10, 50, 100);
    Output6 results;
    run(&s, &results);
}

static void testDelayRamp() {
    const unsigned dur = 100;
    SignalSource s;
    s.initForDelayRamp(0, 1, 100);
    float x = -.001;
    for (int i = 0; i < 100; ++i) {
        assertEQ(s.isMoreData(), true);
        const float y = s.getDelay();
        assertGT(y, x);
        x = y;
        assertGE(y, 0);
        assertLT(y, 1);
        s.next();
    }
    assertClose(x, .99, .005);
    assertEQ(s.isMoreData(), false);
}

static void testConstantRamp() {
    const unsigned dur = 100;
    SignalSource s;
    s.initForConstant(4, 7);
    for (int i = 0; i < 7; ++i) {
        assertEQ(s.isMoreData(), true);
        const float y = s.getDelay();
        assertEQ(y, 4);
        s.next();
    }
    assertEQ(s.isMoreData(), false);
}

static void testClockGen(float dutyCycle) {
    //  void initClock(int period, float dutycycle)
    SignalSource s;

    const int period = 200;
    s.initClock(period, dutyCycle, 500);
    const unsigned clocksHigh = period * dutyCycle / 100;
    const unsigned clocksLow = period * (100 - dutyCycle) / 100;

    for (unsigned i = 0; i < clocksHigh; ++i) {
        const bool x = s.getClock();
        assertEQ(x, true);
        s.next();
    }
    for (unsigned i = 0; i < clocksLow; ++i) {
        const bool x = s.getClock();
        assertEQ(x, false);
        s.next();
    }
    for (unsigned i = 0; i < clocksHigh; ++i) {
        const bool x = s.getClock();
        assertEQ(x, true);
        s.next();
    }
}

static void testClockGen() {
    testClockGen(50);
    testClockGen(10);
}

static void testRunZero(unsigned period) {
    SignalSource src;
    Output6 output;

    unsigned duration = period * 5;
    src.initClock(period, 50, duration);
    src.initForConstant(0, duration);
    run(&src, &output);
    assert(output.maxSamplesBetweenClocks == period);
    assert(output.maxSamplesBetweenClocks == output.minSamplesBetweenClocks);
}

static void testRunZero() {
    testRunZero(10);
    testRunZero(113);
    testRunZero(4321);
}

static void testRunRamp(unsigned duration, float initDelay, float finalDelay, unsigned period, unsigned acceptableJitter) {
    // SQINFO("\ntestRunRamp(%u, %f, %f, %u)", duration, initDelay, finalDelay, period);
    SignalSource src;
    Output6 output;

    src.initClock(period, 50, duration);
    src.initForDelayRamp(initDelay, finalDelay, duration);  // delay one clock over 8 input
    run(&src, &output);
    unsigned jitter = std::abs(output.maxSamplesBetweenClocks - output.minSamplesBetweenClocks);
    assertLE(jitter, acceptableJitter);
    // SQINFO("in new test, jitter=%d, maxSamples=%d min=%d", jitter, output.maxSamplesBetweenClocks, output.minSamplesBetweenClocks);
}

static void testRunRamp() {
    // values from some old test.
    testRunRamp(400, 0, 1, 10, 1);

    // ramp up from xx to 1
    for (float x = 0; x < 1; x += .00958) {
        // SQINFO("x=%f", x);
        testRunRamp(1400, x, 1, 112, 10);  // jitter 4/112 is ok with me.
    }

    // ramp down from 1 to xx
    for (float x = .999; x > 0; x -= .00958) {
        // SQINFO("xx=%f", x);
        testRunRamp(1400, 1, x, 112, 4);  // jitter 4/112 is ok with me.
    }
}

void testClockShifter6d() {
    testCanCall();
    testDelayRamp();
    testConstantRamp();
    testClockGen();
    testRunZero();
    testRunRamp();
    assertEQ(SqLog::errorCount, 0);
}

#if 0
void testFirst() {
  //  ClockShifter6::llv = 1;
  //  testRunSlowDown(400, .2, .4, 10);

   // testRunSlowDown(15, 0, 1, 5);
  //  testRunRamp(15, 1, 0, 5);
 //   testRunRamp(150, 1, 0, 50);
  //  testRunSlowDown(4000, .2, .4, 5);
  //  testRunSlowDown(4000, .2, .4, 100);

    //testRunRamp();
    logLevel = 1;
    float  x = 0.172440;
    testRunRamp(1400, x, 1, 112, 10);
   // testClockShifter6d();
 
}
#endif