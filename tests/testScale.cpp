#include "FloatNote.h"
#include "NoteConvert.h"
#include "Scale.h"
#include "SqLog.h"
#include "asserts.h"

static void testCMaj() {
    Scale scale;
    scale.set(MidiNote(4), Scale::Scales::Major);
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
        for (int mode = Scale::firstScale; mode <= Scale::lastScale; ++mode) {
            Scale scale;
            Scale::Scales emode = Scale::Scales(mode);
            scale.set(MidiNote(root), emode);
            for (int testPitch = 0; testPitch < 12; testPitch++) {
                const int x = scale.quantize(testPitch);
            }
        }
    }
}

static void testAMin() {
    Scale scale;
    scale.set(MidiNote(4 - 3), Scale::Scales::Minor);
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
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Chromatic);

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
    scale.set(MidiNote(4), Scale::Scales::Minor);
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
    scale.set(MidiNote(4), Scale::Scales::Major);
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
    scale.set(MidiNote(4 + 4), Scale::Scales::MinorPentatonic);

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
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);

    MidiNote c(MidiNote::C3);
    ScaleNote sn = scale.m2s(c);
    assertEQ(sn.getDegree(), 0);
    assertEQ(sn.getOctave(), 3);
    assert(!sn.isAccidental());
}

static void testm2sCMajC4() {
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);

    // middle c is degree 0 in cmaj
    MidiNote c(MidiNote::C3 + 12);
    ScaleNote sn = scale.m2s(c);
    assertEQ(sn.getDegree(), 0);
    assertEQ(sn.getOctave(), 4);
    assert(!sn.isAccidental());
}

static void testm2sCMajCsharp4() {
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);

    // MAKE C#4
    MidiNote c(MidiNote::C3 + 12 + 1);
    ScaleNote sn = scale.m2s(c);
    assertEQ(sn.getDegree(), 0);
    assertEQ(sn.getOctave(), 4);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::sharp);
}

#if 0
static void testm2sDSharp() {
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Minor);

    // MAKE D#4
    MidiNote c(MidiNote::C3 + 12 + 3);
    ScaleNote sn = scale.m2s(c, true);

    //assert(false);
    assertEQ(sn.getDegree(), 1);
    assertEQ(sn.getOctave(), 4);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::sharp);
}
#endif

static void tests2mCMajC4() {
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);

    // middle c is degree 0 in cmaj, octave c
    ScaleNote sn(0, 3);
    MidiNote mn = scale.s2m(sn);
    assertEQ(mn.get(), MidiNote::C3);
}

static void tests2mCMajC5() {
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);

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
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);

    auto info = scale.getScoreInfo();
    assertEQ(info.numFlats, 0);
    assertEQ(info.numSharps, 0);
    validate(info);

    // gmaj
    scale.set(MidiNote(MidiNote::C + 7), Scale::Scales::Major);
    info = scale.getScoreInfo();
    assertEQ(info.numFlats, 0);
    assertEQ(info.numSharps, 1);
    validate(info);
}

static void testScore2() {
    Scale scale;

    // E flat is three flat major
    scale.set(MidiNote(MidiNote::C + 3), Scale::Scales::Major);

    auto info = scale.getScoreInfo();
    assertEQ(info.numFlats, 3);
    assertEQ(info.numSharps, 0);
    validate(info);

    // C Min is the relative minor
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Minor);
    info = scale.getScoreInfo();
    assertEQ(info.numFlats, 3);
    assertEQ(info.numSharps, 0);
    validate(info);
}

