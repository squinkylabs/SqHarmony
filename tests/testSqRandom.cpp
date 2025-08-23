#include "SqRandom.hpp"
#include "asserts.h"

static void testSqRandom_random() {
    SqRandom _random(1234, 5678);

    int numAbove = 0;
    const int tries = 100;
    for (int i = 0; i < tries; ++i) {
        auto x = _random.generateDouble();
        // SQINFO("rand=%f", x);
        if (x > .5) {
            numAbove++;
        }
    }
    assertEQ(numAbove, tries / 2);  // this doesn't need to be exact... bit it's passing now...
}

static void testSqRandom_random2() {
    SqRandom _random(1234, 5678);

    int numAbove = 0;
    int numMax = 0;
    const int tries = 200;
    for (int i = 0; i < tries; ++i) {
        const int x = _random.generateInteger(123);
        // SQINFO("rand=%f", x);
        assert(x >= 0);
        assert(x < 123);
        if (x > 61) {
            numAbove++;
        }
        if (x == 122) {
            numMax++;
        }
    }
    assertClose(numAbove, tries / 2, 4);  // this doesn't need to be exact... bit it's passing now...
    assertEQ(numMax, 1);                  // we should have hit this.
}

static void testSqRandom_dist(int range) {
    SqRandom _random(1234, 5678);

    std::map<int, int> map;
    const int tries = 100000;  // Run this a long time, to get good ditribution
                               // was 10 ** 6

    for (int i = 0; i < tries; ++i) {
        const int x = _random.generateInteger(range);

        const auto iter = map.find(x);
        if (iter == map.end()) {
            map[x] = 1;
        } else {
            map[x]++;
        }
        assert(x >= 0);
        assert(x <= range);
    }

    const int expectedCount = tries / range;
    const int expectedDeviation = 700;

    assertLT(expectedDeviation, (expectedCount * 8) / 100);  // sanity check - we expect small deviation
    for (int i = 0; i < range; ++i) {
        // SQINFO("got %d expected %d diff=%d\n", map[i], expectedCount, std::abs(expectedCount - map[i]));
        assertClose(map[i], expectedCount, expectedDeviation);
    }
}

static void testSqRandom_random3() {
    testSqRandom_dist(10);
    testSqRandom_dist(3);
    testSqRandom_dist(1);
}

void testSqRandom() {
    testSqRandom_random();
    testSqRandom_random2();
    testSqRandom_random3();
}

#if 0
void testFirst() {
    testSqRandom();
    // testSqRandom_random3();
    // testSqRandom_dist(1);
}
#endif
