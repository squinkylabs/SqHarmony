

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

#if _CHORD4_USE_NEW == true
static void testCanCreate2() {
   const Options options = makeOptions(false);
   int x[] = { 60, 60 + 4, 60 + 7, 60 + 12 };
    Chord4* chord = new Chord4(options, 1, x, false);
    delete chord;
}
#endif

static void testCanCreateList() {
     const Options options = makeOptions(false);
    //   Chord4List(const Options& options, int root, bool show = false);
    Chord4List list(options, 1, false);
    assertGT(list.size(), 0);

}

void testChord4() {
    testCanCreate();
#if _CHORD4_USE_NEW == true
    testCanCreate2();
#endif
    testCanCreateList();
   
}