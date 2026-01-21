
#include "EvaluationSummary.h"
#include "MelodyGenerator.h"
#include "asserts.h"

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
    for (unsigned i = 0; i < size; ++i) {
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
    summary.assertValid();
    const auto topResult = summary.results[0];
    assert(topResult.rule != Styles::Disabled);
    assert(topResult.score > 0);
    SQINFO("end of testOcatve %d: %s", numRows, summary.toString().c_str());
}

static void testOctave() {
    testOctave(1);
}

static void testOctave2() {
    testOctave(2);
}

static void testValid() {
    EvaluationSummary es;
    es.assertValid();

    es.results[0].score = 1;
    SQINFO("inv = %s", es.toString().c_str());
    assertEQ(es.isValid(), false);
}

static void testCombine1() {
    EvaluationSummary es1;
    EvaluationSummary es2;
    EvaluationSummary::combine(es1, es2);
    es1.assertValid();

    EvaluationSummary es3;
    assert(es1 == es3);
}

static void testCombine2() {
    EvaluationSummary es1;
    EvaluationSummary es2;
    es2.results[0].rule = Styles::OnlySeekCenter;
    es2.results[0].score = 1;
    EvaluationSummary::combine(es1, es2);
    es1.assertValid();

    EvaluationSummary es3 = {{Styles::OnlySeekCenter, 1},
                             {Styles::Disabled, 0},
                             {Styles::Disabled, 0},
                             {Styles::Disabled, 0}};

    assert(es1 == es3);
}

static void testCombine3() {
    EvaluationSummary es1;
    es1.results[0].rule = Styles::OnlyDiscorageLeaps;
    es1.results[0].score = 1;
    EvaluationSummary es2;
    es2.results[0].rule = Styles::OnlySeekCenter;
    es2.results[0].score = 1;
    EvaluationSummary::combine(es1, es2);
    es1.assertValid();

    SQINFO("combined = %s", es1.toString().c_str());
    EvaluationSummary es3 = { {Styles::OnlyDiscorageLeaps, 1},
                             {Styles::OnlySeekCenter, 1},
                             {Styles::Disabled, 0},
                             {Styles::Disabled, 0} };

    assert(es1 == es3);
}

static void testCombine() {
    testCombine1();
    testCombine2();
    testCombine3();
}

void testEvaluationSummary() {
    testCanCall();
    testOctave();
    testOctave2();

    testValid();
    testCombine();
}

#if 0
void testFirst() {
    // testEvaluationSummary();
    testCombine3();
}
#endif