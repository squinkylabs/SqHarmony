
#include "Chord4Manager.h"
#include "KeysigOld.h"

#include "asserts.h"

static StylePtr makeStyle() {
    return std::make_shared<Style>();
}

static KeysigOldPtr makeKeysig(bool minor) {
    auto ret =  std::make_shared<KeysigOld>(Roots::C);
    if (minor) {
        ret->set(MidiNote::C, Scale::Scales::Minor);
    }
    return ret;
}

static Options makeOptions(bool minor=false) {
    Options o(makeKeysig(minor), makeStyle());
    return o;
}

static int getNumChords(int root) {
    auto o = makeOptions();
    Chord4Manager m(o);
    int num = 0;
    for (bool bDone = false; !bDone; ) {
        auto p = m.get2(root, num);
        if (p) {
            ++num;
        } else {
            bDone = true;
        }
    }
    return num;
}

static void test1() {
    for (int root = 1; root < 8; ++root) {
        assertGT(getNumChords(root), 10);
    }
}

static void validateRank(int root) {
    auto o = makeOptions();
    Chord4Manager m(o);
    int num = 0;
    for (bool bDone = false; !bDone; ) {
        auto p = m.get2(root, num);
        if (p) {
            assertEQ(p->rank, num)
            ++num;
        }
        else {
            bDone = true;
        }
    }
}

static void test2() {
    for (int root = 1; root < 8; ++root) {
        validateRank(root);
    }
}



void testChord4Manager() {
    test1();
    test2();
}