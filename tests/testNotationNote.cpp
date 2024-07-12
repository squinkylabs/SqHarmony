#include "NotationNote.h"
#include "asserts.h"

static void test0() {
    MidiNote mn(MidiNote::C3);
    NotationNote nn;
    nn = NotationNote();
    nn = NotationNote(mn, NotationNote::Accidental::none, -2);
}

static void testToString() {
    MidiNote mn(MidiNote::C3);
    NotationNote nn;
    nn = NotationNote();
    assertGT(nn.toString().length(), 0);
    nn = NotationNote(mn, NotationNote::Accidental::none, -2);

    assertGT(nn.toString().length(), 0);
}

static void testEqual() {
    MidiNote mn(MidiNote::C3);
    MidiNote mn2(MidiNote::C3);
    MidiNote mn3(MidiNote::C3 - 1);
    assert(NotationNote(mn, NotationNote::Accidental::none, 5) == NotationNote(mn2, NotationNote::Accidental::none, 5));
    assert(NotationNote(mn, NotationNote::Accidental::none, 5) != NotationNote(mn2, NotationNote::Accidental::none, 6));
    assert(NotationNote(mn, NotationNote::Accidental::none, 5) != NotationNote(mn2, NotationNote::Accidental::natural, 5));
    assert(NotationNote(mn, NotationNote::Accidental::none, 5) != NotationNote(mn3, NotationNote::Accidental::none, 5));
}

static void testIsAccidental() {
    MidiNote mn(MidiNote::C3);
    assertEQ(NotationNote(mn, NotationNote::Accidental::none, 5).isAccidental(), false);
    assertEQ(NotationNote(mn, NotationNote::Accidental::natural, 5).isAccidental(), true);
    assertEQ(NotationNote(mn, NotationNote::Accidental::sharp, 5).isAccidental(), true);
    assertEQ(NotationNote(mn, NotationNote::Accidental::flat, 5).isAccidental(), true);
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