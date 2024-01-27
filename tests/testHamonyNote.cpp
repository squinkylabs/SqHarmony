
#include "HarmonyNote.h"
#include "KeysigOld.h"
#include "asserts.h"

static StylePtr makeStyle() {
    return std::make_shared<Style>();
}

static KeysigOldPtr makeKeysig(bool minor) {
    auto ret = std::make_shared<KeysigOld>(Roots::C);
    if (minor) {
        ret->set(MidiNote::C, Scale::Scales::Minor);
    }
    return ret;
}

static Options makeOptions(bool minor) {
    Options o(makeKeysig(minor), makeStyle());
    return o;
}

static void test0() {
    HarmonyNote n(makeOptions(false));
}

static void test1(bool minor) {
    auto o = makeOptions(minor);
    assert(o.style->absMinPitch() > 0);
    HarmonyNote n(o);
    n.setMin(o);
    assert(n == o.style->absMinPitch());
}

static void test1() {
    test1(false);
    test1(true);
}

static void test2(bool minor) {
    auto o = makeOptions(minor);
    HarmonyNote n(o);
    assert(!n.isTooHigh(o));
    for (int i = 0; i < 127; ++i) {
        ++n;
    }
    assert(n.isTooHigh(o));
}

static void test2() {
    test2(false);
    test2(true);
}

static void test3(bool minor) {
    auto o = makeOptions(minor);
    HarmonyNote n(o);
    assert(n == o.style->absMinPitch());
}

static void test3() {
    test3(false);
    test3(true);
}

static void test4(bool minor) {
    auto o = makeOptions(minor);
    HarmonyNote n(o);

    auto name = n.tellPitchName();
    assert(!name.empty());
}

static void test4() {
    test4(false);
    test4(true);
}

static void testAbsPitch(bool minor) {
    auto o = makeOptions(minor);
    HarmonyNote n(o);
    assertLT(n, HarmonyNote::C3);
    while (n < HarmonyNote::C3) {
        ++n;
    }

    assertEQ(n, HarmonyNote::C3);
    auto s = n.tellPitchName();
    assertEQ(s, "C3");

    ++n;
    s = n.tellPitchName();
    assertEQ(s, "C#3");

    ++n;
    s = n.tellPitchName();
    assertEQ(s, "D3");

    ++n;
    s = n.tellPitchName();
    assertEQ(s, "D#3");

    ++n;
    s = n.tellPitchName();
    assertEQ(s, "E3");

    ++n;
    s = n.tellPitchName();
    assertEQ(s, "F3");

    ++n;
    s = n.tellPitchName();
    assertEQ(s, "F#3");

    ++n;
    s = n.tellPitchName();
    assertEQ(s, "G3");

    ++n;
    s = n.tellPitchName();
    assertEQ(s, "G#3");

    ++n;
    s = n.tellPitchName();
    assertEQ(s, "A3");

    ++n;
    s = n.tellPitchName();
    assertEQ(s, "A#3");

    ++n;
    s = n.tellPitchName();
    assertEQ(s, "B3");

    ++n;
    s = n.tellPitchName();
    assertEQ(s, "C4");

#if 0
    while (n < (HarmonyNote::C3 + 12)) {
        ++n;
    }
    s = n.tellPitchName();
    assertEQ(s, "C4");
#endif
}

static void testAbsPitch() {
    testAbsPitch(false);
    testAbsPitch(true);
}

void testHarmonyNote() {
    test0();
    test1();
    test2();
    test3();
    test4();

    testAbsPitch();
}
