#pragma once

#include <algorithm>

struct Port;
class PolyClockShifter {
public:
    class PortInfo {
    public:
        ::Port* clockOutput;
        ::Port * clockInput;
        ::Port * ribsTrigger;
        ::Port* shiftAmount;
    };
    void runEveryBlock(const PortInfo& info);
    void runEverySample(const PortInfo& info);

private:
    bool haveRunBlock = false;
};

inline void PolyClockShifter::runEverySample(const PortInfo& info) {
    if (haveRunBlock) {
    }
}

inline void PolyClockShifter::runEveryBlock(const PortInfo& info) {
    haveRunBlock = true;
    if (!info.clockOutput->isConnected()) {
        return;
    }
    int numOutputs = 1; 

    numOutputs = std::max(numOutputs, int(info.clockInput->channels));
    numOutputs = std::max(numOutputs, int(info.ribsTrigger->channels));
    numOutputs = std::max(numOutputs, int(info.shiftAmount->channels));
    info.clockOutput->setChannels(numOutputs);
}