static void testSharpsFlatsDiatonic() {
    Scale scale;

    scale.set(MidiNote(MidiNote::C), Scale::Scales::Minor);
    auto info = scale.getSharpsFlatsPref();
    assert(info == SharpsFlatsPref::Flats);

    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == SharpsFlatsPref::DontCare);

    // C#/Dflat
    scale.set(MidiNote(MidiNote::C + 1), Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == SharpsFlatsPref::Flats);

    scale.set(MidiNote(MidiNote::D), Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == SharpsFlatsPref::Sharps);

    // D#/Eflat
    scale.set(MidiNote(MidiNote::D + 1), Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == SharpsFlatsPref::Flats);

    scale.set(MidiNote(MidiNote::E), Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == SharpsFlatsPref::Sharps);

    scale.set(MidiNote(MidiNote::F), Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == SharpsFlatsPref::Flats);

    // F#/G-
    scale.set(MidiNote(MidiNote::F + 1), Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == SharpsFlatsPref::DontCare);

    scale.set(MidiNote(MidiNote::G), Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == SharpsFlatsPref::Sharps);

    // G#/A-
    scale.set(MidiNote(MidiNote::G + 1), Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == SharpsFlatsPref::Flats);

    scale.set(MidiNote(MidiNote::A), Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == SharpsFlatsPref::Sharps);

    // A#/B-
    scale.set(MidiNote(MidiNote::A + 1), Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == SharpsFlatsPref::Flats);

    scale.set(MidiNote(MidiNote::B), Scale::Scales::Major);
    info = scale.getSharpsFlatsPref();
    assert(info == SharpsFlatsPref::Sharps);
}

static void testSharpsFlatsOtherMinor() {
    Scale scale;
    scale.set(MidiNote(MidiNote::E), Scale::Scales::MinorPentatonic);
    auto info = scale.getSharpsFlatsPref();
    assert(info == SharpsFlatsPref::Sharps);

    scale.set(MidiNote(MidiNote::E), Scale::Scales::HarmonicMinor);
    info = scale.getSharpsFlatsPref();
    assert(info == SharpsFlatsPref::Sharps);

    scale.set(MidiNote(MidiNote::E - 1), Scale::Scales::MinorPentatonic);
    info = scale.getSharpsFlatsPref();
    assert(info == SharpsFlatsPref::DontCare);

    scale.set(MidiNote(MidiNote::E - 1), Scale::Scales::HarmonicMinor);
    info = scale.getSharpsFlatsPref();
    assert(info == SharpsFlatsPref::DontCare);

    scale.set(MidiNote(MidiNote::G), Scale::Scales::MinorPentatonic);
    info = scale.getSharpsFlatsPref();
    assert(info == SharpsFlatsPref::Flats);

    scale.set(MidiNote(MidiNote::G), Scale::Scales::HarmonicMinor);
    info = scale.getSharpsFlatsPref();
    assert(info == SharpsFlatsPref::Flats);
}

static void testSharpsFlatsWierdos() {
    Scale scale;
    int count = 0;
    for (int i = MidiNote::C; i <= MidiNote::B; ++i) {
        scale.set(MidiNote(i), Scale::Scales::Chromatic);
        auto info = scale.getSharpsFlatsPref();
        assert(info == SharpsFlatsPref::DontCare);

        scale.set(MidiNote(i), Scale::Scales::Diminished);
        info = scale.getSharpsFlatsPref();
        assert(info == SharpsFlatsPref::DontCare);

        scale.set(MidiNote(i), Scale::Scales::DominantDiminished);
        info = scale.getSharpsFlatsPref();
        assert(info == SharpsFlatsPref::DontCare);

        scale.set(MidiNote(i), Scale::Scales::WholeStep);
        info = scale.getSharpsFlatsPref();
        assert(info == SharpsFlatsPref::Sharps);
        ++count;
    }
    assertEQ(count, 12);
}

static void testSharpsFlatsNoAssert() {
    Scale scale;
    int count = 0;
    for (int i = int(Scale::Scales::Major); i <= int(Scale::Scales::Chromatic); ++i) {
        scale.set(MidiNote(0), Scale::Scales(i));
        auto info = scale.getSharpsFlatsPref();
        ++count;
    }
    assertGE(count, 12);
}

