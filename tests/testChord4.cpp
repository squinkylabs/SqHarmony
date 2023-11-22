

#include "Chord4.h"
#include "Chord4List.h"
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

static void testCanCreate() {
    Chord4* chord = new Chord4();
    delete chord;
}

static void testCanCreate2() {
   const Options options = makeOptions(false);
   int x[] = { 72, 72 + 4, 72 + 7, 72 + 12 };
    Chord4* chord = new Chord4(options, 1, x, false);
    delete chord;
}

static void testCanCreateList() {
     const Options options = makeOptions(false);
    //   Chord4List(const Options& options, int root, bool show = false);
    Chord4List list(options, 1, false);

}

void testChord4() {
    testCanCreate();
    testCanCreate2();
    testCanCreateList();
}