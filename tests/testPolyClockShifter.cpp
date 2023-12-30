
#include "PolyClockShifter.h"

#include "TestComposite.h"      // need a Port definition.

static void test0() {
    Port clockOutput;
    Port clockInput;
    Port shiftAmount;
    PolyClockShifter psh;
    psh.runEveryBlock();
    psh.runEverySample(&clockOutput, &clockInput, &shiftAmount);
}
void testPolyClockShifter() {
    test0();
}