#include "asserts.h"

#include "Harmony3.h"
#include "TestComposite.h"

using Comp = Harmony3<TestComposite>;
using CompPtr = std::shared_ptr<Comp>;

 

static void testCanCall() {
    CompPtr comp = std::make_shared<Comp>();
    Comp h;
}

void testHarmony3() {
    testCanCall();
}