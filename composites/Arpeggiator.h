
#include <stdio.h>

#include <cmath>

#include "ArpegPlayer.h"
#include "ArpegRhythmPlayer.h"
#include "GateDelay.h"
#include "NoteBuffer.h"
#include "SeqClock.h"

// #define _GCK        // gate clocked

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
        POLY_PARAM,  // not used yet?
        RESET_MODE_PARAM,
        GATE_DELAY_PARAM,
        GATE_CLOCKED_PARAM,  // if true, gate only changes on clock rising edge.
        NUM_PARAMS
    };
    enum InputIds {
        CV_INPUT,
        GATE_INPUT,
        CLOCK_INPUT,
        RESET_INPUT,
        HOLD_INPUT,
        MODE_INPUT,
        CV2_INPUT,
        SHUFFLE_TRIGGER_INPUT,
        NUM_INPUTS
    };

    enum OutputIds {
        CV_OUTPUT,
        GATE_OUTPUT,
        EOC_OUTPUT_unused,
        CV2_OUTPUT,
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
    void onGateChange(int channel, bool gate, int currentPolyphony);

    /**
     * @param clockFired is true when detector decides a clock is rea.
     * @param clockValue is cleaned up clock input
     */
    void onClockChange(bool clockFired, bool clockValue);

    bool lastGate[16]{0};
    bool allGatesLow = true;
    bool lastClock{false};
    void processParams();

    NoteBuffer noteBuffer{32};
    ArpegPlayer hiddenPlayer{&noteBuffer};
    ArpegRhythmPlayer outerPlayer{&hiddenPlayer};
    SeqClock clock;
    GateDelay<5> gateOnlyDelay;
    GateDelay<10> clockOnlyDelay;
    GateTrigger triggerInputProc;

    const int numModes = {int(modes().size())};
    int polyphonyWhenGateWentHigh = 0;
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
    const int currentPolyphony = TBase::inputs[CV_INPUT].channels;
    const bool monoGates = (gates == 1) && (currentPolyphony > 1);

    int highestGateProcessed = 0;
    if (monoGates) {
        highestGateProcessed = currentPolyphony;
        // for mono gates, just look at gate[0], but send to to all cv channels
        // SQDEBUG("gate delay will process mg input=%f", TBase::inputs[GATE_INPUT].getVoltage(0));
        gateOnlyDelay.process(TBase::inputs[GATE_INPUT], gates);
        const bool gate = gateOnlyDelay.getGate(0);
        if (gate != lastGate[0]) {
            lastGate[0] = gate;
            const int numChannelsToProcess = gate ? currentPolyphony : std::max(currentPolyphony, this->polyphonyWhenGateWentHigh);
            for (int ch = 0; ch < numChannelsToProcess; ++ch) {
                onGateChange(ch, gate, currentPolyphony);
            }
        }
    } else {
        highestGateProcessed = gates;
        gateOnlyDelay.process(TBase::inputs[GATE_INPUT], gates);
        for (int ch = 0; ch < gates; ++ch) {
            const bool gate = gateOnlyDelay.getGate(ch);
            if (gate != lastGate[ch]) {
                lastGate[ch] = gate;
                onGateChange(ch, gate, currentPolyphony);
            }
        }
    }

    // optimization: could do this only if connected changes
    // Not sure what this is any more! Can find out from unit tests.
    for (int ch = highestGateProcessed; ch < 16; ++ch) {
        if (lastGate[ch]) {
            lastGate[ch] = false;
            onGateChange(ch, false, currentPolyphony);
        }
    }

    const bool shuffleInputConnected = TBase::inputs[SHUFFLE_TRIGGER_INPUT].isConnected();
    if (shuffleInputConnected) {
        triggerInputProc.go(TBase::inputs[SHUFFLE_TRIGGER_INPUT].getVoltage(0));
        if (triggerInputProc.trigger()) {
            outerPlayer.armReShuffle();
        }
    } else {
        // if no CV, we want it armed "all the time"
        outerPlayer.armReShuffle();
    }

    clockOnlyDelay.process(TBase::inputs[CLOCK_INPUT], 1);
    const float clockVoltageX = clockOnlyDelay.getGate(0) ? 10.f : 0.f;
    const float resetVoltage = TBase::inputs[RESET_INPUT].getVoltage(0);
    auto clockResults = clock.updateOnce(clockVoltageX, true, resetVoltage);

    if (clockResults.didReset) {
        clockResults.didClock = true;  // Let's force one after this, to get the new value?
        outerPlayer.reset();
    }

    const bool processedClock = clock.getClockValue();
    if (clockResults.didClock || processedClock != lastClock) {
        lastClock = processedClock;
        onClockChange(clockResults.didClock, processedClock);
    }
}

template <class TBase>
inline void Arpeggiator<TBase>::onGateChange(int channel, bool gate, int currentPolyphony) {
    if (gate) {
        const float cv1 = TBase::inputs[CV_INPUT].getVoltage(channel);
        const float cv2 = TBase::inputs[CV2_INPUT].getVoltage(channel);
        noteBuffer.push_back(cv1, cv2, channel);
        this->polyphonyWhenGateWentHigh = currentPolyphony;
    } else {
        noteBuffer.removeForChannel(channel);
    }

    // TODO: get rid of this all gates low stuff?
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
        const auto cvs = outerPlayer.clock2();
        if (std::get<0>(cvs)) {
            // SQINFO("will output player out to CV: cv1=%f, cv2=%f", std::get<1>(cvs), std::get<2>(cvs));
            TBase::outputs[CV_OUTPUT].setVoltage(std::get<1>(cvs), 0);
            TBase::outputs[CV2_OUTPUT].setVoltage(std::get<2>(cvs), 0);
        } else {
            // SQINFO("ignoring clock on empty");
        }
    }

    if (hiddenPlayer.empty()) {
        clockValue = false;
        // SQDEBUG("AM muting everything, no notes, clockFired=%d, value=%d", clockFired, clockValue);
    }

    if (allGatesLow) {
        // SQDEBUG("setting clock value low because all low. will force gate low\n");
        // clockValue = false;
        // SQDEBUG("would mute everything, but I took that out");
    }
    const float clockVoltage = clockValue ? cGateOutHi : cGateOutLow;

    // SQDEBUG("setting gate out to %f", clockVoltage);
    TBase::outputs[GATE_OUTPUT].setVoltage(clockVoltage, 0);
}

template <class TBase>
inline void Arpeggiator<TBase>::processParams() {
    const int length = int(std::round(TBase::params[LENGTH_PARAM].value));
    const int beats = int(std::round(TBase::params[BEATS_PARAM].value));
    const bool resetMode = bool(std::round(TBase::params[RESET_MODE_PARAM].value));
    const bool delayEnabled = bool(std::round(TBase::params[GATE_DELAY_PARAM].value));

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
        hold = bool(std::round(TBase::params[HOLD_PARAM].value));
    }

    outerPlayer.setLength(beats);
    hiddenPlayer.setMode(ArpegPlayer::Mode(mode));
    noteBuffer.setCapacity(length);
    noteBuffer.setHold(hold);
    clock.setResetMode(resetMode);
    gateOnlyDelay.enableDelay(delayEnabled);
    clockOnlyDelay.enableDelay(delayEnabled);
}
