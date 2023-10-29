#pragma once

class Ratchet {
public:
    void setCount(int count);
    /**
     * @brief when it's time to ratchet, return the count. Otherwise returns 1
     * 
     * @param ckIn 
     * @return int 
     */
    int run(bool ckIn);
    void trigger();
    //
    void onOutputClock();
    bool isStillRatcheting() const;
private:
    // What the caller passed to setCount.
    int _count = 1;
    bool _isTriggered = false;

    // Each time a ratchet is triggered, this counts down.
    int _countDown = 0;
};

inline void Ratchet::setCount(int count) {
    _count = count;
}

inline int Ratchet::run(bool ckIn) {
    if (!_isTriggered) {
        return 1;
    }
    // TODO: do we support factional or not??
    int ret = int(_count);
  //  _count = 1;
    _countDown = _count;
    return ret;
}

inline void Ratchet::trigger() {
    _isTriggered = true;
}

inline void Ratchet::onOutputClock() {
}

inline bool Ratchet::isStillRatcheting() const {
    return _countDown > 0;
}