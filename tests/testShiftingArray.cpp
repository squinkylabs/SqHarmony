
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

void testShiftingArray() {
    testInitialConditions();
    testPush();
}

#if 1
void testFirst() {;
    testShiftingArray();
}
#endif