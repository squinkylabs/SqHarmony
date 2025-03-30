#include "asserts.h"

#include "TestComposite.h"
#include "Mutator.h"

using Comp = Mutator<TestComposite>;

static void testCanCall() {
    Comp c;
    const auto args = TestComposite::ProcessArgs();
    c.process(args);
}

void testMutatorComposite() {
    testCanCall();
}

void testFirst() {
    //foo();
    testMutatorComposite();
}
