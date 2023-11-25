#pragma once

class OneShotSampleTimer {
public:
    /**
     * @brief one more cycle of counting
     *
     * @return true if the timer just stopped running, false otherwise
     */
    bool run();

    void arm(int period);

    /**
     * @brief get the state of the timer
     *
     * @return true after armed, before delay times out
     */
    bool isRunning() const;

private:
    bool _isRunning = false;
    int _counter = 0;
};

inline bool OneShotSampleTimer::run() {
    bool justFired = false;
    if (_isRunning) {
        --_counter;
        if (_counter < 1) {
            _isRunning = false;
            justFired = true;
        }
    }
    return justFired;
}

inline void OneShotSampleTimer::arm(int period) {
    _isRunning = true;
    _counter = period;
}

inline bool OneShotSampleTimer::isRunning() const {
    return _isRunning;
}