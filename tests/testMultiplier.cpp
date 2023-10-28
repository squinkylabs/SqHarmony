
#include "Multiplier.h"
#include "TestComposite.h"

#include "asserts.h"
#include <memory>

using Comp = Multiplier<TestComposite>;
using CompPtr = std::shared_ptr<Comp>;

static void test0() {
    Comp p;
}




void testMultiplier() {
    test0();
}