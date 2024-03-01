#include "asserts.h"

#include "Harmony2.h"
#include "TestComposite.h"

using Comp = Harmony2<TestComposite>;
using CompPtr = std::shared_ptr<Comp>;

 

static void testCanCall() {
    CompPtr comp = std::make_shared<Comp>();
    Comp::getTransposeDegreeLabels();
    Comp::getTransposeOctaveLabels();
    Comp h;
}

static void testLabels(std::vector<std::string>& x) {
    
    assertGT(x.size(), 4);

    int count = 0;
    for (auto s : x) {
        assertGT(s.size(), 0);
        ++count;
        
    }
    assertEQ(count, x.size());
}

static void testLabels() {
    auto y = Comp::getTransposeOctaveLabels();
    testLabels(y);
    y = Comp::getTransposeDegreeLabels();
    testLabels(y);
}

void testHarmony2() {
    testCanCall();
    testLabels();
}