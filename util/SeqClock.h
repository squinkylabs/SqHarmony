#pragma once

#include <string>
#include <vector>

#include "GateTrigger.h"
#include "OneShot.h"
#include "SqLog.h"

class SeqClock {
public:
    SeqClock();

    /**
     * Data returned when clock is advanced
     */
    class ClockResults {
    public:
        bool didClock = false;
        bool didReset = false;
    };

    /**
     * param samplesElapsed is how many sample clocks have elapsed since last call. will usually be 1
     * param externalClock - is the clock CV, 0..10. will look for rising edges
     * param runStop is the run/stop flag. External logic must toggle it. It is level
     *      sensitive, so clock stays stopped as long as it is low
     * param reset is edge sensitive. Only false -> true transition will trigger a reset.
     *
     * return - total elapsed metric time
     *
     * note that only one of the two passed params will be used, depending
     * on internal/external model.
     */
    ClockResults updateMulti(int samplesElapsed, float externalClock, bool runStop, float reset);
    ClockResults updateOnce(float externalClock, bool runStop, float reset);

    // sample time is seconds for one sample
    void setup(float sampleTime);

    /**
     * @param mode - if true, use nord trigger, if false use Impromptu;
     */
    void setResetMode(bool mode) {
        nordModeReset = mode;
    }
    //    void reset(bool internalClock);

    bool getClockValue() {
        return clockProcessor.gate();
    }

private:
    float sampleTime = 1.f / 44100.f;
    // double metricTimePerClock = 1;
    bool nordModeReset = false;

    GateTrigger clockProcessor;
    GateTrigger resetProcessor;
    OneShot resetLockout;
    bool nordResetRequested = false;
};

// We don't want reset logic on clock, as clock high should not be ignored.
// Probably don't want on reset either.
inline SeqClock::SeqClock() : clockProcessor(false),
                              resetProcessor(false) {
    resetLockout.setDelayMs(1);
    resetLockout.setSampleTime(1.f / 44100.f);
}

inline SeqClock::ClockResults SeqClock::updateMulti(int samplesElapsed, float externalClock, bool runStop, float reset) {
    SeqClock::ClockResults results;
    for (int i = 0; i < samplesElapsed; ++i) {
        auto x = updateOnce(externalClock, runStop, reset);
        results.didClock |= x.didClock;
        results.didReset |= x.didReset;
    }
    return results;
}

inline SeqClock::ClockResults SeqClock::updateOnce(float externalClock, bool runStop, float reset) {
    ClockResults results;

    resetProcessor.go(reset);
    const bool resetEdge = resetProcessor.trigger();
    if (resetEdge) {
        // reset line just went active
        if (nordModeReset) {
            // queue up a reset for next clock
            nordResetRequested = true;
        } else {
            results.didReset = true;
            resetLockout.set();
            // reset the clock so that high clock can gen another clock
            clockProcessor.reset();
        }
    }

    resetLockout.step();  // one more sample for the lockout guy

    if (!runStop) {
        return results;
    }

    // ignore external clock during lockout
    // assert(!nordModeReset);
    if (nordModeReset) {
        clockProcessor.go(externalClock);
        if (clockProcessor.trigger()) {
            if (nordResetRequested) {
                results.didReset = true;
                nordResetRequested = false;
            }

            // either way we generate a clock
            results.didClock = true;
            SQDEBUG("clock fired in nord mode");
        }
    } else {
        if (resetLockout.hasFired()) {
            // external clock
            clockProcessor.go(externalClock);
            if (clockProcessor.trigger()) {
                results.didClock = true;
                SQDEBUG("clock fired in non-nord");
            }
        } else {
            clockProcessor.ignoreInput(externalClock);
        }
    }

    return results;
}

inline void SeqClock::setup(float sampleTime) {
    sampleTime = sampleTime;
    resetLockout.setSampleTime(sampleTime);
}