static void testScore3() {
    Scale scale;

    // all the natural modes
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);
    validate(scale.getScoreInfo(), 0, 0);
    scale.set(MidiNote(MidiNote::D), Scale::Scales::Dorian);
    validate(scale.getScoreInfo(), 0, 0);
    scale.set(MidiNote(MidiNote::E), Scale::Scales::Phrygian);
    validate(scale.getScoreInfo(), 0, 0);
    scale.set(MidiNote(MidiNote::F), Scale::Scales::Lydian);
    validate(scale.getScoreInfo(), 0, 0);
    scale.set(MidiNote(MidiNote::G), Scale::Scales::Mixolydian);
    validate(scale.getScoreInfo(), 0, 0);
    scale.set(MidiNote(MidiNote::A), Scale::Scales::Minor);
    validate(scale.getScoreInfo(), 0, 0);
    scale.set(MidiNote(MidiNote::B), Scale::Scales::Locrian);
    validate(scale.getScoreInfo(), 0, 0);

    // all the one sharp scales
    scale.set(MidiNote(MidiNote::G), Scale::Scales::Major);
    validate(scale.getScoreInfo(), 1, 0);
    scale.set(MidiNote(MidiNote::A), Scale::Scales::Dorian);
    validate(scale.getScoreInfo(), 1, 0);
    scale.set(MidiNote(MidiNote::B), Scale::Scales::Phrygian);
    validate(scale.getScoreInfo(), 1, 0);
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Lydian);
    validate(scale.getScoreInfo(), 1, 0);
    scale.set(MidiNote(MidiNote::D), Scale::Scales::Mixolydian);
    validate(scale.getScoreInfo(), 1, 0);
    scale.set(MidiNote(MidiNote::E), Scale::Scales::Minor);
    validate(scale.getScoreInfo(), 1, 0);
    scale.set(MidiNote(MidiNote::F + 1), Scale::Scales::Locrian);
    validate(scale.getScoreInfo(), 1, 0);

    // all the two sharp scales
    scale.set(MidiNote(MidiNote::D), Scale::Scales::Major);
    validate(scale.getScoreInfo(), 2, 0);
    scale.set(MidiNote(MidiNote::E), Scale::Scales::Dorian);
    validate(scale.getScoreInfo(), 2, 0);
    scale.set(MidiNote(MidiNote::F + 1), Scale::Scales::Phrygian);
    validate(scale.getScoreInfo(), 2, 0);
    scale.set(MidiNote(MidiNote::G), Scale::Scales::Lydian);
    validate(scale.getScoreInfo(), 2, 0);
    scale.set(MidiNote(MidiNote::A), Scale::Scales::Mixolydian);
    validate(scale.getScoreInfo(), 2, 0);
    scale.set(MidiNote(MidiNote::B), Scale::Scales::Minor);
    validate(scale.getScoreInfo(), 2, 0);
    scale.set(MidiNote(MidiNote::C + 1), Scale::Scales::Locrian);
    validate(scale.getScoreInfo(), 2, 0);

    scale.set(MidiNote(MidiNote::A), Scale::Scales::Major);
    validate(scale.getScoreInfo(), 3, 0);

    scale.set(MidiNote(MidiNote::E), Scale::Scales::Major);
    validate(scale.getScoreInfo(), 4, 0);

    // b maj = 5 sharp or 7 flats
    scale.set(MidiNote(MidiNote::B), Scale::Scales::Major);
    validate(scale.getScoreInfo(), 5, 7);

    scale.set(MidiNote(MidiNote::F + 1), Scale::Scales::Major);
    validate(scale.getScoreInfo(), 6, 6);

    // c# major / d- major
    scale.set(MidiNote(MidiNote::C + 1), Scale::Scales::Major);
    validate(scale.getScoreInfo(), 7, 5);

    // one flat
    scale.set(MidiNote(MidiNote::F), Scale::Scales::Major);
    validate(scale.getScoreInfo(), 0, 1);

    // 2 flat b- major
    scale.set(MidiNote(MidiNote::B - 1), Scale::Scales::Major);
    validate(scale.getScoreInfo(), 0, 2);

    // 3 flat e- maj
    scale.set(MidiNote(MidiNote::E - 1), Scale::Scales::Major);
    validate(scale.getScoreInfo(), 0, 3);

    // 4 flats A- major = g# major
    scale.set(MidiNote(MidiNote::A - 1), Scale::Scales::Major);
    validate(scale.getScoreInfo(), 0, 4);

    //  5 flats d- major . also c# major - 7 sharps
    scale.set(MidiNote(MidiNote::D - 1), Scale::Scales::Major);
    validate(scale.getScoreInfo(), 7, 5);

    // 6 flats g- major, algo f# mar
    scale.set(MidiNote(MidiNote::G - 1), Scale::Scales::Major);
    validate(scale.getScoreInfo(), 6, 6);

    // 7 flats c- major. also bmaj = 5 sharps
    scale.set(MidiNote(MidiNote::C + 12 - 1), Scale::Scales::Major);
    validate(scale.getScoreInfo(), 5, 7);
}

