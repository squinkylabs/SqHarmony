#pragma once

#include "AtomicRingBuffer.h"
#include "Chord4.h"
#include "Chord4Manager.h"
#include "Divider.h"
#include "FloatNote.h"
#include "GateDelay.h"
#include "GateTrigger.h"
#include "HarmonyChords.h"
#include "KeysigOld.h"
#include "NoteConvert.h"
#include "Options.h"
#include "Scale.h"
#include "ScaleQuantizer.h"
#include "SqLog.h"

namespace rack {
namespace engine {
struct Module;
}
}  // namespace rack
using Module = ::rack::engine::Module;

template <class TBase>
class Harmony : public TBase {
public:
    Harmony(Module* module) : TBase(module) {
        init();
    }
    Harmony() : TBase() {
        init();
    }

    enum ParamIds {
        SCORE_COLOR_PARAM,  // 0 is white notes, 1 is black notes
        SCORE_GLOW_PARAM,
        SCHEMA_PARAM,
        KEY_PARAM,
        MODE_PARAM,
        INVERSION_PREFERENCE_PARAM,
        CENTER_PREFERENCE_PARAM,
        NNIC_PREFERENCE_PARAM,
        RETRIGGER_CV_AND_NOTE_PARAM,
        HISTORY_SIZE_PARAM,
        TRANSPOSE_STEPS_PARAM,
        TRIGGER_DELAY_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        CV_INPUT,
        TRIGGER_INPUT,
        NUM_INPUTS
    };

    enum OutputIds {
        xQUANTIZER_OUTPUT,
        BASS_OUTPUT,
        TENOR_OUTPUT,
        ALTO_OUTPUT,
        SOPRANO_OUTPUT,
        NUM_OUTPUTS
    };

    enum LightIds {
        NUM_LIGHTS
    };

    void process(const typename TBase::ProcessArgs& args) override;

    class Chord {
    public:
        MidiNote pitch[4];  // the four patches
        int root = 0;       // 1..8
        int inversion = 0;  // 0 = root, 1= first 2 = second
    };

    // called from UI thread
    bool isChordAvailable() const {
        return !chordsOut.empty();
    }

    // called from the UI thread
    Chord getChord() {
        return chordsOut.pop();
    }

    ConstScalePtr getScale() const {
        return chordOptions->keysig->getUnderlyingScale();
    }

    int getOutputChannels(int voice) const {
        int channels = 0;
        switch (voice) {
            case 0:
                channels = TBase::outputs[BASS_OUTPUT].getChannels();
                break;
            case 1:
                channels = TBase::outputs[TENOR_OUTPUT].getChannels();
                break;
            case 2:
                channels = TBase::outputs[ALTO_OUTPUT].getChannels();
                break;
            case 3:
                channels = TBase::outputs[SOPRANO_OUTPUT].getChannels();
                break;
        }
        return channels;
    }

    static std::vector<std::string> getHistoryLabels() {
        return {"off", "4", "8", "13"};
    }

    int _size() const {
        return chordManager->_size();
    }

private:
    void init();
    void outputPitches(const Chord4*);
    void stepn();
    void updateEverything();
    void lookForKeysigChange();

    /**
     * input quantization
     *
     */
    ScaleQuantizer::OptionsPtr quantizerOptions;
    ScaleQuantizerPtr inputQuantizer;

    AtomicRingBuffer<Chord, 12> chordsOut;

    Divider divn;

    int voiceToOutput[4] = {BASS_OUTPUT, TENOR_OUTPUT, ALTO_OUTPUT, SOPRANO_OUTPUT};
    int voiceToChannel[4] = {0};
    /**
     * chord finder
     *
     */
    const Chord4* chordA = nullptr;
    const Chord4* chordB = nullptr;
    OptionsPtr chordOptions;
    Chord4ManagerPtr chordManager;
    HarmonyChords::ChordHistory chordHistory;
    GateTrigger triggerInputProc;
    GateDelay<5> gateDelay;
    float lastQuantizedPitch = -100;
    int count = 0;
    bool mustUpdate = false;
};

template <class TBase>
inline void Harmony<TBase>::init() {
    // Chord options get cmag
    auto keysig = std::make_shared<KeysigOld>(Roots::C);
    auto style = std::make_shared<Style>();
    chordOptions = std::make_shared<Options>(keysig, style);

    // Input Q get CMaj
    quantizerOptions = std::make_shared<ScaleQuantizer::Options>();
    quantizerOptions->scale = std::make_shared<Scale>();
    quantizerOptions->scale->set(MidiNote::C, Scale::Scales::Major);
    inputQuantizer = std::make_shared<ScaleQuantizer>(quantizerOptions);

    chordManager = std::make_shared<Chord4Manager>(*chordOptions);
    assert(chordManager->isValid());


    divn.setup(32, [this]() {
        this->stepn();
    });
}

