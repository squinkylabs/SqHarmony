
#include "EvaluationSummary.h"
#include "FloatNote.h"
#include "MelodyEvaluator.h"
#include "MelodyGenerator.h"
#include "NoteConvert.h"
#include "Scale.h"
#include "asserts.h"

extern bool verboseProbability;

static Scale scaleCMaj() {
    Scale scale;
    MidiNote base(MidiNote::C);
    scale.set(base, Scale::Scales::Major);
    return scale;
}

static void testMelodyRowSize() {
    MelodyRow r;
    r.setSize(2);
    assertEQ(r.getSize(), 2);
}

static void testMelodyRowInit() {
    MelodyRow r;
    size_t size = 7;
    Scale scale = scaleCMaj();

    r.init(size, scale);

    assertEQ(r.getSize(), size);

    for (unsigned i = 0; i < size; ++i) {
        ScaleNote scaleNote;
        const MidiNote midiNote = r.getNote(i);
        NoteConvert::m2s(scaleNote, scale, midiNote);
        assertEQ(scaleNote.getDegree(), 0);
        assertEQ(scaleNote.getOctave(), 4);
    }
}

static void testMelodyRowEqual() {
    MelodyRow r;
    size_t size = 3;
    Scale scale;

    // test in E phrygian
    MidiNote base(MidiNote::E);
    scale.set(base, Scale::Scales::Phrygian);

    {
        // sanity check
        ScaleNote scaleNote;
        NoteConvert::m2s(scaleNote, scale, base);
        assertEQ(scaleNote.getDegree(), 0);
    }

    r.init(size, scale);
    MelodyRow r2 = r;
    assert(r == r2);

    Scale scale2;
    MidiNote base2(MidiNote::F);
    scale2.set(base2, Scale::Scales::Phrygian);
    r2.init(size, scale2);
    assert(r != r2);

    r2 = r;
    r2.setSize(size + 1);
    assert(r != r2);
}

static void testMelodyRowWrap() {
    MelodyRow r;
    Scale scale = scaleCMaj();
    r.init(4, scale);
    assertEQ(r.wrapIndex(1), 1);
}

static void testMelodyRowWrap2() {
    MelodyRow r;
    Scale scale = scaleCMaj();
    r.init(4, scale);
    assertEQ(r.wrapIndex(5), 1);
}

static void testMelodyRowCanPrint() {
    MelodyRow r;
    size_t size = 3;
    Scale scale;

    // test in E phrygian
    MidiNote base(MidiNote::E);
    scale.set(base, Scale::Scales::Phrygian);

    r.init(size, scale);
    const std::string s = r.toString();
    assert(!s.empty());

    assert(s.find(',') != std::string::npos);
    assert(s.find(' ') != std::string::npos);
    assert(s.back() != ',');
    assert(s.back() != ' ');
}

static void testAveragePitch() {
    MelodyRow r;
    size_t size = 3;
    Scale scale;

    MidiNote base(MidiNote::A);
    scale.set(base, Scale::Scales::Mixolydian);
    r.init(size, scale);

    const MidiNote average = r.getAveragePitch();
    assertEQ(average.get(), MidiNote::A + MidiNote::MiddleC);
}

static void testMelodyRowSetNote() {
    MelodyRow r;
    size_t size = 3;
    Scale scale;
    MidiNote base(MidiNote::A);
    scale.set(base, Scale::Scales::Mixolydian);
    r.init(size, scale);

    MidiNote t(21);
    const MidiNote& t2 = r.getNote(1);
    assert(!(t == t2));

    r.setNote(1, t);
    const MidiNote& t3 = r.getNote(1);
    // t2 = r.getNote(1);
    assert(t == t3);
}

static void testMelodyRowWrapNote() {
    MelodyRow r;
    size_t size = 3;
    Scale scale;
    MidiNote base(MidiNote::A);
    scale.set(base, Scale::Scales::Mixolydian);
    r.init(size, scale);

    MidiNote t(21);
    const MidiNote& t2 = r.getNote(0);
    assert(!(t == t2));

    r.setNote(0, t);  // set first one
    const MidiNote& t3 = r.getNote(size);
    // it should show up past the end, too.
    assert(t == t3);

    const MidiNote& t4 = r.getNote(0);
    assert(t == t4);
}

