
#include "MelodyEvaluator.h"
#include "MelodyGenerator.h"
#include "NoteConvert.h"
#include "Scale.h"
#include "asserts.h"

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
    const auto x1 = state.random();
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
    Scale scale = scaleCMaj();
    r.init(1, scale);
    MelodyGenerator::mutate(r, scale, state, style);
}

static void testMelodyGeneratorWillMutate() {
    MelodyRow r;
    MelodyRow rOrig(r);
    MelodyMutateState state;
    MelodyMutateStyle style;
    Scale scale = scaleCMaj();
    r.init(3, scale);
    rOrig.init(3, scale);

    assert(r == rOrig);
    MelodyGenerator::mutate(r, scale, state, style);
    assert(r != rOrig);
}

static void testMelodyGeneratorMutateMulti() {
    MelodyRow r;
    MelodyRow rOrig(r);
    MelodyMutateState state;
    MelodyMutateStyle style;
    style.numToMutate = 2;
    Scale scale = scaleCMaj();
    const size_t size = 3;
    r.init(size, scale);
    rOrig.init(3, scale);

    assert(r == rOrig);
    MelodyGenerator::mutate(r, scale, state, style);
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
    Scale scale = scaleCMaj();
    const size_t size = 3;
    r.init(size, scale);
    rOrig.init(3, scale);

    assert(r == rOrig);
    MelodyGenerator::mutate(r, scale, state, style);
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
    Scale scale = scaleCMaj();
    const size_t size = 5;
    r.init(size, scale);
    rOrig.init(size, scale);

    assert(r == rOrig);
    MelodyGenerator::mutate(r, scale, state, style);
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
    Scale scale = scaleCMaj();
    r.init(3, scale);
    rOrig.init(3, scale);

    MelodyGenerator::mutate(r, scale, state, style);

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
    Scale scale = scaleCMaj();
    r.init(3, scale);
    rOrig.init(3, scale);

    MelodyGenerator::mutate(r, scale, state, style);
    rOrig = r;
    MelodyGenerator::mutate(r, scale, state, style);

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
    MelodyRow r;
    MelodyMutateState state;
    MelodyMutateStyle style;
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);
    r.init(5, scale);

    assertEQ(r.getAveragePitch().get(), MidiNote::MiddleC);
    const int iterations = 500;
    for (int i = 0; i < iterations; ++i) {
        MelodyGenerator::mutate(r, scale, state, style);
    }

    // expect won't have moved a ton.
    assertLE(r.getAveragePitch().get(), MidiNote::MiddleC + 2);
    assertGE(r.getAveragePitch().get(), MidiNote::MiddleC - 2);
}

static void testMelodyGeneratorMutateDrift2() {
    MelodyRow r;
    MelodyMutateState state;
    MelodyMutateStyle style;
    style.nonCenteredWeight = 0;
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Major);
    r.init(5, scale);

    assertEQ(r.getAveragePitch().get(), MidiNote::MiddleC);
    const int iterations = 5000;
    for (int i = 0; i < iterations; ++i) {
        MelodyGenerator::mutate(r, scale, state, style);
    }

    // expect we drifter higher in this key
    assertGT(r.getAveragePitch().get(), MidiNote::MiddleC + 12);
}

