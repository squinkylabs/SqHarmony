
#include "asserts.h"

#include "MelodyGenerator.h"

#include "NoteConvert.h"
#include "Scale.h"


static Scale scaleCMaj() {
    Scale scale;
    MidiNote base(MidiNote::C);
    scale.set(base, Scale::Scales::Major);
    return scale;
}

static void testMelodyRowSize() {
    MelodyRow r;
    r.setSize(2);
    assertEQ(r._getSize(), 2);
} 

static void testMelodyRowInit() {
    MelodyRow r;
    size_t size = 7;
    Scale scale = scaleCMaj();

  
    r.init(size, scale);

    assertEQ(r._getSize(), size);

    for (unsigned i = 0; i < size; ++i) {
        ScaleNote scaleNote;
        const MidiNote midiNote = r.getNote(i);
        NoteConvert::m2s(scaleNote, scale, midiNote);       
        assertEQ(scaleNote.getDegree(), 0);
        assertEQ(scaleNote.getOctave(), 4);
    }
}

static void testMelodyRowEqual() {
    MelodyRow r;
    size_t size = 3;
    Scale scale;

    // test in E phrygian
    MidiNote base(MidiNote::E);
    scale.set(base, Scale::Scales::Phrygian);

    {
        // sanity check
        ScaleNote scaleNote;
        NoteConvert::m2s(scaleNote, scale, base);
        assertEQ(scaleNote.getDegree(), 0);
    }

    r.init(size, scale);
    MelodyRow r2 = r;
    assert(r == r2);


    Scale scale2;
    MidiNote base2(MidiNote::F);
    scale2.set(base2, Scale::Scales::Phrygian);
    r2.init(size, scale2);
    assert(r != r2);

    r2 = r;
    r2.setSize(size + 1);
    assert(r != r2);

}

static void testMelodyRow() {
    testMelodyRowSize();
    testMelodyRowInit();
    testMelodyRowEqual();
}

////////////////////////////////////////

static void testMelodyGeneratorCanCall() {
    //  static void mutate(MelodyRow& row, MelodyMutateState& state, MelodyMutateStyle& style);

    MelodyRow r;
    MelodyMutateState state;
    MelodyMutateStyle style;
    Scale scale = scaleCMaj();
    r.init(1, scale);
    MelodyGenerator::mutate(r, scale, state, style);
}

static void testMelodyGeneratorWillMutate() {
    MelodyRow r;
    const MelodyRow rOrig(r);
    MelodyMutateState state;
    MelodyMutateStyle style;
    Scale scale = scaleCMaj();
    r.init(1, scale);

    MelodyGenerator::mutate(r, scale, state, style);
    assert(r != rOrig);
}

static void testMelodyGenerator2() {
    testMelodyGeneratorCanCall();
    testMelodyGeneratorWillMutate();

}
void testMelodyGenerator() {
    testMelodyRow();
    testMelodyGenerator2();
}

void testFirst() {
   testMelodyGenerator();
}