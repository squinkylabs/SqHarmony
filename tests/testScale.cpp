#include "FloatNote.h"
#include "NoteConvert.h"
#include "Scale.h"
#include "SqLog.h"
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

static void testNoCrash(const Scale& scale) {
    for (int i = 0; i < 11; ++i) {
        const int x = scale.quantize(i);
        assertGE(x, 0);
        assertLE(x, 7);
    }
}

static void testGeneral() {
    for (int root = 0; root < 12; ++root) {
        for (int mode = 0; mode <= int(Scale::Scales::Chromatic); ++mode) {
            Scale scale;
            Scale::Scales emode = Scale::Scales(mode);
            scale.set(root, emode);
            for (int testPitch = 0; testPitch < 12; testPitch++) {
                const int x = scale.quantize(testPitch);
            }
        }
    }
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
        assertLT(t, MidiNote::MiddleC + 20);

        assertGE(b, MidiNote::MiddleC - (12 + 5));
        assertLE(b, MidiNote::MiddleC - 3);
    }

    for (int i = 0; i < info.numFlats; ++i) {
        assert(info.flatsInBassClef);
        assert(info.flatsInTrebleClef);

        int t = info.flatsInTrebleClef[i].get();
        int b = info.flatsInBassClef[i].get();
        assertGT(t, MidiNote::MiddleC);
        assertLT(t, MidiNote::MiddleC + 19);

        assertGE(b, MidiNote::MiddleC - (12 + 5));
        assertLE(b, MidiNote::MiddleC - 3);
    }
}

