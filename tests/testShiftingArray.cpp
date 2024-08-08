
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

static void testShift() {
    ShiftingArray<int> sa;
    sa.push_back(44);
    sa.shift(11);
    assertEQ(sa.size(), 2);
    assertEQ(sa[0], 11);
    assertEQ(sa[1], 44);

}
static void testShiftEmpty() {
      ShiftingArray<int> sa;
      sa.shift(22);
      assertEQ(sa.empty(), true);
}
static void testShiftMulti() {
    ShiftingArray<std::string> sa;
    sa.push_back("44");
    sa.shift(3, "");
    const int x = sa.size();
    assertEQ(sa.size(), 4);
    assertEQ(sa[0].empty(), true);
    assertEQ(sa[1].empty(), true);
    assertEQ(sa[2].empty(), true);
   // assertEQ(sa[0].empty(), true);
 assertEQ(sa[3], "44");
}

void testShiftingArray() {
    testInitialConditions();
    testPush();
    testIterator();
    testShift();
    testShiftEmpty();
    testShiftMulti();
}

#if 0
void testFirst() {
    ;
    testShiftingArray();
}
#endif