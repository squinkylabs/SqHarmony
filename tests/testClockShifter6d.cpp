
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
};

class SignalSource : public SignalSourceInterface {
public:
    bool getClock() const override {
        return false;
    }
    float getDelay() const override {
        return 0;
    }
    bool isMoreData() const override {
        return _samplesRemaining > 0;
    }
    void next() override {
        assert(isMoreData());
        _generateNextSample();
    }

    void initForDelayRamp(float initial, float final, unsigned duration) {
        assert(initial != final);
        _sampleCounter = 0;
        _samplesRemaining = duration;

        _currentDelay = initial;
        _deltaDelayPerSample = (final - initial) / duration;
        assert(_deltaDelayPerSample != 0);
    }
private:
    double _currentDelay = 0;
    unsigned _sampleCounter = 0;
    int _samplesRemaining = 0;
    double _deltaDelayPerSample = 0;

    void _generateNextSample() {
        _sampleCounter++;
        _samplesRemaining--;
        _currentDelay += _deltaDelayPerSample;
    }
};

class Output6 {
public:
    int outputClocks = 0;
    int minSamplesBetweenClocks = 100000;
    int maxSamplesBetweenClocks = 0;

    float maxDelay = -100;
    float minDelay = 100;
};

static void run(SignalSourceInterface*, Output6* theResults) {
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
    for (int i=0; i<100; ++i) {
        assertEQ(s.isMoreData(), true);
        s.next();
    }
    assertEQ(s.isMoreData(), false);

}

void testClockShifter6d() {
    testCanCall();
    testDelayRamp();
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