static void testMelodyRow() {
    testMelodyRowSize();
    testMelodyRowInit();
    testMelodyRowEqual();
    testMelodyRowWrap();
    testMelodyRowWrap2();
    testMelodyRowCanPrint();
    testAveragePitch();
    testMelodyRowSetNote();
    testMelodyRowWrapNote();
}

////////////////////////////////////////////////////////

static void testMelodyGeneratorMutateStateRandomSeed() {
    MelodyMutateState state;
    const auto x1 = state.random.generateDouble();
    assertNE(x1, 0);
}

static void testMelodyGeneratorMutateState() {
    testMelodyGeneratorMutateStateRandomSeed();
}

////////////////////////////////////////

static void testMelodyGeneratorCanCall() {
    MelodyRow r;
    MelodyMutateState state;
    MelodyMutateStyle style;
    style.scale = scaleCMaj();
    r.init(1, style.scale);
    MelodyGenerator::mutate(r, state, style);
}

static void testMelodyGeneratorWillMutate() {
    MelodyRow r;
    MelodyRow rOrig(r);
    MelodyMutateState state;
    MelodyMutateStyle style;
    style.scale = scaleCMaj();
    r.init(3, style.scale);
    rOrig.init(3, style.scale);

    assert(r == rOrig);
    MelodyGenerator::mutate(r, state, style);
    assert(r != rOrig);
}

static void testMelodyGeneratorMutateMulti() {
    MelodyRow r;
    MelodyRow rOrig(r);
    MelodyMutateState state;
    MelodyMutateStyle style;
    style.numToMutate = 2;
    style.scale = scaleCMaj();
    const size_t size = 3;
    r.init(size, style.scale);
    rOrig.init(3, style.scale);

    assert(r == rOrig);
    MelodyGenerator::mutate(r, state, style);
    int numChanged = 0;
    for (size_t i = 0; i < size; ++i) {
        if (r.getNote(i).get() != rOrig.getNote(i).get()) {
            ++numChanged;
        }
    }
    assertEQ(numChanged, 2);
}

static void testMelodyGeneratorMutateMultiWrap() {
    MelodyRow r;
    MelodyRow rOrig(r);
    MelodyMutateState state;
    MelodyMutateStyle style;
    style.numToMutate = 2;
    state.nextToMutate = 2;
    style.scale = scaleCMaj();
    const size_t size = 3;
    r.init(size, style.scale);
    rOrig.init(3, style.scale);

    assert(r == rOrig);
    MelodyGenerator::mutate(r, state, style);
    int numChanged = 0;
    for (size_t i = 0; i < size; ++i) {
        if (r.getNote(i).get() != rOrig.getNote(i).get()) {
            ++numChanged;
        }
    }
    assertEQ(numChanged, 2);
}

static void testMelodyGeneratorMutateTooMany() {
    MelodyRow r;
    MelodyRow rOrig(r);
    MelodyMutateState state;
    MelodyMutateStyle style;
    style.numToMutate = 100;
    state.nextToMutate = 2;
    style.scale = scaleCMaj();
    const size_t size = 5;
    r.init(size, style.scale);
    rOrig.init(size, style.scale);

    assert(r == rOrig);
    MelodyGenerator::mutate(r, state, style);
    int numChanged = 0;
    for (size_t i = 0; i < size; ++i) {
        if (r.getNote(i).get() != rOrig.getNote(i).get()) {
            ++numChanged;
        }
    }
    assertEQ(numChanged, size);
}

static void testMelodyGeneratorWillMutateFirstNoteByDefault() {
    MelodyRow r;
    MelodyRow rOrig(r);
    MelodyMutateState state;
    MelodyMutateStyle style;
    style.scale = scaleCMaj();
    r.init(3, style.scale);
    rOrig.init(3, style.scale);

    MelodyGenerator::mutate(r, state, style);

    // should have changed note 0
    MidiNote note = r.getNote(0);
    MidiNote noteOrig = rOrig.getNote(0);
    assert(!(note == noteOrig));
    // should not have changed note 1
    note = r.getNote(1);
    noteOrig = rOrig.getNote(1);
    assert(note == noteOrig);
}

