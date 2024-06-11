

#include "Visualizer.h"
#include "TestComposite.h"

#include "asserts.h"

using Comp = Visualizer<TestComposite>;

static void testCanCall() {
    Comp v;
}

void testVisualizer() {
    testCanCall();
}