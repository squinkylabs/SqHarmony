
#include "ShiftingArray.h"

#include "asserts.h"

static void testInitialConditions() {
    ShiftingArray<int> sa;
    assertEQ(sa.empty(), true);
    assertEQ(sa.size(), 0);
}

static void testPush() {
    ShiftingArray<int> sa;
    sa.push_back(22);
    assertEQ(sa.empty(), false);
    assertEQ(sa.size(), 1);
    assertEQ(sa[0], 22);
}

static void testIterator() {
    ShiftingArray<int> sa;
    sa.push_back(22);

    const auto iter = sa.begin();
    int x = *iter;
    assertEQ(x, 22);

    int count = 0;
    for (auto iter = sa.begin(); iter != sa.end(); ++iter) {
        ++count;
        assertEQ(*iter, 22);
    }
    assertEQ(count, 1);
}

void testShiftingArray() {
    testInitialConditions();
    testPush();
    testIterator();
}

#if 1
void testFirst() {;
    testShiftingArray();
}
#endif