static void testMelodyGeneratorWillMutateSecondNote() {
    MelodyRow r;
    MelodyRow rOrig(r);
    MelodyMutateState state;
    MelodyMutateStyle style;
    style.scale = scaleCMaj();
    r.init(3, style.scale);
    rOrig.init(3, style.scale);

    MelodyGenerator::mutate(r, state, style);
    rOrig = r;
    MelodyGenerator::mutate(r, state, style);

    // should have changed note 1
    MidiNote note = r.getNote(1);
    MidiNote noteOrig = rOrig.getNote(1);
    assert(!(note == noteOrig));
    // should not have changed note 0
    note = r.getNote(0);
    noteOrig = rOrig.getNote(0);
    assert(note == noteOrig);
}

static void testMelodyGeneratorMutateDrift() {
    //    SQINFO("testMelodyGeneratorMutateDrift");
    MelodyRow r;
    MelodyMutateState state;
    MelodyMutateStyle style;
    // Scale scale;
    style.scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);
    r.init(5, style.scale);

// Style default center voltage is MidiNote::MiddleC
#if 0
    const float cv =  style.centerVoltage;
    FloatNote fn(cv);
    MidiNote mcv;
    NoteConvert::f2m(mcv, fn);
   SQINFO("center = %f midi = %d", cv, mcv.get());

   MidiNote mMiddleC(MidiNote::MiddleC);
   NoteConvert::m2f(fn, mMiddleC);
   SQINFO("middle c = %f %d", fn.get(), mMiddleC.get());
#endif

    assertEQ(r.getAveragePitch().get(), MidiNote::MiddleC);
    const int iterations = 500;  // was 500
    for (int i = 0; i < iterations; ++i) {
        // SQINFO("\n--- iteration %d", i);
        MelodyGenerator::mutate(r, state, style);
        const std::string s = MelodyEvaluator::toString(r, style);
        SQINFO("iter %d avg = %d %s", i, r.getAveragePitch().get(), s.c_str());
        // SQINFO("** ROW: %s", r.toString().c_str());
    }

    SQINFO("---- end testMelodyGeneratorMutateDrift");
    // expect won't have moved a ton.
    assertLE(r.getAveragePitch().get(), MidiNote::MiddleC + 4);
    assertGE(r.getAveragePitch().get(), MidiNote::MiddleC - 4);
}

static void testMelodyGeneratorCanShift(int amount) {
    MelodyRow r;
    MelodyMutateState state;
    MelodyMutateStyle style;
    Scale scale = scaleCMaj();
    r.init(1, scale);

    int expectedPitch = -1;
    switch (amount) {
        case 0:
            expectedPitch = MidiNote::MiddleC;
            break;
        case 1:
            expectedPitch = MidiNote::MiddleC + MidiNote::D;
            break;
        case 2:
            expectedPitch = MidiNote::MiddleC + MidiNote::E;
            break;
        case -1:
            expectedPitch = MidiNote::MiddleC + MidiNote::B - 12;
            break;
        default:
            assert(false);
    }

    MelodyGenerator::_changeOneNoteInMode(r, scale, 0, amount);

    // should have changed note 0
    const MidiNote& note = r.getNote(0);
    assertEQ(note.get(), expectedPitch);
}

static void testMelodyGeneratorMutate_getIndiciesToMutate(
    //   bool adjacent,
    SlotSelectionMethod slotSelectionMethod,
    size_t rowLength,
    size_t curIndex,
    int numToMutate,
    const int* expected,
    int expectedNext = -1) {
    MelodyRow row;
    MelodyMutateState state;
    MelodyMutateStyle style;

    state.nextToMutate = curIndex;
    style.slotSelectionMethod = slotSelectionMethod;
    style.numToMutate = numToMutate;

    Scale scale = scaleCMaj();
    row.init(rowLength, scale);
    int indiciesToMutate[MelodyRow::maxNotes + 1];
    MelodyGenerator::getIndiciesToMutate(row, state, style, indiciesToMutate);

    for (size_t i = 0; i < numToMutate; ++i) {
        // SQINFO("in compare loop, i=%lld epxected=%d actual=%d", i, expected[i], indiciesToMutate[i]);
        assertEQ(indiciesToMutate[i], expected[i]);
    }
    assertEQ(indiciesToMutate[numToMutate], -1);

    if (expectedNext >= 0) {
        assertEQ(state.nextToMutate, expectedNext);
    }
}