static void testScaleNumbers() {
    assert(Scale::numScalesTotal() == (1 + int(Scale::lastScale)));
    assertEQ(Scale::numScales(true), Scale::numDiatonicScales());
    assertEQ(Scale::numScales(false), Scale::numScalesTotal());
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

    assertEQ(Scale::getShortScaleLabels(false).size(), Scale::lastScale + 1);

    // TODO: these hard coded number are bad!
    assertEQ(Scale::getShortScaleLabels(true).size(), 7);  // 7 diatonic
    assertEQ(Scale::getShortScaleLabels(false).size(), Scale::lastScale + 1);
}

static void testNumNotes() {
    assertEQ(Scale::numNotesInScale(Scale::Scales::Major), 7);
    assertEQ(Scale::numNotesInScale(Scale::Scales::Dorian), 7);
    assertEQ(Scale::numNotesInScale(Scale::Scales::Phrygian), 7);
    assertEQ(Scale::numNotesInScale(Scale::Scales::Lydian), 7);
    assertEQ(Scale::numNotesInScale(Scale::Scales::Mixolydian), 7);
    assertEQ(Scale::numNotesInScale(Scale::Scales::Minor), 7);
    assertEQ(Scale::numNotesInScale(Scale::Scales::Locrian), 7);

    assertEQ(Scale::numNotesInScale(Scale::Scales::MinorPentatonic), 5);
    assertEQ(Scale::numNotesInScale(Scale::Scales::MajorPentatonic), 5);
    assertEQ(Scale::numNotesInScale(Scale::Scales::HarmonicMinor), 7);
    assertEQ(Scale::numNotesInScale(Scale::Scales::Chromatic), 12);

    assertEQ(Scale::numNotesInScale(Scale::Scales::Diminished), 8);
    assertEQ(Scale::numNotesInScale(Scale::Scales::DominantDiminished), 8);
    assertEQ(Scale::numNotesInScale(Scale::Scales::WholeStep), 6);

    assertEQ(Scale::lastScale + 1, 14);
}

static void testGetNotesInScale() {
    for (int i = 0; i <= Scale::lastScale; ++i) {
        Scale scale;
        scale.set(MidiNote(MidiNote::C), Scale::Scales(i));
        const int* pitches = scale._getNormalizedScalePitches();
        int len = 0;
        while (*pitches >= 0) {
            ++pitches;
            ++len;
        }
        assertGE(len, 5);
    }
}

static void testConvertEmpty() {
    // bogus case
    Scale::Role roles[] = {Scale::Role::End};
    const auto x = Scale::convert(roles, false);
    assertEQ(std::get<0>(x), false);
}

Scale::Role* getRolesCMajor() {
    static Scale::Role roles[] = {
        Scale::Role::Root,  // C
        Scale::Role::NotInScale,
        Scale::Role::InScale,  // D
        Scale::Role::NotInScale,
        Scale::Role::InScale,  // E
        Scale::Role::InScale,  // F
        Scale::Role::NotInScale,
        Scale::Role::InScale,  // G
        Scale::Role::NotInScale,
        Scale::Role::InScale,  // A
        Scale::Role::NotInScale,
        Scale::Role::InScale,  // B
        Scale::Role::End};
    return roles;
}

Scale::Role* getRolesDMajor() {
    static Scale::Role roles[] = {
        Scale::Role::NotInScale,  // C
        Scale::Role::InScale,     // C#
        Scale::Role::Root,        // D
        Scale::Role::NotInScale,
        Scale::Role::InScale,     // E
        Scale::Role::NotInScale,  // F
        Scale::Role::InScale,     // F#
        Scale::Role::InScale,     // G
        Scale::Role::NotInScale,
        Scale::Role::InScale,  // A
        Scale::Role::NotInScale,
        Scale::Role::InScale,  // B
        Scale::Role::End};
    return roles;
}

