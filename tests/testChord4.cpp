

#include "Chord4.h"
#include "Chord4List.h"
#include "KeysigOld.h"
#include "asserts.h"

static StylePtr makeStyle() {
    auto style = std::make_shared<Style>();
    style->setRangesPreference(Style::Ranges::WIDE_RANGE);
    return style;
}

static KeysigOldPtr makeKeysig(bool minor) {
    auto ret = std::make_shared<KeysigOld>(Roots::C);
    if (minor) {
        ret->set(MidiNote(MidiNote::C), Scale::Scales::Minor);
    }
    return ret;
}

static Options makeOptions(bool minor) {
    Options o(makeKeysig(minor), makeStyle());
    return o;
}

static void testCanCreate() {
    Chord4* chord = new Chord4();
    delete chord;
}


static void testCanCreate2() {
    const Options options = makeOptions(false);
    Chord4* chord = new Chord4(options, 1, false);
    assertEQ(chord->fetchRoot(), 1);
    delete chord;
}

static void testRootSet() {
 const Options options = makeOptions(false);
  //  int x[] = {60 -12 + 5, 60 + 5 + 4, 60 + 5 + 7, 60 + 5 + 12};
    Chord4* chord = new Chord4(options, 4, false);
    assertEQ(chord->fetchRoot(), 4);
    delete chord;
}




static void testCanCreateList() {
    const Options options = makeOptions(false);
    for (int root = 1; root < 8; ++root) {
        Chord4List list(options, root, false);
        assertGT(list.size(), 0);
        // SQINFO("root %d made %d", root, list.size());
        // SQINFO("first chord = %s ", list.get2(0)->toStringShort().c_str());
    }
}

static void testRootCInversion() {
    // Root: normal root position double root, good
    const Options options = makeOptions(false);
    auto chord = Chord4::fromString(options, 1, "C2C3E3G3");
    assertEQ(chord->isCorrectDoubling(options), true);
}

static void testRootCInversion2() {
    // root: root position double third, ng
    const Options options = makeOptions(false);
    auto chord = Chord4::fromString(options, 1, "C2E2E3G3");
    assertEQ(chord->isCorrectDoubling(options), false);
}

static void testRootCInversion3() {
    // root: first inversion double root, GOOD
    const Options options = makeOptions(false);
    auto chord = Chord4::fromString(options, 1, "E2C3C4G4");
    assertEQ(chord->isCorrectDoubling(options), true);
}

static void testRootCInversion4() {
    // fourth: root position double root, good
    const Options options = makeOptions(false);
    auto chord = Chord4::fromString(options, 4, "F2A3F4C5");
    assert(chord);
    assertEQ(chord->isCorrectDoubling(options), true);
}

static void testRootCInversion5() {
    // fourth: root position double third, ng
    const Options options = makeOptions(false);
    auto chord = Chord4::fromString(options, 4, "F2A3C4A4");
    assert(chord);
    assertEQ(chord->isCorrectDoubling(options), false);
}

static void testRootCInversion6() {
    if (Style::doubleBass()) {
        return;     // this rule only for double root.
    }
    // fourth: root position double fifth, ok
    const Options options = makeOptions(false);
    auto chord = Chord4::fromString(options, 4, "F2C3C4A4");
    assert(chord);
    assertEQ(chord->isCorrectDoubling(options), true);
}

static void testRootCInversion7() {
    if (Style::doubleBass()) {
        return;     // this rule only for double root.
    }
    // second: root position double root, ng
    const Options options = makeOptions(false);
    auto chord = Chord4::fromString(options, 2, "D2F2A2D3");
    assert(chord);
    assertEQ(chord->isCorrectDoubling(options), false);
}

static void testRootCInversion8() {
    if (Style::doubleBass()) {
        return;     // this rule only for double root.
    }
    // second: root position double third, good
    const Options options = makeOptions(false);
    auto chord = Chord4::fromString(options, 2, "D2F2A2F3");
    assert(chord);
    assertEQ(chord->isCorrectDoubling(options), true);
}

static void testRootCInversion9() {
    // second: root position double fifth, ng
    const Options options = makeOptions(false);
    auto chord = Chord4::fromString(options, 2, "D2F2A2A3");
    assert(chord);
    assertEQ(chord->isCorrectDoubling(options), false);
}
void testChord4() {
    testCanCreate();
    testRootSet();
    testCanCreate2();
    testCanCreateList();
    testRootCInversion();
    testRootCInversion2();
    testRootCInversion3();
    testRootCInversion4();
    testRootCInversion5();
    testRootCInversion6();
    testRootCInversion7();
    testRootCInversion8();
    testRootCInversion9();
}