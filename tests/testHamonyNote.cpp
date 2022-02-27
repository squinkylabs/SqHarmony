
#include "HarmonyNote.h"
#include "Keysig.h"
#include "asserts.h"

static StylePtr makeStyle() {
    return std::make_shared<Style>();
}

static KeysigPtr makeKeysig() {
    return std::make_shared<Keysig>(Roots::C);
}

static Options makeOptions() {
    Options o(makeKeysig(), makeStyle());
    return o;
}

static void test0() {
    HarmonyNote n(makeOptions());
}

static void test1() {
    auto o = makeOptions();
    assert(o.style->absMinPitch() > 0);
    HarmonyNote n(o);
    n.setMin(o);
    assert(n == o.style->absMinPitch());
}

static void test2() {
    auto o = makeOptions();
    HarmonyNote n(o);
    assert(!n.isTooHigh(o));
    for (int i = 0; i < 127; ++i) {
        ++n;
    }
    assert(n.isTooHigh(o));
}

static void test3() {
    auto o = makeOptions();
    HarmonyNote n(o);
    assert(n == o.style->absMinPitch());
}

static void test4() {
    auto o = makeOptions();
    HarmonyNote n(o);

    auto name = n.tellPitchName();
    assert(!name.empty());
}

static void testAbsPitch() {
    auto o = makeOptions();
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

void testHarmonyNote() {
    test0();
    test1();
    test2();
    test3();
    test4();

    testAbsPitch();
}