// various combinations, only call once.
static void testMelodyGeneratorMutate_getIndiciesToMutate1() {
    int expectedIndiciesToMutate[MelodyRow::maxNotes + 1] = {0, -1};
    testMelodyGeneratorMutate_getIndiciesToMutate(SlotSelectionMethod::ROUND_ROBIN_ADJACENT, 1, 0, 1, expectedIndiciesToMutate);

    int expectedIndiciesToMutate2[MelodyRow::maxNotes + 1] = {3, -1};
    testMelodyGeneratorMutate_getIndiciesToMutate(SlotSelectionMethod::ROUND_ROBIN_ADJACENT, 5, 3, 1, expectedIndiciesToMutate2);

    int expectedIndiciesToMutate3[MelodyRow::maxNotes + 1] = {0, 1, 2, -1};
    testMelodyGeneratorMutate_getIndiciesToMutate(SlotSelectionMethod::ROUND_ROBIN_ADJACENT, 5, 0, 3, expectedIndiciesToMutate3);

    int expectedIndiciesToMutate4[MelodyRow::maxNotes + 1] = {0, 4, -1};
    testMelodyGeneratorMutate_getIndiciesToMutate(SlotSelectionMethod::ROUND_ROBIN_DISTRIBUTED, 8, 0, 2, expectedIndiciesToMutate4);

    int expectedIndiciesToMutate5[MelodyRow::maxNotes + 1] = {0, 3, 6, -1};
    testMelodyGeneratorMutate_getIndiciesToMutate(SlotSelectionMethod::ROUND_ROBIN_DISTRIBUTED, 8, 0, 3, expectedIndiciesToMutate5);

    int expectedIndiciesToMutate6[MelodyRow::maxNotes + 1] = {1, 5, -1};
    testMelodyGeneratorMutate_getIndiciesToMutate(SlotSelectionMethod::ROUND_ROBIN_DISTRIBUTED, 8, 1, 2, expectedIndiciesToMutate6);
}

// call twice to see it increment
static void testMelodyGeneratorMutate_getIndiciesToMutate2() {
    int expectedIndiciesToMutate[MelodyRow::maxNotes + 1] = {3, -1};
    testMelodyGeneratorMutate_getIndiciesToMutate(SlotSelectionMethod::ROUND_ROBIN_ADJACENT, 5, 3, 1, expectedIndiciesToMutate, 4);

    // two adjacent
    int expectedIndiciesToMutate2[MelodyRow::maxNotes + 1] = {0, 1, -1};
    testMelodyGeneratorMutate_getIndiciesToMutate(SlotSelectionMethod::ROUND_ROBIN_ADJACENT, 5, 0, 2, expectedIndiciesToMutate2, 2);

    // one, wrap
    int expectedIndiciesToMutate3[MelodyRow::maxNotes + 1] = {3, -1};
    testMelodyGeneratorMutate_getIndiciesToMutate(SlotSelectionMethod::ROUND_ROBIN_ADJACENT, 4, 3, 1, expectedIndiciesToMutate3, 0);

    // three adjacent, wrap
    int expectedIndiciesToMutate4[MelodyRow::maxNotes + 1] = {3, 4, 0, -1};
    testMelodyGeneratorMutate_getIndiciesToMutate(SlotSelectionMethod::ROUND_ROBIN_ADJACENT, 5, 3, 3, expectedIndiciesToMutate4, 1);
}

