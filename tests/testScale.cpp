#include "FloatNote.h"
#include "NoteConvert.h"
#include "Scale.h"
#include "asserts.h"

static void testCMaj() {
    Scale scale;
    scale.set(4, Scale::Scales::Major);
    assertEQ(scale.degreeToSemitone(0), 0);   // C
    assertEQ(scale.degreeToSemitone(1), 2);   // D
    assertEQ(scale.degreeToSemitone(2), 4);   // E
    assertEQ(scale.degreeToSemitone(3), 5);   // F
    assertEQ(scale.degreeToSemitone(4), 7);   // G
    assertEQ(scale.degreeToSemitone(5), 9);   // A
    assertEQ(scale.degreeToSemitone(6), 11);  // B
}

static void testAMin() {
    Scale scale;
    scale.set(4 - 3, Scale::Scales::Minor);
    assertEQ(scale.degreeToSemitone(0), 0);   // A
    assertEQ(scale.degreeToSemitone(1), 2);   // B
    assertEQ(scale.degreeToSemitone(2), 3);   // C
    assertEQ(scale.degreeToSemitone(3), 5);   // D
    assertEQ(scale.degreeToSemitone(4), 7);   // E
    assertEQ(scale.degreeToSemitone(5), 8);   // F
    assertEQ(scale.degreeToSemitone(6), 10);  // G
}

static void testChromatic() {
    Scale scale;
    // C chromatic
    scale.set(MidiNote::C, Scale::Scales::Chromatic);

    assertEQ(scale.degreeToSemitone(0), 0);  // A
    assertEQ(scale.degreeToSemitone(1), 1);
    assertEQ(scale.degreeToSemitone(2), 2);
    assertEQ(scale.degreeToSemitone(3), 3);
    assertEQ(scale.degreeToSemitone(4), 4);
    assertEQ(scale.degreeToSemitone(5), 5);
    assertEQ(scale.degreeToSemitone(6), 6);
    assertEQ(scale.degreeToSemitone(7), 7);
    assertEQ(scale.degreeToSemitone(8), 8);
    assertEQ(scale.degreeToSemitone(9), 9);
    assertEQ(scale.degreeToSemitone(10), 10);
    assertEQ(scale.degreeToSemitone(11), 11);
}

static void testQuantCMin() {
    Scale scale;
    scale.set(4, Scale::Scales::Minor);
    // already in scale
    assertEQ(scale.quantize(0), 0);   // c -> c (0)
    assertEQ(scale.quantize(2), 1);   // d -> d (1)
    assertEQ(scale.quantize(3), 2);   // e- -> e- (2)
    assertEQ(scale.quantize(5), 3);   // f -> f (3)
    assertEQ(scale.quantize(7), 4);   // g -> g (4)
    assertEQ(scale.quantize(8), 5);   // a- -> a- (5)
    assertEQ(scale.quantize(10), 6);  // b- -> b- (6)

    // not in scale
    assertEQ(scale.quantize(1), 1);   // m2 -> M2
    assertEQ(scale.quantize(4), 2);   // M3 -> m3
    assertEQ(scale.quantize(6), 4);   // tritone -> g (4)
    assertEQ(scale.quantize(9), 5);   // M6 -> m6
    assertEQ(scale.quantize(11), 6);  // M7 -> m7
}

static void testQuantCMaj() {
    Scale scale;
    scale.set(4, Scale::Scales::Major);
    // already in scale
    assertEQ(scale.quantize(0), 0);  // c -> c (0)
    assertEQ(scale.quantize(2), 1);  // d -> d (1)
    assertEQ(scale.quantize(4), 2);  // e -> e (2)
    assertEQ(scale.quantize(5), 3);  // f -> f (3)
    assertEQ(scale.quantize(7), 4);  // g -> g (4)
    assertEQ(scale.quantize(9), 5);  // a -> a (5)
    assertEQ(scale.quantize(11), 6);

    // not in scale
    assertEQ(scale.quantize(1), 1);   // m2 -> M2
    assertEQ(scale.quantize(3), 2);   // m3 -> M3
    assertEQ(scale.quantize(6), 4);   // tritone -> g (4)
    assertEQ(scale.quantize(8), 4);   // aug 5th -> g (4)
    assertEQ(scale.quantize(10), 6);  // dim 7th -> b (6)
}

