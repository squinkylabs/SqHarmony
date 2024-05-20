
#include "asserts.h"

static void test0() {
    std::mt19937 gen32;
    std::mt19937 gen32b;
    gen32.discard(10000);
    for (int i = 0; i < 10; ++i) {
        const int x = gen32();
        const int y = gen32b();
       // SQINFO("rand[%d] = %x %x", i, x, y);
    }
}

void testTwister() {
    test0();
    // assert(false);
}