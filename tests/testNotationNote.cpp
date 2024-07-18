#include "NotationNote.h"
#include "asserts.h"

static void test0() {
    MidiNote mn(MidiNote::C3);
    NotationNote nn;
    nn = NotationNote();
    nn = NotationNote(mn, NotationNote::Accidental::none, -2, false);
}

static void testToString() {
    MidiNote mn(MidiNote::C3);
    NotationNote nn;
    nn = NotationNote();
    assertGT(nn.toString().length(), 0);
    nn = NotationNote(mn, NotationNote::Accidental::none, -2, false);

    assertGT(nn.toString().length(), 0);
}

static void testEqual() {
    MidiNote mn(MidiNote::C3);
    MidiNote mn2(MidiNote::C3);
    MidiNote mn3(MidiNote::C3 - 1);
    assert(NotationNote(mn, NotationNote::Accidental::none, 5, false) == NotationNote(mn2, NotationNote::Accidental::none, 5, false));
    assert(NotationNote(mn, NotationNote::Accidental::none, 5, false) != NotationNote(mn2, NotationNote::Accidental::none, 6, false));
    assert(NotationNote(mn, NotationNote::Accidental::none, 5, false) != NotationNote(mn2, NotationNote::Accidental::natural, 5, false));
    assert(NotationNote(mn, NotationNote::Accidental::none, 5, false) != NotationNote(mn3, NotationNote::Accidental::none, 5, false));
    assert(NotationNote(mn, NotationNote::Accidental::none, 5, false) != NotationNote(mn2, NotationNote::Accidental::none, 5, true));
}

static void testIsAccidental() {
    MidiNote mn(MidiNote::C3);
    assertEQ(NotationNote(mn, NotationNote::Accidental::none, 5, false).isAccidental(), false);
    assertEQ(NotationNote(mn, NotationNote::Accidental::natural, 5, false).isAccidental(), true);
    assertEQ(NotationNote(mn, NotationNote::Accidental::sharp, 5, false).isAccidental(), true);
    assertEQ(NotationNote(mn, NotationNote::Accidental::flat, 5, false).isAccidental(), true);
}

void testNotationNote() {
    test0();
    testToString();
    testEqual();
    testIsAccidental();
}

#if 0
void testFirst() {
    testNotationNote();
}
#endif