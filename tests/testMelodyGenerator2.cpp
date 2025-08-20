
#include <optional>

#include "FloatNote.h"
#include "MelodyEvaluator.h"
#include "MelodyGenerator.h"
#include "NoteConvert.h"
#include "PitchKnowledge.h"
#include "asserts.h"

/**
 * @brief
 *
 * @param primeRandomCount - the number of times to run the prime number generator first
 * @return MelodyRow
 */
static void testGenerate(MelodyRow& row, unsigned iterations, unsigned primeRandomCount, MelodyMutateStyle style) {
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);

    ;
    row.setSize(2);

    MelodyMutateState state;

    for (unsigned i = 0; i < primeRandomCount; ++i) {
        state.random.generateDouble();
    }

    while (iterations--) {
        SQINFO("----- iteratrion row=%s",
               row.toString().c_str());
        SQINFO(" evaluation = %s",
               MelodyEvaluator::toString(row, style).c_str());
        MelodyGenerator::_mutateOne(row, 0, scale, state, style);
    }
}

static void testGenerateRandomSanityCheck() {
    MelodyMutateStyle style;  // default style
    MelodyRow row1, row2;
    testGenerate(row1, 1, 0, style);
    testGenerate(row2, 1, 0, style);

    assert(row1 == row2);
}

static void testGenerateRandom() {
    testGenerateRandomSanityCheck();

    MelodyMutateStyle style;  // default style
    MelodyRow row1, row2;

    testGenerate(row1, 1, 0, style);
    testGenerate(row2, 1, 1, style);

    // Note that since the random numbers are deterministic, we will get the same result every time.
    // But the fact that only one extra call was required to pass this test is just luck.
    // May need to adapt if it starts for fail in the future.
    assert(row1 != row2);
}

static void testDriftRate(unsigned iterations) {
    MelodyMutateStyle style;
    SQINFO("default style = %s", style.toString().c_str());
    style.setStyles(Styles::OnlySeekCenter);

    SQINFO("style= %s", style.toString().c_str());

    MidiNote note(MidiNote::C3 + 5 * 12);

    const std::string s = PitchKnowledge::nameOfAbs(note.get());
    SQINFO("orig target = %s", s.c_str());
    FloatNote fNote;
    NoteConvert::m2f(fNote, note);
    style.centerVoltage = fNote.get();

    //    const unsigned iteration = 10;
    MelodyRow row;
    row.setSize(2);
    row.setNote(0, note);
    testGenerate(
        row,
        iterations,
        0,
        style);

    SQINFO("iter = %d row: %s", iterations, row.toString().c_str());
}

static void testDriftRate() {
    //  testDriftRate(1);
    testDriftRate(10);
    //  testDriftRate(100);
}

static void testPenalties2ProbabilitiesSub(unsigned num, const float* penalties, const float* expectedProbabilities) {
    assert(num < 10);
    float temp[10];
    MelodyGenerator::_penalties2Probabilities(num, penalties, temp);

    float sum = 0;
    for (unsigned i = 0; i < num; ++i) {
        const auto x = temp[i];
        assertGE(x, 0.f);
        assertLE(x, 1.f);

        assertClose(x, expectedProbabilities[i], .001f);
        sum += x;
    }

    assertClose(sum, 1.f, .0001f)
}

static void testPenalties2ProbabilitiesSame() {
    const float pen[] = {1, 1};
    float prob[2];
    MelodyGenerator::_penalties2Probabilities(2, pen, prob);
    assertClose(prob[0], .5, .00001);
    assertClose(prob[1], .5, .00001);
}

static void testPenalties2Probabilities() {
    {
        const float p[] = {1.f, 1.f, 1.f};
        const float e[] = {.3333f, .3333f, .3333f};
        testPenalties2ProbabilitiesSub(3, p, e);
    }
    {
        const float p[] = {10.f, 10.f};
        const float e[] = {.5f, .5f};
        testPenalties2ProbabilitiesSub(2, p, e);
    }
    {
        const float p[] = {112.f};
        const float e[] = {1.f};
        testPenalties2ProbabilitiesSub(1, p, e);
    }
    {
        const float p[] = {0.f};
        const float e[] = {1.f};
        testPenalties2ProbabilitiesSub(1, p, e);
    }
    {
        const float p[] = {1.f, 2.f, 3.f};
        const float e[] = {.6666f, .3333f, 0};
        testPenalties2ProbabilitiesSub(3, p, e);
    }
}

void testMelodyGenerator2() {
    testGenerateRandom();
    testPenalties2ProbabilitiesSame();
    testPenalties2Probabilities();
    testDriftRate();
}

#if 1
void testFirst() {
    testDriftRate();
    // testMelodyGenerator2();
    //  testGenerateRandom();
    // testPenalties2Probabilities();
}
#endif