// non adjacent more complex
static void testMelodyGeneratorMutate_getIndiciesToMutate3() {
    int expectedIndiciesToMutate4[MelodyRow::maxNotes + 1] = {0, 4, -1};
    testMelodyGeneratorMutate_getIndiciesToMutate(SlotSelectionMethod::ROUND_ROBIN_DISTRIBUTED, 8, 0, 2, expectedIndiciesToMutate4, 1);
}

// mutate all
static void testMelodyGeneratorMutate_getIndiciesToMutate4() {
    MelodyRow row;
    MelodyMutateState state;
    MelodyMutateStyle style;
    Scale scale = scaleCMaj();

    const int rowLength = 5;
    style.slotSelectionMethod = SlotSelectionMethod::ROUND_ROBIN_ADJACENT;
    style.numToMutate = 0;

    row.init(rowLength, scale);
    int indiciesToMutate[MelodyRow::maxNotes + 1];
    MelodyGenerator::getIndiciesToMutate(row, state, style, indiciesToMutate);
    assertEQ(indiciesToMutate[0], 0);
    assertEQ(indiciesToMutate[1], 1);
    assertEQ(indiciesToMutate[2], 2);
    assertEQ(indiciesToMutate[3], 3);
    assertEQ(indiciesToMutate[4], 4);
    assertEQ(indiciesToMutate[5], -1);
}

// random, random one
static void testMelodyGeneratorMutate_getIndiciesToMutate5() {
    SlotSelectionMethod slotSelectionMethod = SlotSelectionMethod::RANDOM_RANDOM;  // fully random
    size_t rowLength = 9;
    MelodyRow row;
    MelodyMutateState state;
    MelodyMutateStyle style;

    state.nextToMutate = 0;
    style.slotSelectionMethod = slotSelectionMethod;
    style.numToMutate = 1;

    Scale scale = scaleCMaj();
    row.init(rowLength, scale);
    int indiciesToMutate[MelodyRow::maxNotes + 1];

    std::set<int> haveSeen;

    // note that "5" is a heuristic here - depends on random stuff.
    int expectedRR = 0;
    int seenExpectedRR = 0;
    for (int i = 0; i < rowLength * 3; ++i) {
        MelodyGenerator::getIndiciesToMutate(row, state, style, indiciesToMutate);
        assertEQ(indiciesToMutate[1], -1);
        haveSeen.insert(indiciesToMutate[0]);
        //  SQINFO("just added %d", indiciesToMutate[0]);
        if (indiciesToMutate[0] == expectedRR) {
            seenExpectedRR++;
        }
        expectedRR++;
    }
    // size_t rowLength = 9;
    assertEQ(haveSeen.size(), rowLength);  // we should have seen every possible index...
    assertEQ(seenExpectedRR, 0);           // but never in round robin order
}

// random, adjacent one
static void testMelodyGeneratorMutate_getIndiciesToMutate6() {
    SlotSelectionMethod slotSelectionMethod = SlotSelectionMethod::RANDOM_ADJACENT;
    size_t rowLength = 11;
    MelodyRow row;
    MelodyMutateState state;
    MelodyMutateStyle style;

    state.nextToMutate = 0;
    style.slotSelectionMethod = slotSelectionMethod;
    style.numToMutate = 2;

    Scale scale = scaleCMaj();
    row.init(rowLength, scale);
    int indiciesToMutate[MelodyRow::maxNotes + 1];

    std::set<int> haveSeen;

    for (int i = 0; i < rowLength * 3; ++i) {
        MelodyGenerator::getIndiciesToMutate(row, state, style, indiciesToMutate);
        assertEQ(indiciesToMutate[2], -1);  // there should be two
        haveSeen.insert(indiciesToMutate[0]);

        int expectedNext = indiciesToMutate[0] + 1;
        if (expectedNext >= rowLength) {
            expectedNext -= rowLength;
        }

        assertEQ(indiciesToMutate[1], expectedNext);
    }
    assertEQ(haveSeen.size(), rowLength);  // we should have seen every possible index...
}