template <class TBase>
inline void Harmony<TBase>::stepn() {
    assert(TENOR_OUTPUT == (BASS_OUTPUT + 1));
    assert(ALTO_OUTPUT == (TENOR_OUTPUT + 1));
    assert(SOPRANO_OUTPUT == (ALTO_OUTPUT + 1));

    // figure out the voice to jack assignments
    int nextVoiceToAssign = 0;
    int debt = 0;
    for (int portIndex = 0; portIndex < 4; ++portIndex) {
        OutputIds x = OutputIds(portIndex + BASS_OUTPUT);
        const bool connected = Harmony<TBase>::outputs[x].isConnected();
        if (connected) {
            // SQINFO("assign output %d with %d voices", portIndex, 1 + debt);
            Harmony<TBase>::outputs[x].setChannels(1 + debt);

            for (int i = 0; i <= debt; ++i) {
                voiceToOutput[nextVoiceToAssign] = x;
                voiceToChannel[nextVoiceToAssign] = i;
                ++nextVoiceToAssign;
            }
            debt = 0;
        } else {
            debt++;
        }
    }

    const int rawHistorySize = int(std::round(Harmony<TBase>::params[HISTORY_SIZE_PARAM].value));
    int historySize = 1;
    switch (rawHistorySize) {
        case 0:
            historySize = 1;
            break;
        case 1:
            historySize = 4;
            break;
        case 2:
            historySize = 8;
            break;
        case 3:
            historySize = 13;
            break;
        default:
            assert(false);
    }

    chordHistory.setSize(historySize);

    const bool gateDelayEnabled = bool(std::round(Harmony<TBase>::params[TRIGGER_DELAY_PARAM].value));
    gateDelay.enableDelay(gateDelayEnabled);

    bool noNotesInCommon = Harmony<TBase>::params[NNIC_PREFERENCE_PARAM].value > .5;
    auto style = chordOptions->style;
    style->setNoNotesInCommon(noNotesInCommon);

    const Style::Ranges range = Style::Ranges(int(std::round(Harmony<TBase>::params[CENTER_PREFERENCE_PARAM].value)));
    if (style->getRangesPreference() != range) {
        style->setRangesPreference(range);
        mustUpdate = true;
    }

    const Style::InversionPreference ip = Style::InversionPreference(int(std::round(Harmony<TBase>::params[INVERSION_PREFERENCE_PARAM].value)));
    style->setInversionPreference(ip);

    lookForKeysigChange();
}

template <class TBase>
inline void Harmony<TBase>::lookForKeysigChange() {
    const int basePitch = int(std::round((Harmony<TBase>::params[KEY_PARAM].value)));
    const auto mode = Scale::Scales(int(std::round(Harmony<TBase>::params[MODE_PARAM].value)));
    // const auto newSetting = std::make_pair(basePitch, mode);

    const auto current = chordOptions->keysig->get();
    const int currentPitch = current.first.get();
    if ((current.second != mode) || (currentPitch != basePitch)) {
        mustUpdate = true;
        chordOptions->keysig->set(basePitch, mode);
        quantizerOptions->scale->set(basePitch, mode);
    }
}

template <class TBase>
inline void Harmony<TBase>::outputPitches(const Chord4* chord) {
    const HarmonyNote* harmonyNotes = chord->fetchNotes();

    // Chord c is just used for passing to UI for score drawing.
    // Does not affect outputs
    Chord c;
    c.root = chord->fetchRoot();
    c.inversion = int(chord->inversion(*chordOptions));

    // SQINFO("output pitches %s (root=%d) root+4=%d", chord->toStringShort().c_str(), c.root, c.root+4);

    for (int i = 0; i < 4; ++i) {
        MidiNote mn(12 + harmonyNotes[i]);  // harmony note and midi note are about the same;
        FloatNote fn;
        NoteConvert::m2f(fn, mn);
        const int outputPort = voiceToOutput[i];
        const int outputChannel = voiceToChannel[i];
        Harmony<TBase>::outputs[outputPort].setVoltage(fn.get(), outputChannel);
        // SQINFO("set output[%d] to %f from base pitch %f", i, fn.get(), fn.get());
        c.pitch[i] = mn.get();
    }

    if (!chordsOut.full()) {
        chordsOut.push(c);
    } else {
       // SQWARN("in outputPitches, no room for output\n");
    }
}

