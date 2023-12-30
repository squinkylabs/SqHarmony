#pragma once

struct Port;
class PolyClockShifter {
public:
    void runEverySample(Port* clocOutput, Port* clockInput, Port* shiftAmount);
    void runEveryBlock();
private:
};

inline void PolyClockShifter::runEveryBlock() {

}

inline void PolyClockShifter::runEverySample(Port* clocOutput, Port* clockInput, Port* shiftAmount) {

}