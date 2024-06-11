

#include "Visualizer.h"
#include "TestComposite.h"

#include "asserts.h"

using Comp = Visualizer<TestComposite>;

void clockInChord(Comp&,  const std::vector<int>&) {

}

static void testCanCall() {
    Comp v;

    const std::vector<int> foo;
    clockInChord(v, foo);
}

static void testCanClockIn() {
    Comp v;
    const std::vector<int> foo;
    clockInChord(v, foo);
}

void testVisualizer() {
    testCanCall();
    testCanClockIn();
}