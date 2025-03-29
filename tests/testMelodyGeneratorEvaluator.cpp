
#include "asserts.h"

#include "MelodyGenerator.h"


//////////////////////////////////////////////////////////

static void testMelodyEvaluatorCanCall() {
    MelodyRow r;
    const int a = MelodyEvaluator::getPenalty(r);
    const int b = MelodyEvaluator::leapsPenalty(r);
}

//static void testMelodyGeneratorEvaluator() {
//    testMelodyGeneratorEvaluatorCanCall();
//}

void testMelodyEvaluator() {
    testMelodyEvaluatorCanCall();
}

void testFirst() {
    //foo();
   testMelodyEvaluator();
}