#pragma once

class OneShotSampleTimer {
public:
    /**
     * @brief one more cycle of counting
     *
     * @return true if the timer is still running, false if is had expired
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
    if (_counter > -1) {
        --_counter;
        if (_counter < 1) {
            _isRunning = false;
        }
    }
    return isRunning();
}

inline void OneShotSampleTimer::arm(int period) {
    _isRunning = true;
    _counter = period;
}

inline bool OneShotSampleTimer::isRunning() const {
    return _isRunning;
}