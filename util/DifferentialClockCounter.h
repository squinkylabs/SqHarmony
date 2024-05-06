
#pragma once

class DifferentialClockCounter {
public:
    void process(float v1, float v2);

    int getDiff() const;
private:
};

inline void DifferentialClockCounter::process(float, float) {

}

inline int DifferentialClockCounter::getDiff() const {
    return 0;
}