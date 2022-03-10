#pragma once

#include "AtomicRingBuffer.h"
#include "Chord4.h"
#include "Chord4Manager.h"
#include "Divider.h"
#include "FloatNote.h"
#include "HarmonyChords.h"
#include "Keysig.h"
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
        NUM_PARAMS
    };
    enum InputIds {
        CV_INPUT,
        NUM_INPUTS
    };

    enum OutputIds {
        QUANTIZER_OUTPUT,
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

    bool isChordAvailable() const {
        return !chordsOut.empty();
    }
    Chord getChord() {
        return chordsOut.pop();
    }

private:
    void init();
    void outputPitches(const Chord4*);
    void stepn();

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
    float lastQuantizedPitch = -100;
    int count = 0;
};

template <class TBase>
inline void Harmony<TBase>::init() {
    auto keysig = std::make_shared<Keysig>(Roots::C);
    auto style = std::make_shared<Style>();
    chordOptions = std::make_shared<Options>(keysig, style);

    quantizerOptions = std::make_shared<ScaleQuantizer::Options>();
    quantizerOptions->scale = std::make_shared<Scale>();
    quantizerOptions->scale->set(MidiNote::C, Scale::Scales::Major);
    inputQuantizer = std::make_shared<ScaleQuantizer>(quantizerOptions);

    chordManager = std::make_shared<Chord4Manager>(*chordOptions);

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
#if 0
    SQINFO("final debt = %d", debt);
    for (int i = 0; i < 4; ++i) {
        SQINFO("voice %d output = %d, channel = %d", i, voiceToOutput[i], voiceToChannel[i]);
    }
#endif
// NNIC_PREFERENCE_PARAM
    bool noNotesInCommon =  Harmony<TBase>::params[NNIC_PREFERENCE_PARAM].value > .5;
    //  OptionsPtr chordOptions;
    auto style = chordOptions->style;
    style->setNoNotesInCommon(noNotesInCommon);

//    this->configSwitch(Comp::CENTER_PREFERENCE_PARAM, 0, 2, 0, "Centered preference",
// {"None", "A little", "A lot"});
   
    Style::Ranges range = Style::Ranges::NORMAL_RANGE;
    const int i = int(std::round(Harmony<TBase>::params[CENTER_PREFERENCE_PARAM].value));
    switch(i) {
        case 0:
            range = Style::Ranges::NORMAL_RANGE;
            break;
        case 1:
            range = Style::Ranges::NARROW_RANGE;
            break;
        case 2:
            range = Style::Ranges::ENCOURAGE_CENTER;
            break;
        default:
            assert(false);
    }

    style->setRangesPreference(range);
}

template <class TBase>
inline void Harmony<TBase>::outputPitches(const Chord4* chord) {
    const HarmonyNote* harmonyNotes = chord->fetchNotes();
    Chord c;

    c.root = chord->fetchRoot();
    c.inversion = int(chord->inversion(*chordOptions));

   // SQINFO("output pitches %s (bass=%d)", chord->toStringShort().c_str(), (int)harmonyNotes[0]);

    for (int i = 0; i < 4; ++i) {
        MidiNote mn(12 + harmonyNotes[i]);  // harmony note and midi note are about the same;
        FloatNote fn;
        NoteConvert::m2f(fn, mn);
        const int outputPort = voiceToOutput[i];
        const int outputChannel = voiceToChannel[i];
        Harmony<TBase>::outputs[outputPort].setVoltage(fn.get(), outputChannel);
       //SQINFO("set output[%d] to %f from base pitch %f", i, fn.get(), fn.get());
        c.pitch[i] = mn.get();
    }

    if (!chordsOut.full()) {
        chordsOut.push(c);
    } else {
        SQWARN("in outputPitches, no room for output\n");
        //fflush(stdout);
    }
}

template <class TBase>
inline void Harmony<TBase>::process(const typename TBase::ProcessArgs& args) {
    divn.step();
    //   static int count = 0;
    const float input = Harmony<TBase>::inputs[CV_INPUT].getVoltage(0);
    ++count;
    // if (count < 20)
    //     fprintf(stderr, "\n==== process[%d] input = %f this=%p\n", count, input, this);
    assert(chordManager);
    MidiNote mn = inputQuantizer->run(input);
    FloatNote quantizedNote;
    NoteConvert::m2f(quantizedNote, mn);
    Harmony<TBase>::outputs[QUANTIZER_OUTPUT].setVoltage(quantizedNote.get(), 0);

    // generate a new chord any time the quantizer outputs a new pitch
    if (quantizedNote.get() != lastQuantizedPitch) {
        #if 0
        printf("got new Q pitch v=%f semis=%f q=%f\n",
               input,
               input * 12,
               quantizedNote.get());
         #endif
        ScaleNote scaleNote;
        NoteConvert::m2s(scaleNote, *quantizerOptions->scale, mn);

        bool octaveJump = false;
        if (chordB) {
            octaveJump = (chordB->fetchRoot() == (1 + scaleNote.getDegree()));
        } else if (chordA) {
            octaveJump = (chordA->fetchRoot() == (1 + scaleNote.getDegree()));
        }
        if (octaveJump) {
            printf("\nignoring octave jump\n");
        } else {

        #if 0
            printf("\n** new Q note m=%d f=%f last=%f ct=%d\n",
                   mn.get(),
                   quantizedNote.get(),
                   lastQuantizedPitch,
                   count);
            fflush(stdout);
        #endif

            const bool show = false;
            // If it's our first chord, generate single.
            // TODO: shouldn't we have done this before? What are we outputting?
            if (!chordA) {
               // printf("process new chords case 1\n");
               // fflush(stdout);
                chordA = HarmonyChords::findChord(show, *chordOptions, *chordManager, 1 + scaleNote.getDegree());
                outputPitches(chordA);
              //  printf("after process new chords case 1\n");
             //   fflush(stdout);
            } else if (!chordB) {
             //   printf("process new chords case 2\n");
             //   fflush(stdout);
                chordB = HarmonyChords::findChord(show, *chordOptions, *chordManager, *chordA, 1 + scaleNote.getDegree());
                outputPitches(chordB);
            } else {
              //  printf("process new chords case 3\n");
              //  fflush(stdout);
                const Chord4* chord = HarmonyChords::findChord(show, *chordOptions, *chordManager, *chordA, *chordB, 1 + scaleNote.getDegree());
                outputPitches(chord);
                chordA = chordB;
                chordB = chord;
            }
        }

        lastQuantizedPitch = quantizedNote.get();

       // printf("SET lqp to %f\n", lastQuantizedPitch);
       // fflush(stdout);
    }
    if (count < 20) {
        //  fprintf(stderr, "leave proc= process[%d] input = %f\n", count, input);
        //  fflush(stderr);
    }
}