// random, distributed one
static void testMelodyGeneratorMutate_getIndiciesToMutate7() {
    SlotSelectionMethod slotSelectionMethod = SlotSelectionMethod::RANDOM_DISTRIBUTED;
    size_t rowLength = 12;
    MelodyRow row;
    MelodyMutateState state;
    MelodyMutateStyle style;

    state.nextToMutate = 0;
    style.slotSelectionMethod = slotSelectionMethod;
    style.numToMutate = 2;

    Scale scale = scaleCMaj();
    row.init(rowLength, scale);
    int indiciesToMutate[MelodyRow::maxNotes + 1];

    std::set<int> haveSeen;
    // SQINFO("row len = %lld", rowLength);
    for (int i = 0; i < rowLength * 4; ++i) {
        MelodyGenerator::getIndiciesToMutate(row, state, style, indiciesToMutate);
        assertEQ(indiciesToMutate[2], -1);  // there should be two
        haveSeen.insert(indiciesToMutate[0]);

        int expectedNext = indiciesToMutate[0] + (rowLength / 2);
        if (expectedNext >= rowLength) {
            expectedNext -= rowLength;
        }
        // SQINFO("in loop i=%d, tomutate= %d %d", i, indiciesToMutate[0], expectedNext);

        assertEQ(indiciesToMutate[1], expectedNext);
    }
    assertEQ(haveSeen.size(), rowLength);  // we should have seen every possible index...
}

static void testMelodyGeneratorMutate_getIndiciesToMutate() {
    testMelodyGeneratorMutate_getIndiciesToMutate1();
    testMelodyGeneratorMutate_getIndiciesToMutate2();
    testMelodyGeneratorMutate_getIndiciesToMutate3();
    testMelodyGeneratorMutate_getIndiciesToMutate4();
    // testMelodyGeneratorMutate_getIndiciesToMutate5();
    SQINFO("fix testMelodyGeneratorMutate_getIndiciesToMutate5");
    testMelodyGeneratorMutate_getIndiciesToMutate6();
    testMelodyGeneratorMutate_getIndiciesToMutate7();
}

static void testMelodyGenerator_toMutateIncludes() {
    const int x[] = {-1};
    assert(!MelodyGenerator::toMutateIncludes(x, 7));
    const int x2[] = {0, -1};
    assert(MelodyGenerator::toMutateIncludes(x2, 0));
    assert(!MelodyGenerator::toMutateIncludes(x2, 1));

    const int x3[] = {8, 9, -1};
    assert(!MelodyGenerator::toMutateIncludes(x3, 7));
    assert(MelodyGenerator::toMutateIncludes(x3, 8));
    assert(MelodyGenerator::toMutateIncludes(x3, 9));
}
/////////////////////////////////////////////////////

static void testMelodyGeneratorCanShift() {
    testMelodyGeneratorCanShift(0);
    testMelodyGeneratorCanShift(1);
    testMelodyGeneratorCanShift(-1);
    testMelodyGeneratorCanShift(2);
}

static void testMelodyGenerator2() {
    testMelodyGeneratorCanCall();
    testMelodyGeneratorCanShift();
    testMelodyGeneratorWillMutate();
    testMelodyGeneratorWillMutateFirstNoteByDefault();
    testMelodyGeneratorWillMutateSecondNote();

    SQINFO("!! make drift test work again !!");
  //  testMelodyGeneratorMutateDrift();
    testMelodyGeneratorMutateMulti();
    testMelodyGeneratorMutateMultiWrap();
    testMelodyGeneratorMutateTooMany();

    testMelodyGeneratorMutate_getIndiciesToMutate();
}

void testMelodyGenerator() {
    testMelodyGenerator_toMutateIncludes();
    testMelodyRow();
    testMelodyGeneratorMutateState();
    testMelodyGenerator2();
}

#if 0
void testFirst() {
    //   verboseProbability = true;
    testMelodyGeneratorMutateDrift();
    // testMelodyGeneratorMutate_getIndiciesToMutate();
    // testMelodyGenerator_toMutateIncludes();
    //   testMelodyGeneratorMutate_getIndiciesToMutate5();
    // testMelodyGeneratorMutate_getIndiciesToMutate7();
    // testMelodyGeneratorMutateTooMany();
    //   testMelodyGenerator();
    // testMelodyGenerator_random();
    // testMelodyGenerator_random2();
}
#endif