#include "HarmonySong.h"
#include "KeysigOld.h"
#include "Options.h"
#include "Style.h"


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

static Options makeOptions(bool minor) {
    Options o(makeKeysig(minor), makeStyle());
    return o;
}

static void test0(bool minor) {
    auto o = makeOptions(minor);
    int progression[] = { 1,4,5, 0 };
    HarmonySong s(o, progression);
    assert(s.size() == 3);
  //  printf("\nnow will print song:\n");
  //  s.print();
}

static void test0() {
    test0(false);
    test0(true);
}

static void testGenerate(bool minor) {
    auto o = makeOptions(minor);
    int progression[] = { 1,4,5, 0 };
    HarmonySong s(o, progression);
    s.Generate(o, 0, false);
    assert(s.size() == 3);
  //  printf("\nnow will print song:\n");
  //  s.print();
}

static void testGenerate() {
    testGenerate(false);
    testGenerate(true);
}

void  testHarmonySong() {
    test0();
    testGenerate();
}