static void validate(const Scale::ScoreInfo& info, int expectedSharps, int expectedFlats) {
    // assert(expectedFlats==0 || expectedSharps==0);
    assertEQ(info.numFlats, expectedFlats);
    assertEQ(info.numSharps, expectedSharps);
    validate(info);
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

static void testSharpsFlatsDiatonic() {
    Scale scale;

    scale.set(MidiNote::C, Scale::Scales::Minor);
    auto info = scale.getSharpsFlatsPref();
    assert(info == Scale::SharpsFlatsPref::Flats);

    scale.set(MidiNote::C, Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == Scale::SharpsFlatsPref::DontCare);

    scale.set(MidiNote::C + 1, Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == Scale::SharpsFlatsPref::Sharps);

    scale.set(MidiNote::D, Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == Scale::SharpsFlatsPref::Sharps);

    scale.set(MidiNote::D + 1, Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == Scale::SharpsFlatsPref::Flats);

    scale.set(MidiNote::E, Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == Scale::SharpsFlatsPref::Sharps);

    scale.set(MidiNote::F, Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == Scale::SharpsFlatsPref::Flats);

    scale.set(MidiNote::F + 1, Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == Scale::SharpsFlatsPref::DontCare);

    scale.set(MidiNote::G, Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == Scale::SharpsFlatsPref::Sharps);

    scale.set(MidiNote::G + 1, Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == Scale::SharpsFlatsPref::Flats);

    scale.set(MidiNote::A, Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == Scale::SharpsFlatsPref::Sharps);

    scale.set(MidiNote::A + 1, Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == Scale::SharpsFlatsPref::Flats);

    scale.set(MidiNote::B, Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == Scale::SharpsFlatsPref::Sharps);
}

static void testSharpsFlatsOtherMinor() {
    Scale scale;
    scale.set(MidiNote::E, Scale::Scales::MinorPentatonic);
    auto info = scale.getSharpsFlatsPref();
    assert(info == Scale::SharpsFlatsPref::Sharps);

    scale.set(MidiNote::E -1, Scale::Scales::MinorPentatonic);
    info = scale.getSharpsFlatsPref();
    assert(info == Scale::SharpsFlatsPref::DontCare);

    scale.set(MidiNote::E - 1, Scale::Scales::MinorPentatonic);
    info = scale.getSharpsFlatsPref();
    assert(info == Scale::SharpsFlatsPref::DontCare);

    scale.set(MidiNote::G, Scale::Scales::MinorPentatonic);
    info = scale.getSharpsFlatsPref();
    assert(info == Scale::SharpsFlatsPref::Flats);


    
    assert(false);
}

static void testScore3() {
    Scale scale;

    // all the natural modes
    scale.set(MidiNote::C, Scale::Scales::Major);
    validate(scale.getScoreInfo(), 0, 0);
    scale.set(MidiNote::D, Scale::Scales::Dorian);
    validate(scale.getScoreInfo(), 0, 0);
    scale.set(MidiNote::E, Scale::Scales::Phrygian);
    validate(scale.getScoreInfo(), 0, 0);
    scale.set(MidiNote::F, Scale::Scales::Lydian);
    validate(scale.getScoreInfo(), 0, 0);
    scale.set(MidiNote::G, Scale::Scales::Mixolydian);
    validate(scale.getScoreInfo(), 0, 0);
    scale.set(MidiNote::A, Scale::Scales::Minor);
    validate(scale.getScoreInfo(), 0, 0);
    scale.set(MidiNote::B, Scale::Scales::Locrian);
    validate(scale.getScoreInfo(), 0, 0);

    // all the one sharp scales
    scale.set(MidiNote::G, Scale::Scales::Major);
    validate(scale.getScoreInfo(), 1, 0);
    scale.set(MidiNote::A, Scale::Scales::Dorian);
    validate(scale.getScoreInfo(), 1, 0);
    scale.set(MidiNote::B, Scale::Scales::Phrygian);
    validate(scale.getScoreInfo(), 1, 0);
    scale.set(MidiNote::C, Scale::Scales::Lydian);
    validate(scale.getScoreInfo(), 1, 0);
    scale.set(MidiNote::D, Scale::Scales::Mixolydian);
    validate(scale.getScoreInfo(), 1, 0);
    scale.set(MidiNote::E, Scale::Scales::Minor);
    validate(scale.getScoreInfo(), 1, 0);
    scale.set(MidiNote::F + 1, Scale::Scales::Locrian);
    validate(scale.getScoreInfo(), 1, 0);

    // all the two sharp scales
    scale.set(MidiNote::D, Scale::Scales::Major);
    validate(scale.getScoreInfo(), 2, 0);
    scale.set(MidiNote::E, Scale::Scales::Dorian);
    validate(scale.getScoreInfo(), 2, 0);
    scale.set(MidiNote::F + 1, Scale::Scales::Phrygian);
    validate(scale.getScoreInfo(), 2, 0);
    scale.set(MidiNote::G, Scale::Scales::Lydian);
    validate(scale.getScoreInfo(), 2, 0);
    scale.set(MidiNote::A, Scale::Scales::Mixolydian);
    validate(scale.getScoreInfo(), 2, 0);
    scale.set(MidiNote::B, Scale::Scales::Minor);
    validate(scale.getScoreInfo(), 2, 0);
    scale.set(MidiNote::C + 1, Scale::Scales::Locrian);
    validate(scale.getScoreInfo(), 2, 0);

    scale.set(MidiNote::A, Scale::Scales::Major);
    validate(scale.getScoreInfo(), 3, 0);

    scale.set(MidiNote::E, Scale::Scales::Major);
    validate(scale.getScoreInfo(), 4, 0);

    // b maj = 5 sharp or 7 flats
    scale.set(MidiNote::B, Scale::Scales::Major);
    validate(scale.getScoreInfo(), 5, 7);

    scale.set(MidiNote::F + 1, Scale::Scales::Major);
    validate(scale.getScoreInfo(), 6, 6);

    // c# major / d- major
    scale.set(MidiNote::C + 1, Scale::Scales::Major);
    validate(scale.getScoreInfo(), 7, 5);

    // one flat
    scale.set(MidiNote::F, Scale::Scales::Major);
    validate(scale.getScoreInfo(), 0, 1);

    // 2 flat b- major
    scale.set(MidiNote::B - 1, Scale::Scales::Major);
    validate(scale.getScoreInfo(), 0, 2);

    // 3 flat e- maj
    scale.set(MidiNote::E - 1, Scale::Scales::Major);
    validate(scale.getScoreInfo(), 0, 3);

    // 4 flats A- major = g# major
    scale.set(MidiNote::A - 1, Scale::Scales::Major);
    validate(scale.getScoreInfo(), 0, 4);

    //  5 flats d- major . also c# major - 7 sharps
    scale.set(MidiNote::D - 1, Scale::Scales::Major);
    validate(scale.getScoreInfo(), 7, 5);

    // 6 flats g- major, algo f# mar
    scale.set(MidiNote::G - 1, Scale::Scales::Major);
    validate(scale.getScoreInfo(), 6, 6);

    // 7 flats c- major. also bmaj = 5 sharps
    scale.set(MidiNote::C + 12 - 1, Scale::Scales::Major);
    validate(scale.getScoreInfo(), 5, 7);
}

static void testLabels(const std::vector<std::string>& labels) {
    std::set<std::string> x;
    assert(!labels.empty());
    for (auto s : labels) {
        assert(!s.empty());
        if (x.find(s) != x.end()) assert(false);
        x.insert(s);
    }
    assert(!x.empty());
}

static void testLabels() {
    assertEQ(Scale::getShortScaleLabels(false).size(), Scale::getScaleLabels(false).size());
    assertEQ(Scale::getShortScaleLabels(true).size(), Scale::getScaleLabels(true).size());
    assertGT(Scale::getShortScaleLabels(false).size(), Scale::getShortScaleLabels(true).size());

    testLabels(Scale::getShortScaleLabels(false));
    testLabels(Scale::getShortScaleLabels(true));
    testLabels(Scale::getScaleLabels(false));
    testLabels(Scale::getScaleLabels(true));
}

void testScale() {
    testCMaj();
    testAMin();
    testGeneral();
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
    testScore3();

    testLabels();

    testSharpsFlatsDiatonic();
    testSharpsFlatsOtherMinor();
}

#if 1
void testFirst() {
    testSharpsFlatsOtherMinor();
}
#endif