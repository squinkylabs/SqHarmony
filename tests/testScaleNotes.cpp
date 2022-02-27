
#include "FloatNote.h"
#include "NoteConvert.h"
#include "Scale.h"

#include "asserts.h"
static void testm2sRoundTrip() {
    Scale scale;
    scale.set(4, Scale::Scales::Major);
    for (int i = 0; i < 128; ++i) {
        MidiNote mn(i);
        ScaleNote sn;
        NoteConvert::m2s(sn, scale, mn);
        MidiNote mn2;
        NoteConvert::s2m(mn2, scale, sn);
        assert(mn == mn2);
    }
}

static void testf2sCMajC4() {
    Scale scale;
    scale.set(MidiNote::C, Scale::Scales::Major);

    // float is VCV standard: 0 = c4
    const FloatNote fn;
    ScaleNote sn;
    NoteConvert::f2s(sn, scale, fn);
    assertEQ(sn.getDegree(), 0);
    assert(!sn.isAccidental());
    assertEQ(sn.getOctave(), 4);
}

static void testf2sRoundTrip() {
    Scale scale;
    scale.set(MidiNote::C + 4, Scale::Scales::Major);

    for (int i = -20; i < 200; ++i) {
        MidiNote mn(i);
        FloatNote fn;
        ScaleNote sn;
        NoteConvert::m2f(fn, mn);
        NoteConvert::f2s(sn, scale, fn);
        FloatNote fn2;
        NoteConvert::s2f(fn2, scale, sn);
        MidiNote mn2;
        NoteConvert::f2m(mn2, fn2);
        assert(mn == mn2);
    }
}

 static void testf2sCMajCsharp5() {
     Scale scale;
     scale.set(MidiNote::C, Scale::Scales::Major);

     const FloatNote fn(1 + (1.f / 12.f));        // c sharp
     ScaleNote sn;
     NoteConvert::f2s(sn, scale, fn);
     assertEQ(sn.getDegree(), 0);
     assert(sn.isAccidental());
     assert(sn.getAccidental() == ScaleNote::Accidental::sharp);
     assertEQ(sn.getOctave(), 5);      // c4 is defaulat
 }

static void tests2fCMajC4() {
    Scale scale;
    scale.set(MidiNote::C, Scale::Scales::Major);

    ScaleNote sn(0, 4);         // C4

    FloatNote fn;

    NoteConvert::s2f(fn, scale, sn);
    assertEQ(fn.get(), 0);
    
}



void testScaleNotes() {
   

    testf2sCMajC4();
    testf2sCMajCsharp5();
    
    tests2fCMajC4();


    testm2sRoundTrip();
    testf2sRoundTrip();



}