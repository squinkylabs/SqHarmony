

#pragma once
#include <utility>
class ResettablePhaseAccumulator {
public:
    void reset(double phase, double freq);
    /**
     * @brief clocks the accumulator.
     *
     * @return true when phase rolls over (fires)
     */
    bool tick();

    /**
     * @brief Get the Phase And Delta object, just for debugging
     *
     * @return std::pair<double, double> first is phase, second is delta
     */
    std::pair<double, double> getPhaseAndDelta();

private:
    double _phase = 0;
    double _delta = 0;
};

inline void ResettablePhaseAccumulator::reset(double phase, double freq) {
    assert(phase >= 0);
    assert(phase < 1);
    assert(freq >= 0);
    assert(freq < .5);

    _phase = phase;
    _delta = freq;
    SQINFO("on reset, acc phase=%f, freq=%f", _phase, _delta);
}

inline bool ResettablePhaseAccumulator::tick() {
    bool didRoll = false;
    _phase += _delta;
    while (_phase >= 1) {
        _phase -= 1;
        didRoll = true;
    }
    SQINFO("acc tick phase is %f rollover=%d", _phase, didRoll);
    return didRoll;
}

inline std::pair<double, double> ResettablePhaseAccumulator::getPhaseAndDelta() {
    return std::make_pair(_phase, _delta);
}