
#include "FloatNote.h"
#include "NoteConvert.h"
#include "Scale.h"
#include "asserts.h"

static void testm2sRoundTrip() {
    Scale scale;
    scale.set(MidiNote(4), Scale::Scales::Major);
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
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);

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
    scale.set(MidiNote(MidiNote::C + 4), Scale::Scales::Major);

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
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);

    const FloatNote fn(1 + (1.f / 12.f));  // c sharp
    ScaleNote sn;
    NoteConvert::f2s(sn, scale, fn);
    assertEQ(sn.getDegree(), 0);
    assert(sn.isAccidental());
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::sharp);
    assertEQ(sn.getOctave(), 5);  // c4 is defaulat
}

static void tests2fCMajC4() {
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);

    ScaleNote sn(0, 4);  // C4

    FloatNote fn;

    NoteConvert::s2f(fn, scale, sn);
    assertEQ(fn.get(), 0);
}

static void testCMinorValidInScale() {
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Minor);

    MidiNote mn(MidiNote::MiddleC);
    auto sn = scale.m2s(mn);
    assertEQ(sn.getDegree(), 0);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::none);
    assertEQ(sn.getOctave(), 4);

    mn = MidiNote(MidiNote::MiddleC + MidiNote::D);
    sn = scale.m2s(mn);
    assertEQ(sn.getDegree(), 1);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::none);

    mn = MidiNote(MidiNote::MiddleC + MidiNote::E - 1);
    sn = scale.m2s(mn);
    assertEQ(sn.getDegree(), 2);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::none);

    mn = MidiNote(MidiNote::MiddleC + MidiNote::F);
    sn = scale.m2s(mn);
    assertEQ(sn.getDegree(), 3);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::none);

    mn = MidiNote(MidiNote::MiddleC + MidiNote::F);
    sn = scale.m2s(mn);
    assertEQ(sn.getDegree(), 3);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::none);

    mn = MidiNote(MidiNote::MiddleC + MidiNote::G);
    sn = scale.m2s(mn);
    assertEQ(sn.getDegree(), 4);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::none);

    mn = MidiNote(MidiNote::MiddleC + MidiNote::A - 1);
    sn = scale.m2s(mn);
    assertEQ(sn.getDegree(), 5);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::none);

    mn = MidiNote(MidiNote::MiddleC + MidiNote::B - 1);
    sn = scale.m2s(mn);
    assertEQ(sn.getDegree(), 6);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::none);
    assertEQ(sn.getOctave(), 4);

    mn = MidiNote(MidiNote::MiddleC + 12);
    sn = scale.m2s(mn);
    assertEQ(sn.getDegree(), 0);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::none);
    assertEQ(sn.getOctave(), 5);
}

static void testCMinorValidNotInScale() {
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Minor);

    // C# / D- should go to D flat.
    MidiNote mn2(MidiNote::MiddleC + 1);
    auto sn = scale.m2s(mn2);
    assertEQ(sn.getDegree(), 1);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::flat);
    assertEQ(sn.getOctave(), 4);

    SQINFO("finish me");
}

static void testDMajorValidInScale() {
    Scale scale;
    scale.set(MidiNote(MidiNote::D), Scale::Scales::Major);

    MidiNote mn(MidiNote::MiddleC + MidiNote::D);
    auto sn = scale.m2s(mn);
    assertEQ(sn.getDegree(), 0);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::none);
    assertEQ(sn.getOctave(), 4);

    mn = MidiNote(MidiNote::MiddleC + MidiNote::E);
    sn = scale.m2s(mn);
    assertEQ(sn.getDegree(), 1);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::none);

    mn = MidiNote(MidiNote::MiddleC + MidiNote::F + 1);
    sn = scale.m2s(mn);
    assertEQ(sn.getDegree(), 2);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::none);

    mn = MidiNote(MidiNote::MiddleC + MidiNote::G);
    sn = scale.m2s(mn);
    assertEQ(sn.getDegree(), 3);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::none);

    mn = MidiNote(MidiNote::MiddleC + MidiNote::A);
    sn = scale.m2s(mn);
    assertEQ(sn.getDegree(), 4);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::none);

    mn = MidiNote(MidiNote::MiddleC + MidiNote::B);
    sn = scale.m2s(mn);
    assertEQ(sn.getDegree(), 5);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::none);

    mn = MidiNote(MidiNote::MiddleC + MidiNote::C + 1);
    sn = scale.m2s(mn);
    assertEQ(sn.getDegree(), 6);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::none);

    mn = MidiNote(MidiNote::MiddleC + MidiNote::D + 12);
    sn = scale.m2s(mn);
    assertEQ(sn.getDegree(), 0);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::none);
    assertEQ(sn.getOctave(), 5);
}

static void testDMajorValidNotInScale() {
    Scale scale;
    scale.set(MidiNote(MidiNote::D), Scale::Scales::Major);

    // D# / E- should go to D#.
    MidiNote mn(MidiNote::MiddleC + MidiNote::D + 1);
    auto sn = scale.m2s(mn);
    assertEQ(sn.getDegree(), 0);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::sharp);
    assertEQ(sn.getOctave(), 4);

    // F - not scale -> sharp of degree 1
    mn = MidiNote(MidiNote::MiddleC + MidiNote::F);
    sn = scale.m2s(mn);
    assertEQ(sn.getDegree(), 1);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::sharp);

    // G#/A-
    mn = MidiNote(MidiNote::MiddleC + MidiNote::G + 1);
    sn = scale.m2s(mn);
    assertEQ(sn.getDegree(), 3);
    assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::sharp);

    SQINFO("finish me 203");
}

static void TestCMinor47() {
     Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Minor);

      MidiNote mn(47);
     auto sn = scale.m2s(mn);
    assertEQ(sn.getDegree(), 0);
  //  assert(sn.getAdjustment() == ScaleNote::RelativeAdjustment::sharp);
  //  assertEQ(sn.getOctave(), 4);
    assertEQ(scale._validateScaleNote(sn), true);
}

void testScaleNotes() {
    testf2sCMajC4();
    testf2sCMajCsharp5();
    tests2fCMajC4();
    testm2sRoundTrip();
    testf2sRoundTrip();
    testCMinorValidInScale();
    testCMinorValidNotInScale();
    testDMajorValidInScale();
    testDMajorValidNotInScale();
    TestCMinor47();
}

#if 0
void testFirst() {
    //  testCMinorValidInScale();
    //  testCMinorValid2();
    // testDMajorValidNotInScale();
    TestCMinor47();
}
#endif
