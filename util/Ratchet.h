#pragma once

class Ratchet {
public:
    void setCount(float count);
    bool run(bool ckIn);
    void trigger();
private:

};

inline void Ratchet::setCount(float count) {
}

inline bool Ratchet::run(bool ckIn) {
    return false;
}

inline void Ratchet::trigger() {
}