
#include "asserts.h"

#include "MelodyGenerator.h"
#include "MelodyEvaluator.h"


//  static void _mutateOne(MelodyRow& row, size_t index, const Scale& scale, MelodyMutateState& state, const MelodyMutateStyle& style);
static MelodyRow testGenerate() {

    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);

    MelodyRow row;
    row.setSize(2);

    MelodyMutateState state;
    MelodyMutateStyle style;

    MelodyGenerator::_mutateOne(row, 0, scale, state, style);
    return row;
}

static void testGenerateRandom() {
    const auto row1 = testGenerate();
    const auto row2 = testGenerate();

    assert(row1 != row2);
}


void testMelodyGenerator2() {
    testGenerateRandom();
}


#if 1
void testFirst() {
    //testMelodyGenerator2();
    testGenerateRandom();
}
#endif