#pragma once

class OneShotSampleTimer {
public:
    /**
     * @brief one more cycle of counting
     *
     * @return true when the timer expires
     */
    bool run();

    void arm(int period);

    /**
     * @brief get the state of the timer
     *
     * @return true after armed, before delay times out
     */
    bool get() const;

private:
    bool _state = false;
    int _counter = 0;
};

inline bool OneShotSampleTimer::run() {
    if (_counter) {
        --_counter;
        if (_counter < 1) {
            _state = false;
        }
    }
    return get();
}

inline void OneShotSampleTimer::arm(int period) {
    _state = true;
    _counter = period;
}

 inline bool OneShotSampleTimer::get() const {
    return _state;
 }