static void testMelodyGeneratorMutateDrift3() {
    MelodyRow r;
    MelodyMutateState state;
    MelodyMutateStyle style;
    style.nonCenteredWeight = 0;
    // Scale scale = scaleCMaj();
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Minor);
    r.init(5, scale);

    SQINFO(("orig row = " + r.toString()).c_str());

    assertEQ(r.getAveragePitch().get(), MidiNote::MiddleC);
    const int iterations = 5000;
    for (int i = 0; i < iterations; ++i) {
        MelodyGenerator::mutate(r, scale, state, style);
    }

    // SQINFO(("mutated row = " + r.toString()).c_str());
    //  SQINFO("avg pitch = %d", r.getAveragePitch().get());

    // expect we drifter higher in this key
    assertLT(r.getAveragePitch().get(), 0);
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
    bool adjacent, 
    size_t rowLength, 
    size_t curIndex, 
    int numToMutate, 
    const int* expected,
    int expectedNext = -1) {
    MelodyRow row;
    MelodyMutateState state;
    MelodyMutateStyle style;

    state.nextToMutate = curIndex;
    style.mutateAdjacent = adjacent;
    style.numToMutate = numToMutate;

    Scale scale = scaleCMaj();
    row.init(rowLength, scale);
    int indiciesToMutate[MelodyRow::maxNotes + 1];
    MelodyGenerator::getIndiciesToMutate(row, scale, state, style, indiciesToMutate);

    for (size_t i = 0; i < numToMutate; ++i) {
        SQINFO("in compare loop, i=%lld epxected=%d actual=%d", i, expected[i], indiciesToMutate[i]);
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
    testMelodyGeneratorMutate_getIndiciesToMutate(true, 1, 0, 1, expectedIndiciesToMutate);

    int expectedIndiciesToMutate2[MelodyRow::maxNotes + 1] = {3, -1};
    testMelodyGeneratorMutate_getIndiciesToMutate(true, 5, 3, 1, expectedIndiciesToMutate2);

    int expectedIndiciesToMutate3[MelodyRow::maxNotes + 1] = { 0, 1, 2, -1 };
    testMelodyGeneratorMutate_getIndiciesToMutate(true, 5, 0, 3, expectedIndiciesToMutate3);

    int expectedIndiciesToMutate4[MelodyRow::maxNotes + 1] = { 0, 4, -1 };
    testMelodyGeneratorMutate_getIndiciesToMutate(false, 8, 0, 2, expectedIndiciesToMutate4);

    int expectedIndiciesToMutate5[MelodyRow::maxNotes + 1] = { 0, 3, 6, -1 };
    testMelodyGeneratorMutate_getIndiciesToMutate(false, 8, 0, 3, expectedIndiciesToMutate5);

    int expectedIndiciesToMutate6[MelodyRow::maxNotes + 1] = { 1, 5, -1 };
    testMelodyGeneratorMutate_getIndiciesToMutate(false, 8, 1, 2, expectedIndiciesToMutate6);
}

// call twice to see it increment
static void testMelodyGeneratorMutate_getIndiciesToMutate2() {
    int expectedIndiciesToMutate[MelodyRow::maxNotes + 1] = {3, -1};
    testMelodyGeneratorMutate_getIndiciesToMutate(true, 5, 3, 1, expectedIndiciesToMutate, 4);

    // two adjacent
    int expectedIndiciesToMutate2[MelodyRow::maxNotes + 1] = {0, 1, -1};
    testMelodyGeneratorMutate_getIndiciesToMutate(true, 5, 0, 2, expectedIndiciesToMutate2, 2);

    // one, wrap
    int expectedIndiciesToMutate3[MelodyRow::maxNotes + 1] = {3, -1};
    testMelodyGeneratorMutate_getIndiciesToMutate(true, 4, 3, 1, expectedIndiciesToMutate3, 0);  

     // three adjacent, wrap
     SQINFO("  3 w ");
     int expectedIndiciesToMutate4[MelodyRow::maxNotes + 1] = {3, 4, 0, -1};
     testMelodyGeneratorMutate_getIndiciesToMutate(true, 5, 3, 3, expectedIndiciesToMutate4, 1);
}

// non adjacent more complex
static void testMelodyGeneratorMutate_getIndiciesToMutate3() {
    int expectedIndiciesToMutate4[MelodyRow::maxNotes + 1] = { 0, 4, -1 };
    testMelodyGeneratorMutate_getIndiciesToMutate(false, 8, 0, 2, expectedIndiciesToMutate4, 1);
}

static void testMelodyGeneratorMutate_getIndiciesToMutate() {
    testMelodyGeneratorMutate_getIndiciesToMutate1();
    testMelodyGeneratorMutate_getIndiciesToMutate2();
    testMelodyGeneratorMutate_getIndiciesToMutate3();
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
    testMelodyGeneratorMutateDrift();
    testMelodyGeneratorMutateDrift2();
    testMelodyGeneratorMutateDrift3();
    testMelodyGeneratorMutateMulti();
    testMelodyGeneratorMutateMultiWrap();
    testMelodyGeneratorMutateTooMany();

    testMelodyGeneratorMutate_getIndiciesToMutate();
}

void testMelodyGenerator() {
    testMelodyRow();
    testMelodyGeneratorMutateState();
    testMelodyGenerator2();
}

#if 1
void testFirst() {
    // testMelodyGeneratorMutateDrift3();
    testMelodyGeneratorMutate_getIndiciesToMutate();
    // testMelodyGeneratorMutateTooMany();
    // testMelodyGenerator();
}
#endif