template <class TBase>
inline void Harmony<TBase>::updateEverything() {
    chordManager = std::make_shared<Chord4Manager>(*chordOptions);
    mustUpdate = false;
    assert(chordManager->isValid());
    chordA = nullptr;
    chordB = nullptr;
}

template <class TBase>
inline void Harmony<TBase>::process(const typename TBase::ProcessArgs& args) {
    assert(chordManager->isValid());
    divn.step();
    if (mustUpdate) {
        updateEverything();
    }

    const bool triggerConnected = Harmony<TBase>::inputs[TRIGGER_INPUT].isConnected();
    bool t = false;
    if (triggerConnected) {

#if 0
       triggerInputProc.go(Harmony<TBase>::inputs[TRIGGER_INPUT].getVoltage(0));
        t = triggerInputProc.trigger();
#else

        gateDelay.process(Harmony<TBase>::inputs[TRIGGER_INPUT], 1);
        bool gate = gateDelay.getGate(0);
        triggerInputProc.go(gate ? 10.f : 0.f );
        t = triggerInputProc.trigger();
#endif
    }

    const float input = Harmony<TBase>::inputs[CV_INPUT].getVoltage(0);
    assert(chordManager);
    MidiNote quantizedInput = inputQuantizer->run(input);
  

    // now we could do xpose here if we convert to srn, then add, then convert back
    const int xposeSteps = int(std::round(Harmony<TBase>::params[TRANSPOSE_STEPS_PARAM].value));
    if (xposeSteps) {
        const int x = quantizedInput.get();
        ScaleNote scaleNote;
        NoteConvert::m2s(scaleNote, *quantizerOptions->scale, quantizedInput);
        scaleNote.transposeDegree(xposeSteps);
        NoteConvert::s2m(quantizedInput, *quantizerOptions->scale, scaleNote);
        if (t) {
            SQINFO("xpose %d steps. midi %d, %d xpose-midi=%d", xposeSteps, x, quantizedInput.get(), quantizedInput.get() - x );
        }
    }

    FloatNote quantizedFloatNote;
    NoteConvert::m2f(quantizedFloatNote, quantizedInput);

    // we don't need quantized note here, could use midi note
    const bool pitchChanged = (quantizedFloatNote.get() != lastQuantizedPitch);
    const bool triggerOnBoth = Harmony<TBase>::params[RETRIGGER_CV_AND_NOTE_PARAM].value > .5;
    if (!triggerConnected || triggerOnBoth) {
        t |= pitchChanged;
    }

    // generate a new chord any time the quantizer outputs a new pitch
    if (t) {
        ScaleNote scaleNote;
        NoteConvert::m2s(scaleNote, *quantizerOptions->scale, quantizedInput);
#if 0
        SQINFO("trigger input=%f  input+5th=%f q=%d, q+7=%d this=%p", 
            input,input + 7.f / 12.f,  
            mn.get(), mn.get()+7, 
            this );
#endif

#if 0
            bool octaveJump = false;
            if (chordB) {
                octaveJump = (chordB->fetchRoot() == (1 + scaleNote.getDegree()));
            } else if (chordA) {
                octaveJump = (chordA->fetchRoot() == (1 + scaleNote.getDegree()));
            }
            if (octaveJump) {
                SQINFO("\nignoring octave jump\n");
            } else {
#endif
        {
            const bool show = false;

            const Chord4* chord = HarmonyChords::findChord2(
                show,
                1 + scaleNote.getDegree(),
                *chordOptions,
                *chordManager,
                &chordHistory,
                chordA, chordB);
            if (chord) {
                outputPitches(chord);
                chordA = chordB;
                chordB = chord;
                // SQINFO("gen new %s", chord->toString().c_str());
            } else {
                SQWARN("got no chord");
            }
#if 0
            // If it's our first chord, generate single.
            // TODO: shouldn't we have done this before? What are we outputting?
            if (!chordA) {
                chordA = HarmonyChords::findChord(show, *chordOptions, *chordManager, 1 + scaleNote.getDegree());
                outputPitches(chordA);
            } else if (!chordB) {
                chordB = HarmonyChords::findChord(show, *chordOptions, *chordManager, *chordA, 1 + scaleNote.getDegree());
                outputPitches(chordB);
            } else {
                const Chord4* chord = HarmonyChords::findChord(show, *chordOptions, *chordManager, *chordA, *chordB, 1 + scaleNote.getDegree());
                outputPitches(chord);
                chordA = chordB;
                chordB = chord;
            }
#endif
        }

        lastQuantizedPitch = quantizedFloatNote.get();
    }

    if (mustUpdate) {
        updateEverything();
    }
}