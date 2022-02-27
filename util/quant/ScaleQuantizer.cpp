
#include "FloatNote.h"
#include "NoteConvert.h"
#include "Scale.h"
#include "ScaleQuantizer.h"

#include <assert.h>

ScaleQuantizer::ScaleQuantizer(ConstOptionsPtr opt) : options(opt) {
    assert(options);
    assert(options->scale);
}


ScaleQuantizerPtr ScaleQuantizer::makeTest(int baseMidiPitch, Scale::Scales scaleChoice) {
    auto scale = std::make_shared<Scale>();
    scale->set(baseMidiPitch, scaleChoice);
    auto options = std::make_shared<Options>();
    options->scale = scale;
    auto quantizer = std::make_shared<ScaleQuantizer>(options);
    return quantizer;
}

ScaleQuantizerPtr ScaleQuantizer::makeTestCMaj() {
    return makeTest(MidiNote::C, Scale::Scales::Major);
}
#if 0
ScaleQuantizerPtr ScaleQuantizer::makeTestCMaj() {
    auto scale = std::make_shared<Scale>();
    scale->set(MidiNote::C, Scale::Scales::Major);
    auto options = std::make_shared<Options>();
    options->scale = scale;
    auto quantizer = std::make_shared<ScaleQuantizer>(options);
    return quantizer;
}
#endif


MidiNote ScaleQuantizer::run(float voltage) {
    FloatNote fn(voltage);
    MidiNote mn;
    NoteConvert::f2m(mn, fn);

    assert(options->scale->getWasSet());
    MidiNote baseMidiNote = options->scale->base();
  
    int octave = 0;

    // this seems to be mixing up pitch and octave.
    // TODO: test with other base pitches (not just CMaj)
    int relativeMidiPitch = (mn.get() % 12) - baseMidiNote.get();
    if (relativeMidiPitch < 0) {
        relativeMidiPitch += 12;
        octave--;
    }
    assert(relativeMidiPitch >= 0 && relativeMidiPitch < 12);
    assert(options);
    assert(options->scale);
    const int quantizedDegree = options->scale->quantize(relativeMidiPitch);
    octave += (mn.get() / 12);
    octave -= 2;                    // not sure where this 2 octaves comes from, but middle C.get /12 is 5, so...
    ScaleNote quantizedScaleNote(quantizedDegree, octave);
    MidiNote quantizedMn;
    NoteConvert::s2m(quantizedMn, *options->scale, quantizedScaleNote);

    return quantizedMn;
}