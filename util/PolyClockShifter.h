#pragma once

#include <algorithm>

#include "ClockShifter4.h"
#include "GateTrigger.h"
#include "ShiftCalc.h"

struct Port;
class PolyClockShifter {
public:
    class PortInfo {
    public:
        ::Port* clockOutput;
        ::Port* clockInput;
        ::Port* ribsTrigger;
        ::Port* shiftAmount;
    };
    void runEveryBlock(const PortInfo& info);
    void runEverySample(const PortInfo& info);

private:
    bool haveRunBlock = false;
    GateTrigger _inputClockProc[16];
    ClockShifter4 _clockShifter[16];
    ShiftCalc _shiftCalculator[16];
};

inline void PolyClockShifter::runEverySample(const PortInfo& info) {
    if (haveRunBlock) {
        _shiftCalculator[0].go();
        const float rawClockIn = info.clockInput->getVoltage(0);
        _inputClockProc[0].go(rawClockIn);
        // const bool clockIn = _inputClockProc.go(rawClockIn);
        const bool rawClockOut = _clockShifter[0].process(_inputClockProc[0].trigger(), _inputClockProc[0].gate());
        const float clockOut = rawClockOut ? cGateOutHi : cGateOutLow;
        info.clockOutput->setVoltage(clockOut);
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