#pragma once

class ShiftCalc {
public:
    /**
     * @brief clocks generator once, returns current output.
     * 
     * @return float 
     */
    float go();
    void setup(int samplesInMasterPeriod);
private:
};

inline float ShiftCalc::go() {
    return 0;
}

inline void ShiftCalc::setup(int samplesInMasterPeriod) {
} 