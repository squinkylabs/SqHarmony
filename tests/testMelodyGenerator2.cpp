
#include <optional>

#include "MelodyEvaluator.h"
#include "MelodyGenerator.h"
#include "asserts.h"

//  static void _mutateOne(MelodyRow& row, size_t index, const Scale& scale, MelodyMutateState& state, const MelodyMutateStyle& style);
static MelodyRow testGenerate(std::optional<int> primeRandomCount) {
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);

    MelodyRow row;
    row.setSize(2);

    MelodyMutateState state;
    MelodyMutateStyle style;

    if (primeRandomCount.has_value()) {
        const int count = primeRandomCount.value();
        for (int i = 0; i < count; ++i) {
            state.random.generateDouble();
        }
    }

    style.disable();  // need a test for this!

    MelodyGenerator::_mutateOne(row, 0, scale, state, style);
    return row;
}

static void testGenerateRandomSanityCheck() {
    const auto row1 = testGenerate(std::optional<int>());
    const auto row2 = testGenerate(std::optional<int>());

    assert(row1 == row2);
}

static void testGenerateRandom() {
    testGenerateRandomSanityCheck();

    const auto row1 = testGenerate(std::optional<int>());
    const auto row2 = testGenerate(std::optional<int>(1));

    // Note that since the random numbers are deterministic, we will get the same result every time.
    // But the fact that only one extra call was required to pass this test is just luck.
    // May need to adapt if it starts for fail in the future.
    assert(row1 != row2);
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
}

#if 0
void testFirst() {
    // testMelodyGenerator2();
    // testGenerateRandom();
    testPenalties2Probabilities();
}
#endif