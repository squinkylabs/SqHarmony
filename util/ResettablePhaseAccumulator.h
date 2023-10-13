
class ResettablePhaseAccumulator {
public:
    void reset(double phase, double freq);
    void tick();
    double getPhase() const {
        return _phase;
    }
private:
    double _phase=0;
    double _delta = 0;
};

inline void ResettablePhaseAccumulator::reset(double phase, double freq) {
    assert(phase >= 0);
    assert(phase < 1);
    assert(freq >= 0);
    assert(freq < .5);

    _phase = phase;
    _delta = freq;
}

inline void ResettablePhaseAccumulator::tick() {
    _phase += _delta;
}