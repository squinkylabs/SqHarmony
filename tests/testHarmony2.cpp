#include "asserts.h"

#include "Harmony2.h"
#include "TestComposite.h"

using Comp = Harmony2<TestComposite>;
using CompPtr = std::shared_ptr<Comp>;

 

static void testCanCall() {
    CompPtr comp = std::make_shared<Comp>();
    Comp::getTransposeLabels();
    Comp h;
}

static void testLabels() {
    const auto x = Comp::getTransposeLabels();
    assertGT(x.size(), 4);

    int count = 0;
    for (auto s : x) {
        assertGT(s.size(), 1);
        ++count;
        
    }
    assertEQ(count, x.size());
}

void testHarmony2() {
    testCanCall();
    testLabels();
}