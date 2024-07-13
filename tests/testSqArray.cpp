

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
    SqArray<unsigned ,5> x;
    x.putAt(0, 27);
    x.putAt(1, 33);
    x.putAt(2, 33);
    x.putAt(0, 1);
    assertEQ(x.numValid(), 3);
}

static void testGetDirect() {
    SqArray<unsigned ,5> x;
    x.putAt(0, 27);
    x.putAt(1, 33);
    std::sort(x.getDirectPtrAt(0), x.getDirectPtrAt(2));
}

void testSqArray() {
    testInit();
    testPutGet();
    testGetDirect();
}

#if 0
void testFirst() {

   // testSqArray();
   testBack();
}
#endif