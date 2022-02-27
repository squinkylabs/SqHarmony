
#include <stdio.h>

#include <cmath>

#include "ArpegPlayer.h"
#include "ArpegRhythmPlayer.h"
#include "GateDelay.h"
#include "NoteBuffer.h"
#include "SeqClock.h"

namespace rack {
namespace engine {
struct Module;
}
}  // namespace rack

using Module = ::rack::engine::Module;

template <class TBase>
class Arpeggiator : public TBase {
public:
    Arpeggiator(Module* module) : TBase(module) {
        init();
    }
    Arpeggiator() : TBase() {
        init();
    }

    enum ParamIds {
        MODE_PARAM,
        LENGTH_PARAM,  // how many notes we hold in the buffer
        BEATS_PARAM,   // how we play the pattern back
        HOLD_PARAM,
        SCHEMA_PARAM,
        POLY_PARAM,
        RESET_MODE_PARAM,
        GATE_DELAY_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        CV_INPUT,
        GATE_INPUT,
        CLOCK_INPUT,
        RESET_INPUT,
        HOLD_INPUT,
        MODE_INPUT,
        NUM_INPUTS
    };

    enum OutputIds {
        CV_OUTPUT,
        GATE_OUTPUT,
        EOC_OUTPUT,
        NUM_OUTPUTS
    };

    enum LightIds {
        RESET_LIGHT,
        EOC_LIGHT,
        NUM_LIGHTS
    };

    void process(const typename TBase::ProcessArgs& args) override;

    static std::vector<std::string> modes() {
        return ArpegPlayer::modes();
    }

    static std::vector<std::string> shortModes() {
        return ArpegPlayer::shortModes();
    }

private:
    void init();
    void onGateChange(int channel, bool gate);

    /**
     *
     * @param clockFired is true when detector decides a clock is rea.
     * @param clockValue is cleaned up clock input
     */
    void onClockChange(bool clockFired, bool clockValue);

    bool lastGate[16]{0};
    bool allGatesLow = true;
    float sampledPitch[16]{0};
    bool lastClock{false};
    void processParams();

    NoteBuffer noteBuffer{32};
    ArpegPlayer hiddenPlayer{&noteBuffer};
    ArpegRhythmPlayer outerPlayer{&hiddenPlayer};
    SeqClock clock;
    GateDelay gateDelay;

    const int numModes = {int(modes().size())};
};

template <class TBase>
inline void Arpeggiator<TBase>::init() {
    clock.setResetMode(true);
    // TODO: need to setup clock also.
}

template <class TBase>
inline void Arpeggiator<TBase>::process(const typename TBase::ProcessArgs& args) {
    processParams();
    const int gates = TBase::inputs[GATE_INPUT].channels;
    const int cvs = TBase::inputs[CV_INPUT].channels;
    const bool monoGates = (gates == 1) && (cvs > 1);

    if (monoGates) {
        // for mono gates, just look at gate[0], but send to to all cv channels
        gateDelay.process(TBase::inputs[GATE_INPUT], gates);
        bool gate = gateDelay.getGate(0);
        if (gate != lastGate[0]) {
            lastGate[0] = gate;
            for (int ch = 0; ch < cvs; ++ch) {
                onGateChange(ch, gate);
            }     
        }
    } else
        gateDelay.process(TBase::inputs[GATE_INPUT], gates);
        for (int ch = 0; ch < gates; ++ch) {
            //bool gate = TBase::inputs[GATE_INPUT].getVoltage(ch);
            bool gate = gateDelay.getGate(ch);
            if (gate != lastGate[ch]) {
                lastGate[ch] = gate;
                onGateChange(ch, gate);
            }
        }

    const float clockVoltageX = TBase::inputs[CLOCK_INPUT].getVoltage(0);
    const float resetVoltage = TBase::inputs[RESET_INPUT].getVoltage(0);
    auto clockResults = clock.updateOnce(clockVoltageX, true, resetVoltage);

    if (clockResults.didReset) {
        clockResults.didClock = true;  // let's force one after this, to get the new value?
        outerPlayer.reset();
    }

    const bool processedClock = clock.getClockValue();
    if (clockResults.didClock || processedClock != lastClock) {
        lastClock = processedClock;
        onClockChange(clockResults.didClock, processedClock);
    }
}

template <class TBase>
inline void Arpeggiator<TBase>::onGateChange(int channel, bool gate) {
    const float pitch = TBase::inputs[CV_INPUT].getVoltage(channel);
    if (gate) {
        noteBuffer.push_back(pitch, channel);
        sampledPitch[channel] = pitch;
    } else {
        noteBuffer.removeForChannel(channel);
        sampledPitch[channel] = 0;
    }

    allGatesLow = true;
    for (int i = 0; i < 16; ++i) {
        if (lastGate[i]) {
            allGatesLow = false;
        }
    }
}

template <class TBase>
inline void Arpeggiator<TBase>::onClockChange(bool clockFired, bool clockValue) {
    if (clockFired) {
        const float pitch = outerPlayer.clock();
        TBase::outputs[CV_OUTPUT].setVoltage(pitch, 0);
    }

    if (allGatesLow) {
        clockValue = false;
    }
    const float clockVoltage = clockValue ? cGateOutHi : 0.f;

    TBase::outputs[GATE_OUTPUT].setVoltage(clockVoltage, 0);
}

template <class TBase>
inline void Arpeggiator<TBase>::processParams() {
    const int length = int(std::round(TBase::params[LENGTH_PARAM].value));
    const int beats = int(std::round(TBase::params[BEATS_PARAM].value));
    //   const bool hold = bool(std::round(TBase::params[HOLD_PARAM].value));

    const bool resetMode = bool(std::round(TBase::params[RESET_MODE_PARAM].value));
    const bool gateDelayEnabled = bool(std::round(TBase::params[GATE_DELAY_PARAM].value));

    int mode = 0;
    if (TBase::inputs[MODE_INPUT].isConnected()) {
        const float v = TBase::inputs[MODE_INPUT].getVoltage(0);
        float modeVoltage = v * 12;
        modeVoltage = std::max(modeVoltage, 0.f);
        modeVoltage = std::min(modeVoltage, float(numModes - 1));
        TBase::params[MODE_PARAM].value = modeVoltage;
        mode = int(std::round(modeVoltage));
    } else {
        mode = int(std::round(TBase::params[MODE_PARAM].value));
    }

    bool hold = false;
    if (TBase::inputs[HOLD_INPUT].isConnected()) {
        const float v = TBase::inputs[HOLD_INPUT].getVoltage(0);
        float holdVoltage = v > 1.5 ? 1.f : 0.f;
        TBase::params[HOLD_PARAM].value = holdVoltage;
        hold = bool(holdVoltage);
    } else {
        mode = int(std::round(TBase::params[MODE_PARAM].value));
    }

    outerPlayer.setLength(beats);
    hiddenPlayer.setMode(ArpegPlayer::Mode(mode));
    if (length) {
        noteBuffer.setCapacity(length);
    }
    noteBuffer.setHold(hold);
    clock.setResetMode(resetMode);
    gateDelay.enableDelay(gateDelayEnabled);
}