static void testRolesCMajor() {
    Scale sc;
    const Scale::RoleArray ar = Scale::convert(MidiNote(MidiNote::C), Scale::Scales::Major);
    Scale::Role* expected = getRolesCMajor();

    //  Scale::_dumpRoles("expected", expected);
    //   Scale::_dumpRoles("under test", ar.data);

    for (int i = 0; ar.data[i] != Scale::Role::End; ++i) {
        assert(ar.data[i] == expected[i]);
    }
}

static void testRolesDMajor() {
    Scale sc;
    const Scale::RoleArray ar = Scale::convert(MidiNote(MidiNote::D), Scale::Scales::Major);
    Scale::Role* expected = getRolesDMajor();

    //  Scale::_dumpRoles("expected", expected);
    //   Scale::_dumpRoles("under test", ar.data);

    for (int i = 0; ar.data[i] != Scale::Role::End; ++i) {
        assert(ar.data[i] == expected[i]);
    }
}

static void validate(const Scale::RoleArray& roles) {
    assert(roles.data[12] == Scale::Role::End);
    int numRoots = 0;
    for (int i = 0; i < 12; ++i) {
        if (roles.data[i] == Scale::Role::Root) {
            ++numRoots;
        }
    }
    assertEQ(numRoots, 1);
}

static void testConvertFrom(const Scale::Role* expectedRoles, MidiNote testRoot, Scale::Scales testMode) {
    auto result = Scale::convert(testRoot, testMode);
    validate(result);
    //  Scale::_dumpRoles("expected:", expectedRoles);
    //  Scale::_dumpRoles("actual:", result.data);
    int len = 0;
    for (bool done = false; !done;) {
        assert(expectedRoles[len] == result.data[len]);
        if (expectedRoles[len] == Scale::Role::End) {
            done = true;
            len--;
        }
        ++len;
    }
    assertEQ(len, 12);
}

static void testConvertFromCMajor() {
    const auto roles = getRolesCMajor();
    testConvertFrom(roles, MidiNote(MidiNote::C), Scale::Scales::Major);
}

static void testConvertFromDMajor() {
    const auto roles = getRolesDMajor();
    testConvertFrom(roles, MidiNote(MidiNote::D), Scale::Scales::Major);
}

static void testConvertCMajor() {
    const auto roles = getRolesCMajor();
    //   Scale::_dumpRoles("cmajor", roles);
    const auto x = Scale::convert(roles, false);
    assertEQ(std::get<0>(x), true);
    assert(std::get<1>(x) == MidiNote(MidiNote::C));
    assert(std::get<2>(x) == Scale::Scales::Major);
}

static void testConvertDMajor() {
    const auto roles = getRolesDMajor();
    const auto x = Scale::convert(roles, false);
    assertEQ(std::get<0>(x), true);
    assert(std::get<1>(x) == MidiNote(MidiNote::D));
    assert(std::get<2>(x) == Scale::Scales::Major);
}

static Scale::Role* getRolesCMinor() {
    static Scale::Role roles[] = {
        Scale::Role::Root,  // C
        Scale::Role::NotInScale,
        Scale::Role::InScale,     // D
        Scale::Role::InScale,     // E-
        Scale::Role::NotInScale,  // E
        Scale::Role::InScale,     // F
        Scale::Role::NotInScale,  // G-
        Scale::Role::InScale,     // G
        Scale::Role::InScale,     // A-
        Scale::Role::NotInScale,  // A
        Scale::Role::InScale,     // B-
        Scale::Role::NotInScale,  // B
        Scale::Role::End};
    return roles;
}

static void testConvertCMinor() {
    const auto roles = getRolesCMinor();
    const auto x = Scale::convert(roles, false);
    assertEQ(std::get<0>(x), true);
    assert(std::get<1>(x) == MidiNote(MidiNote::C));
    assert(std::get<2>(x) == Scale::Scales::Minor);
}

