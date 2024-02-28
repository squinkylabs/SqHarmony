
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

    void initClock(unsigned  period, float dutycycle, unsigned duration) {
        assert(dutycycle > 0);
        assert(dutycycle < 100);

        _clockHighSamples = (period * dutycycle) / 100;
        _clockLowSamples = (period * (100 - dutycycle)) / 100;
        assert(period == (_clockLowSamples + _clockHighSamples));
        assert(_samplesRemaining==0 || _samplesRemaining == duration);
        _samplesRemaining = duration;
        _clockPeriod = period;
    }

    void initForDelayRamp(float initial, float final, unsigned duration) {
        assert(initial != final);
        _sampleCounter = 0;
        assert(_samplesRemaining==0 || _samplesRemaining == duration);
        _samplesRemaining = duration;

        _currentDelay = initial;
        _deltaDelayPerSample = (final - initial) / duration;
        assert(_deltaDelayPerSample != 0);
    }

    void initForConstant(float value, unsigned duration) {
        _sampleCounter = 0;
        _samplesRemaining = duration;
        _currentDelay = value;
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

    void _generateNextSample() {
        _sampleCounter++;
        _samplesRemaining--;
        _currentDelay += _deltaDelayPerSample;
        ++_clockAcc;
        if (_clockAcc >= (_clockHighSamples + _clockLowSamples)) {
            _clockAcc = 0;
        }
    }
};

class Output6 {
public:
    int outputClocks = 0;
    int minSamplesBetweenClocks = 100000;
    int maxSamplesBetweenClocks = 0;

    float maxDelay = -100;
    float minDelay = 100;

    void onClock(bool ck, unsigned sample);
};

static void run(SignalSourceInterface* source, Output6* output) {
    unsigned period = source->getPeriod();
    float delay = source->getDelay();
    auto shifter = makeAndPrime(period, delay);

}

static void testCanCall() {
    SignalSource s;
    s.initForDelayRamp(0, 1, 100);
    Output6 results;
    run(&s, &results);
}

static void testDelayRamp() {
    const unsigned dur = 100;
    SignalSource s;
    s.initForDelayRamp(0, 1, 100);
    float x = -.001;
    for (int i=0; i<100; ++i) {
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
    for (int i=0; i<7; ++i) {
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

    for (unsigned  i = 0; i < clocksHigh; ++i) {
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

static void testRunZero() {
    assert(false);
}

void testClockShifter6d() {
    testCanCall();
    testDelayRamp();
    testConstantRamp();
    testClockGen();
    testRunZero();
    assertEQ(SqLog::errorCount, 0);
}

#if 1
void testFirst() {
    ClockShifter6::llv = 1;
    //  This is the case that is bad without the dodgy "fix"
    // testWithLFO(4, 16, 0.136364, 0.400000, 3);

    // testSlowDown(5, 3552, 0.0001407658, 7);
}
#endif