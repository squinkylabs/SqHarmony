

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

void testSqArray() {
    testInit();
    testPutGet();
}

#if 1
void testFirst() {

    testSqArray();
}
#endif