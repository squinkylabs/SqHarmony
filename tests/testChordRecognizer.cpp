
#include "ChordRecognizer.h"

#include "asserts.h"

static void testCanCreate() {
    ChordRecognizer ch;
    const int chord[] = { 1,2,3,-1 };
    ch.recognize(chord);
}

void testChordRecognizer() {
    testCanCreate();
}