#include "HarmonySong.h"
#include "KeysigOld.h"
#include "Options.h"
#include "Style.h"


static StylePtr makeStyle() {
    return std::make_shared<Style>();
}

static KeysigOldPtr makeKeysig() {
    return std::make_shared<KeysigOld>(Roots::C);
}

static Options makeOptions() {
    Options o(makeKeysig(), makeStyle());
    return o;
}

static void test0() {
    auto o = makeOptions();
    int progression[] = { 1,4,5, 0 };
    HarmonySong s(o, progression);
    assert(s.size() == 3);
    printf("\nnow will print song:\n");
    s.print();
}

static void testGenerate() {
    auto o = makeOptions();
    int progression[] = { 1,4,5, 0 };
    HarmonySong s(o, progression);
    s.Generate(o, 0, false);
    assert(s.size() == 3);
    printf("\nnow will print song:\n");
    s.print();
}

void  testHarmonySong() {
    test0();
    testGenerate();
}