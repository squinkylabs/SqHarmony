#include "asserts.h"

#include "Additive.h"
#include "TestComposite.h"

using Comp = Additive<TestComposite>;

static void testAdditive0() {
    Comp a;
}

void testAdditive() {
    testAdditive0();
    
}