static void testRoundTrip(Scale scale, bool diatonicOnly) {
    //  assert(!diatonicOnly);
    // SQINFO("will get roles");
    const auto scaleParts = scale.get();

    const bool scaleIsDiatonic = int(std::get<1>(scaleParts)) < Scale::numDiatonicScales();

    const Scale::RoleArray roleArray = scale.convert(std::get<0>(scaleParts), std::get<1>(scaleParts));

    // SQINFO("will convert back from roles");
    const auto x = Scale::convert(roleArray.data, diatonicOnly);
    const Scale::Scales foundMode = std::get<2>(x);

    // must be found
    const bool expectFound = !diatonicOnly || scaleIsDiatonic;
    assertEQ(std::get<0>(x), expectFound);
    // must find the right mode
    if (expectFound) {
        assert(foundMode == std::get<1>(scaleParts));
        assertEQ(std::get<1>(x).get(), std::get<0>(scaleParts).get());
    }
}

static void testRoundTrip(bool diatonicOnly) {
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);
    testRoundTrip(scale, diatonicOnly);

    scale.set(MidiNote(MidiNote::C), Scale::Scales::Chromatic);
    testRoundTrip(scale, diatonicOnly);
    scale.set(MidiNote(MidiNote::C + 1), Scale::Scales::Chromatic);
    testRoundTrip(scale, diatonicOnly);
    scale.set(MidiNote(MidiNote::C + 1), Scale::Scales::Major);
    testRoundTrip(scale, diatonicOnly);

    for (int mode = Scale::firstScale; mode <= Scale::lastScale; ++mode) {
        const auto smode = Scale::Scales(mode);
        for (int pitch = MidiNote::C; pitch <= MidiNote::B; ++pitch) {
            scale.set(MidiNote(pitch), smode);
            testRoundTrip(scale, diatonicOnly);
        }
    }
}

static void testRoundTrip() {
    testRoundTrip(false);
    testRoundTrip(true);
}

static void testConvert() {
    testRolesCMajor();
    testConvertFromCMajor();
    testRolesDMajor();
    testConvertFromDMajor();
    testConvertEmpty();
    testConvertCMajor();
    testConvertDMajor();
    testConvertCMinor();
    testRoundTrip();
}

static void testMultiRoot() {
    // get C major, but make every note a root
    Scale::Role* test = getRolesCMajor();
    for (int i = 0; test[i] != Scale::Role::End; ++i) {
        if (test[i] == Scale::Role::InScale) {
            test[i] = Scale::Role::Root;
        }
    }
    // Scale::_dumpRoles("after patch", test);
    const auto converted = Scale::convert(test, false);
    assertEQ(std::get<0>(converted), false);
}

static void testChromatic2() {
    Scale sc;
    sc.set(MidiNote(MidiNote::C), Scale::Scales::Chromatic);
    const int* p = sc._getNormalizedScalePitches();
    int last = -1;
    int len = 0;
    for (int i = 0; p[i] >= 0; ++i) {
        // SQINFO("p[%d] = %d", i, p[i]);
        assertEQ(p[i], (last + 1));
        last = p[i];
        ++len;
    }
    assertEQ(len, 12);
}

static void testCtor() {
    Scale scale(MidiNote(MidiNote::F), Scale::Scales::Minor);
    const auto x = scale.get();
    assertEQ(x.first.get(), MidiNote::F);
    assert(x.second == Scale::Scales::Minor);

    scale.degreeToSemitone(0);          // would assert if not prop init.
}

void testScale() {
    testCtor();
    testScaleNumbers();
    testLabels();
    testNumNotes();
    testGetNotesInScale();
    testCMaj();
    testAMin();
    testGeneral();
    testChromatic();
    testChromatic2();

    testQuantCMaj();
    testQuantCMin();
    testQuantEPenta();

    testm2sCMajC3();
    testm2sCMajC4();
    testm2sCMajCsharp4();
    //  testm2sDSharp();

    tests2mCMajC4();
    tests2mCMajC5();

    testScore();
    testScore2();
    testScore3();

    testSharpsFlatsDiatonic();
    testSharpsFlatsOtherMinor();
    testSharpsFlatsWierdos();
    testSharpsFlatsNoAssert();

    testConvert();
    // SQINFO("Make multi root work");
    testMultiRoot();
}

#if 0
void testFirst() {
    testCtor();
}
#endif