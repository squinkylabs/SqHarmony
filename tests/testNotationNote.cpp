#include "asserts.h"

#include "NotationNote.h"

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

void testNotationNote() {
    test0();
    testToString();
}


#if 0
void testFirst() {
    assert(false);
}
#endif