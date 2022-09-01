#include "asserts.h"

#include "Additive.h"
#include "TestComposite.h"

using Comp = Additive<TestComposite>;
using CompPtr = std::shared_ptr<Comp>;

static CompPtr makeComp() {
    CompPtr comp = std::make_shared<Comp>();
    comp->params[Comp::EVENLEVEL_PARAM].value = 1.f;
    comp->params[Comp::ODDLEVEL_PARAM].value = 1.f;
    comp->params[Comp::SLOPE_PARAM].value = 1.f;
    for (unsigned i = 0; i < Comp::numHarmonics; ++i) {
        comp->params[Comp::H0_PARAM + 1].value = 1.f;
    }
    return comp;
}

static void testAdditive0() {
    CompPtr a = makeComp();
}


static void testAdditiveInit() {
    CompPtr a = makeComp();
    const auto x = a->outputs[Comp::AUDIO_OUTPUT].value;
    assertEQ(x, 0);
    assertEQ(a->params[Comp::EVENLEVEL_PARAM].value, 1);
}

static void testAdditiveHasOutput() {
    CompPtr a = makeComp();
    auto args = TestComposite::ProcessArgs();
    a->process(args);
    const auto x = a->outputs[Comp::AUDIO_OUTPUT].value;
    assertNE(x, 0);
}

void testAdditive() {
    testAdditive0();
    testAdditiveInit();
    testAdditiveHasOutput();
    
}