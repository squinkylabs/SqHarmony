

#include "SqArray.h"
#include "asserts.h"

static void testInit() {
    SqArray<int, 5> x;
    assertEQ(x.numValid(), 0);
}

static void testPutGet() {
    SqArray<float, 20> x;
    x.putAt(0, 123.f);
    assertEQ(x.numValid(), 1);
    assertEQ(x.getAt(0), 123.f);
}

static void testBack() {
    SqArray<unsigned, 5> x;
    x.putAt(0, 27);
    x.putAt(1, 33);
    x.putAt(2, 33);
    x.putAt(0, 1);
    assertEQ(x.numValid(), 3);
}

static void testGetDirect() {
    SqArray<unsigned, 5> x;
    x.putAt(0, 27);
    x.putAt(1, 33);
    std::sort(x.getDirectPtrAt(0), x.getDirectPtrAt(2));
}

static void testDirectAccess() {
    SqArray<int, 16> x;
    x.allowRandomAccess();
    int* p0 = x.getDirectPtrAt(0);
    *p0 = 12345;
    int* p15 = x.getDirectPtrAt(15);
    *p15 = 6789;
    assertEQ(x.getAt(0), 12345);
    assertEQ(x.getAt(15), 6789);
}

static void testRandomAccess() {
    SqArray<int, 16> x;
    x.allowRandomAccess();
    x.getAt(0);
    x.getAt(15);
    x.putAt(1, 12);
    x.getDirectPtrAt(3);
}

void testSqArray() {
    testInit();
    testPutGet();
    testGetDirect();
    testDirectAccess();
    testBack();
    testRandomAccess();
}

#if 0
void testFirst() {
    // testSqArray();
    testDirectAccess();
}
#endif