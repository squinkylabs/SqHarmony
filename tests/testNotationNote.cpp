#include "asserts.h"

#include "NotationNote.h"

static void test0() {

    MidiNote mn(MidiNote::C3);
    NotationNote nn;
    nn = NotationNote();
    nn = NotationNote(mn, NotationNote::Accidental::none, -2);
}

static void testReSpellCMajorCSharp() {
    MidiNote mn(MidiNote::C3 + 1);
    NotationNote nn = NotationNote(mn, NotationNote::Accidental::sharp, -2);
    bool b = nn.reSpell(false);
    assert(b);
}

void testNotationNote() {
    test0();
    testReSpellCMajorCSharp();
}


#if 1
void testFirst() {
    testReSpellCMajorCSharp();
}
#endif