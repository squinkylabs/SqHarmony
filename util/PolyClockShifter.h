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
    bool _haveRunBlock = false;
    int _numInputClocks = 0;
    int _numOutputClocks = 0;
    int _numRibsGenerators = 0;

    // initialize these guys to turn off the suppression of the first clo.c
    GateTrigger _inputClockProc[16] = {false, false, false, false,
                                       false, false, false, false,
                                       false, false, false, false,
                                       false, false, false, false};
    ClockShifter4 _clockShifter[16];
    ShiftCalc _shiftCalculator[16];
};

inline void PolyClockShifter::runEverySample(const PortInfo& info) {
    if (!_haveRunBlock) {
        SQWARN("running sample before block");
    }

    for (int i = 0; i < _numInputClocks; ++i) {
        const float rawClockIn = info.clockInput->getVoltage(i);
        _inputClockProc[i].go(rawClockIn);
    }

 //   assert(_numOutputClocks == 1 || _numOutputClocks == 0);
    assert(_numRibsGenerators == 1 || _numRibsGenerators == 0);

    _shiftCalculator[0].go();  // TODO: should be done for all of the active ones (ribs channels);

    if (_numOutputClocks > 0) {
        for (int i = 0; i < _numOutputClocks; ++i) {
          //  assert(_numInputClocks > 1);    
         //   if (_numInputClocks)
            const bool rawClockOut = _clockShifter[i].process(_inputClockProc[i].trigger(), _inputClockProc[i].gate());
            const float clockOut = rawClockOut ? cGateOutHi : cGateOutLow;
            info.clockOutput->setVoltage(clockOut, i);
        }
    }
}

inline void PolyClockShifter::runEveryBlock(const PortInfo& info) {
    _haveRunBlock = true;

    const int channels = info.clockOutput->channels;
    const bool conn = info.clockOutput->isConnected();
    SQINFO("conn = %d", conn);
    if (!conn) {
        SQINFO("doing nothing, clock out not connected");
        return;
    }
    int numOutputs = 1;
    _numInputClocks = info.clockInput->channels;
    _numRibsGenerators = info.ribsTrigger->channels;

    numOutputs = std::max(numOutputs, _numInputClocks);
    numOutputs = std::max(numOutputs, _numRibsGenerators);
    numOutputs = std::max(numOutputs, int(info.shiftAmount->channels));

    _numOutputClocks = numOutputs;
    info.clockOutput->setChannels(numOutputs);
}