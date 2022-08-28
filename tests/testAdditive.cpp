#include "asserts.h"

#include "Additive.h"
#include "TestComposite.h"

using Comp = Additive<TestComposite>;

static void testAdditive0() {
    Comp a;
}

static void testAdditiveInit() {
    Comp a;
    const auto x = a.outputs[Comp::AUDIO_OUTPUT].value;
    assertEQ(x, 0);
}

static void testAdditiveHasOutput() {
    Comp a;
    auto args = TestComposite::ProcessArgs();
    a.process(args);
    const auto x = a.outputs[Comp::AUDIO_OUTPUT].value;
    assertNE(x, 0);
}

void testAdditive() {
    testAdditive0();
    testAdditiveInit();
    testAdditiveHasOutput();
    
}