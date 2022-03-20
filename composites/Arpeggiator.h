
#include <stdio.h>

#include <cmath>

#include "ArpegPlayer.h"
#include "ArpegRhythmPlayer.h"
#include "GateDelay.h"
#include "NoteBuffer.h"
#include "SeqClock.h"

//#define _GCK        // gate clocked

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
        POLY_PARAM,     // not used yet?
        RESET_MODE_PARAM,
        GATE_DELAY_PARAM,
        GATE_CLOCKED_PARAM,     // if true, gate only changes on clock rising edge.
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
        EOC_OUTPUT,
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
    void onGateChange(int channel, bool gate);

    /**
     * @param clockFired is true when detector decides a clock is rea.
     * @param clockValue is cleaned up clock input
     */
    void onClockChange(bool clockFired, bool clockValue);

    bool lastGate[16]{0};
    bool allGatesLow = true;
   // float sampledPitch[16]{0};
    bool lastClock{false};
    void processParams();

    NoteBuffer noteBuffer{32};
    ArpegPlayer hiddenPlayer{&noteBuffer};
    ArpegRhythmPlayer outerPlayer{&hiddenPlayer};
    SeqClock clock;
    GateDelay gateDelay;
    GateTrigger triggerInputProc;

    const int numModes = {int(modes().size())};
};

template <class TBase>
inline void Arpeggiator<TBase>::init() {
    clock.setResetMode(true);
    // TODO: need to setup clock also.
}

template <class TBase>
inline void Arpeggiator<TBase>::process(const typename TBase::ProcessArgs& args) {
    SQINFO("~process (enter)");
    processParams();
    const int gates = TBase::inputs[GATE_INPUT].channels;
    const int cvs = TBase::inputs[CV_INPUT].channels;
    const bool monoGates = (gates == 1) && (cvs > 1);

    if (monoGates) {
        // for mono gates, just look at gate[0], but send to to all cv channels
        //SQDEBUG("gate delay will process mg input=%f", TBase::inputs[GATE_INPUT].getVoltage(0));
        gateDelay.process(TBase::inputs[GATE_INPUT], gates);
        bool gate = gateDelay.getGate(0);
        if (gate != lastGate[0]) {
            lastGate[0] = gate;
            for (int ch = 0; ch < cvs; ++ch) {
                onGateChange(ch, gate);
            }     
        }
    } else {
        //SQDEBUG("gate delay will process !mg input=%f", TBase::inputs[GATE_INPUT].getVoltage(0));
        gateDelay.process(TBase::inputs[GATE_INPUT], gates);
        for (int ch = 0; ch < gates; ++ch) {
            //bool gate = TBase::inputs[GATE_INPUT].getVoltage(ch);
            bool gate = gateDelay.getGate(ch);
            if (gate != lastGate[ch]) {
                lastGate[ch] = gate;
                onGateChange(ch, gate);
            }
        }
    }

   
    const bool shuffleInputConnected = TBase::inputs[SHUFFLE_TRIGGER_INPUT].isConnected();
    if (shuffleInputConnected) {
        triggerInputProc.go(TBase::inputs[SHUFFLE_TRIGGER_INPUT].getVoltage(0));
        if (triggerInputProc.trigger()) {
            SQINFO("got trigger, sending to player");
            outerPlayer.armReShuffle();
        }
    }

    const float clockVoltageX = TBase::inputs[CLOCK_INPUT].getVoltage(0);
    const float resetVoltage = TBase::inputs[RESET_INPUT].getVoltage(0);
    auto clockResults = clock.updateOnce(clockVoltageX, true, resetVoltage);

    if (clockResults.didReset) {
        SQDEBUG("did reset");
        clockResults.didClock = true;  // let's force one after this, to get the new value?
        outerPlayer.reset();
    }

    const bool processedClock = clock.getClockValue();
    if (clockResults.didClock || processedClock != lastClock) {
        SQDEBUG("didClock=%d , proc=%d last=%d", clockResults.didClock, processedClock, lastClock);
        lastClock = processedClock;

   //     const bool shuffleInputConnected = TBase::inputs[SHUFFLE_TRIGGER_INPUT].isConnected();
      //  assert(false);      // pass real value
        onClockChange(clockResults.didClock, processedClock);
    }
     SQINFO("~process (exit)");
}

template <class TBase>
inline void Arpeggiator<TBase>::onGateChange(int channel, bool gate) {
    const float cv1 = TBase::inputs[CV_INPUT].getVoltage(channel);
    const float cv2 = TBase::inputs[CV2_INPUT].getVoltage(channel);
    SQINFO("Arpeggiator<TBase>::onGateChange will send CV to nb: %f, %f", cv1, cv2);
    if (gate) {
        noteBuffer.push_back(cv1, cv2, channel);
      //  sampledPitch[channel] = pitch;
    } else {
        noteBuffer.removeForChannel(channel);
      //  sampledPitch[channel] = 0;
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
    SQDEBUG("Arpeg::onClockChange, fired = %d value = %d", clockFired, clockValue);
    if (clockFired) {
        const auto cvs = outerPlayer.clock();
        SQINFO("will output player out to CV: %f,%f", cvs.first, cvs.second);
        TBase::outputs[CV_OUTPUT].setVoltage(cvs.first, 0);
        TBase::outputs[CV2_OUTPUT].setVoltage(cvs.second, 0);
    }

    if (hiddenPlayer.empty()) {
        clockValue = false;
        SQDEBUG("AM muting everything, no notes, clockFired=%d, value=%d", clockFired, clockValue);  
    }

    if (allGatesLow) {
       // SQDEBUG("setting clock value low because all low. will force gate low\n");
       // clockValue = false;
       SQDEBUG("would mute everything, but I took that out");
    }
    const float clockVoltage = clockValue ? cGateOutHi : 0.f;

    SQDEBUG("setting gate out to %f", clockVoltage);
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
        hold = bool(std::round(TBase::params[HOLD_PARAM].value));
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
