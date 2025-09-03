
#include "asserts.h"

#include "EvaluationSummary.h"
#include "MelodyGenerator.h"

static Scale scaleCMaj() {
    Scale scale;
    MidiNote base(MidiNote::C);
    scale.set(base, Scale::Scales::Major);
    return scale;
}

static MelodyRow makeRow(unsigned size) {
    MelodyRow row;
  //  MelodyMutateState state;
  //  MelodyMutateStyle style;

   // state.nextToMutate = 0;
  //  style.slotSelectionMethod = slotSelectionMethod;
  //  style.numToMutate = 2;

    Scale scale = scaleCMaj();
    row.init(size, scale);
    for (unsigned i=0; i<size; ++i) {
        row.setNote(i, MidiNote(MidiNote::C));
    }
    return row;
}

static void testCanCall() {
    size_t rowLength = 1;
    const auto row = makeRow(1);
    MelodyMutateStyle style;

    EvaluationSummary::fromRows(&row, 1, style, 0);
}

static void checkValid(const EvaluationSummary& summary) {
    bool seenEnabled = false;


    for (unsigned i = 0; i < numStyles; ++i) {
        const auto result = summary.results[i];
        SQINFO("result[%d] rule=%d score=%f", i, result.rule, result.score);
    }
    for (int i= (numStyles -1); i >= 0; --i) {
        const auto result = summary.results[i];
        if (result.rule == Styles::Disabled) {
            assert(seenEnabled == false);
        } else {
            if (i > 0) {
                const auto otherResult = summary.results[i-1];
                assert (result.rule != otherResult.rule);
                assert(otherResult.rule != Styles::Disabled);
                assert(otherResult.score >= result.score);
            }
        }
    }    
}

static void testOctave(unsigned numRows) {
    assert(numRows < 3);
    MelodyRow rows[2];
    size_t rowLength = 2;
    rows[0] = makeRow(rowLength);
    rows[1] = makeRow(rowLength);

    rows[0].setNote(1, MidiNote(MidiNote::MiddleC + 12));
    rows[1].setNote(1, MidiNote(MidiNote::MiddleC + 12));
    MelodyMutateStyle style;

    const auto summary = EvaluationSummary::fromRows(rows, numRows, style, 0);
    checkValid(summary);
    const auto topResult = summary.results[0];
    assert(topResult.rule != Styles::Disabled);
    assert(topResult.score > 0);    
}

static void testOctave() {
    testOctave(1);
}

static void testOctave2() {
    testOctave(2);
}

void testEvaluationSummary() {
    testCanCall();
    testOctave();
     testOctave2();
}

#if 0
void testFirst() {
    testEvaluationSummary();
}
#endif