static void testQuantEPenta() {
    Scale scale;
    scale.set(4 + 4, Scale::Scales::MinorPentatonic);

    // already in scale
    assertEQ(scale.quantize(0), 0);   // e -> e (0)
    assertEQ(scale.quantize(3), 1);   // g -> g (1)
    assertEQ(scale.quantize(5), 2);   // a -> a (2)
    assertEQ(scale.quantize(7), 3);   // b -> b (3)
    assertEQ(scale.quantize(10), 4);  // d -> D (4)

    // not in scale
    assertEQ(scale.quantize(1), 0);  // m2 -> root
    assertEQ(scale.quantize(2), 1);  // d -> d (1)
    assertEQ(scale.quantize(4), 1);  // M3 -> m3

    assertEQ(scale.quantize(6), 3);  // tritone -> g (4)

    assertEQ(scale.quantize(9), 4);   // M6 -> m7
    assertEQ(scale.quantize(11), 4);  // M7 -> m7
}

static void testm2sCMajC3() {
    Scale scale;
    scale.set(MidiNote::C, Scale::Scales::Major);


    MidiNote c(MidiNote::C3);
    ScaleNote sn = scale.m2s(c);
    assertEQ(sn.getDegree(), 0);
    assertEQ(sn.getOctave(), 3);
    assert(!sn.isAccidental());
}

static void testm2sCMajC4() {
    Scale scale;
    scale.set(MidiNote::C, Scale::Scales::Major);

    // middle c is degree 0 in cmaj
    MidiNote c(MidiNote::C3 + 12);
    ScaleNote sn = scale.m2s(c);
    assertEQ(sn.getDegree(), 0);
    assertEQ(sn.getOctave(), 4);
    assert(!sn.isAccidental());
}

static void testm2sCMajCsharp4() {
    Scale scale;
    scale.set(MidiNote::C, Scale::Scales::Major);

    // MAKE C#4
    MidiNote c(MidiNote::C3 + 12 + 1);
    ScaleNote sn = scale.m2s(c);
    assertEQ(sn.getDegree(), 0);
    assertEQ(sn.getOctave(), 4);
    assert(sn.getAccidental() == ScaleNote::Accidental::sharp);
}

static void tests2mCMajC4() {
    Scale scale;
    scale.set(MidiNote::C, Scale::Scales::Major);

    // middle c is degree 0 in cmaj, octave c
    ScaleNote sn(0, 3);
    MidiNote mn = scale.s2m(sn);
    assertEQ(mn.get(), MidiNote::C3);
}

static void tests2mCMajC5() {
    Scale scale;
    scale.set(MidiNote::C, Scale::Scales::Major);

    // middle c is degree 0 in cmaj
    ScaleNote sn(0, 5);
    MidiNote mn = scale.s2m(sn);
    assertEQ(mn.get(), MidiNote::C3 + 2 * 12);
}

static void validate(const Scale::ScoreInfo& info) {

   for (int i = 0; i < info.numSharps; ++i) {
       assert(info.sharpsInBassClef);
       assert(info.sharpsInTrebleClef);

       int t = info.sharpsInTrebleClef[i].get();
       int b = info.sharpsInBassClef[i].get();
       assertGT(t, MidiNote::MiddleC + 4);
       assertLT(t, MidiNote::MiddleC + 19);
   }

    for (int i = 0; i < info.numFlats; ++i) {
       assert(info.flatsInBassClef);
       assert(info.flatsInTrebleClef);

       int t = info.flatsInTrebleClef[i].get();
       int b = info.flatsInBassClef[i].get();
       assertGT(t, MidiNote::MiddleC);
       assertLT(t, MidiNote::MiddleC + 19);
   }
}

static void testScore() {
    Scale scale;
    scale.set(MidiNote::C, Scale::Scales::Major);

    auto info = scale.getScoreInfo();
    assertEQ(info.numFlats, 0);
    assertEQ(info.numSharps, 0);
    validate(info);

    // gmaj
    scale.set(MidiNote::C + 7, Scale::Scales::Major);
    info = scale.getScoreInfo();
    assertEQ(info.numFlats, 0);
    assertEQ(info.numSharps, 1);
    validate(info);
}

static void testScore2() {
    Scale scale;

    // E flat is three flat major
    scale.set(MidiNote::C + 3, Scale::Scales::Major);

    auto info = scale.getScoreInfo();
    assertEQ(info.numFlats, 3);
    assertEQ(info.numSharps, 0);
    validate(info);

    // C Min is the relative minor 
    scale.set(MidiNote::C, Scale::Scales::Minor);
    info = scale.getScoreInfo();
    assertEQ(info.numFlats, 3);
    assertEQ(info.numSharps, 0);
    validate(info);


}


void testScale() {
    testCMaj();
    testAMin();
    testChromatic();

    testQuantCMaj();
    testQuantCMin();
    testQuantEPenta();

    testm2sCMajC3();
    testm2sCMajC4();
    testm2sCMajCsharp4();

    tests2mCMajC4();
    tests2mCMajC5();

    testScore();
    testScore2();
}