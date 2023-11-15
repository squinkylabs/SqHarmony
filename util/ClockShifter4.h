#pragma once

class ClockShifter4 {
public:
    void setShift(float);
    bool run(bool input);

private:
};

inline void ClockShifter4::setShift(float) {
}

inline  bool ClockShifter4::run(bool) {
    return false;
}