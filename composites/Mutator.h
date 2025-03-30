#include "FloatNote.h"
#include "GateTrigger.h"
#include "MelodyGenerator.h"
#include "NoteConvert.h"

namespace rack {
namespace engine {
struct Module;
}
}  // namespace rack
using Module = ::rack::engine::Module;

template <class TBase>
class Mutator : public TBase {
public:
    Mutator(Module* module) : TBase(module) {
        _init();
    }
    Mutator() : TBase() {
        _init();
    }

    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        MUTATE_INPUT,
        NUM_INPUTS
    };

    enum OutputIds {
        NOTES_OUTPUT,
        NUM_OUTPUTS
    };

    enum LightIds {
        NUM_LIGHTS
    };

    void process(const typename TBase::ProcessArgs& args) override;

private:
    void _init();
    void _processTrigger();

    GateTrigger _mutateTrigger;

    MelodyRow _theNoteData;
    Scale _theScale;
    MelodyMutateState _theState;
    MelodyMutateStyle _theStyle;
};

template <class TBase>
inline void Mutator<TBase>::_init() {
    MidiNote base(MidiNote::C);
    _theScale.set(base, Scale::Scales::Major);
    _theNoteData.init(8, _theScale);
}

template <class TBase>
inline void Mutator<TBase>::process(const typename TBase::ProcessArgs& args) {
    const float mutateInput = TBase::inputs[MUTATE_INPUT].getVoltage(0);
    _mutateTrigger.go(mutateInput);
    if (_mutateTrigger.trigger()) {
        // SQINFO("trigger");
        _processTrigger();
    }

    // SQINFO("x %llu", _theNoteData.getSize());

    // This could be done only on change, if desired.
    TBase::outputs[NOTES_OUTPUT].setChannels(_theNoteData.getSize());
    for (size_t i = 0; i < _theNoteData.getSize(); ++i) {
        //  SQINFO("set volt(1.3, %llu)", i);

        FloatNote floatNote;
        NoteConvert::m2f(floatNote, _theNoteData.getNote(i));
        TBase::outputs[NOTES_OUTPUT].setVoltage(floatNote.get(), i);
    }
}

template <class TBase>
inline void Mutator<TBase>::_processTrigger() {
    SQINFO("process trigger");
    MelodyGenerator::mutate(_theNoteData, _theScale, _theState, _theStyle);
    SQINFO("notes: %s", _theNoteData.print().c_str());
}
