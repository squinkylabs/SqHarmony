

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
    int x[] = {60, 60 + 4, 60 + 7, 60 + 12};
    Chord4* chord = new Chord4(options, 1, x, false);
    assertEQ(chord->fetchRoot(), 1);
    delete chord;
}

static void testRootSet() {
 const Options options = makeOptions(false);
    int x[] = {60 -12 + 5, 60 + 5 + 4, 60 + 5 + 7, 60 + 5 + 12};
    Chord4* chord = new Chord4(options, 4, x, false);
    assertEQ(chord->fetchRoot(), 4);
    delete chord;
}
#endif



static void testCanCreateList() {
    const Options options = makeOptions(false);
    //   Chord4List(const Options& options, int root, bool show = false);
    for (int root = 1; root < 8; ++root) {
        Chord4List list(options, root, false);
        assertGT(list.size(), 0);
        SQINFO("root %d made %d", root, list.size());
        SQINFO("first chord = %s ", list.get2(0)->toStringShort().c_str());
    }
}

void testChord4() {
    testCanCreate();
    testRootSet();
#if _CHORD4_USE_NEW == true
    testCanCreate2();
#endif

    testCanCreateList();
}