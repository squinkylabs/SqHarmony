
#include "asserts.h"

#include "MelodyGenerator.h"

#include "NoteConvert.h"
#include "Scale.h"


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

static void testMelodyRowNext() {
    const size_t next = MelodyRow::nextNote(0, 2);
    assertEQ(next, 1);
} 

static void testMelodyRowNextWraps() {
    const size_t next = MelodyRow::nextNote(1, 2);
    assertEQ(next, 0);
} 

static void testMelodyRowCanPrint()
{
    MelodyRow r;
    size_t size = 3;
    Scale scale;

    // test in E phrygian
    MidiNote base(MidiNote::E);
    scale.set(base, Scale::Scales::Phrygian);

    r.init(size, scale);
    const std::string s = r.print();
    //SQINFO("here is s");
    //SQINFO(s.c_str());

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

static void testMelodyRow() {
    testMelodyRowSize();
    testMelodyRowInit();
    testMelodyRowEqual();
    testMelodyRowNext();
    testMelodyRowNextWraps();
    testMelodyRowCanPrint();
    testAveragePitch();
}

////////////////////////////////////////

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
    //  static void mutate(MelodyRow& row, MelodyMutateState& state, MelodyMutateStyle& style);

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
    MidiNote& note = r.getNote(0); 
    MidiNote& noteOrig = rOrig.getNote(0); 
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
    MidiNote& note = r.getNote(1);
    MidiNote& noteOrig = rOrig.getNote(1);
    assert(!(note == noteOrig));
    // should not have changed note 0
    note = r.getNote(0);
    noteOrig = rOrig.getNote(0);
    assert(note == noteOrig);
}

// not a real test
static void foo() {
    SQINFO("-- enter foo --");
    MelodyRow r;
    MelodyMutateState state;
    MelodyMutateStyle style;
    Scale scale = scaleCMaj();
    r.init(4, scale);
    SQINFO("here is starting row");
    SQINFO(r.print().c_str());
    for (int i = 0; i < 5; ++i) {
        MelodyGenerator::mutate(r, scale, state, style);
        SQINFO("here is next row");
        SQINFO(r.print().c_str());
    }


    SQINFO("-- exit foo --");
}

static void testMelodyGeneratorMutateDrift() {
    MelodyRow r;
    MelodyMutateState state;
    MelodyMutateStyle style;
    Scale scale = scaleCMaj();
    r.init(5, scale);

   // SQINFO(("orig row = " + r.print()).c_str());

    assertEQ(r.getAveragePitch().get(), MidiNote::MiddleC);
    for (int i = 0; i < 50; ++i) {
        MelodyGenerator::mutate(r, scale, state, style);
    }

  //  SQINFO(("mutated row = " + r.print()).c_str());

    // expect won't have moved a ton.
    assertLE(r.getAveragePitch().get(), MidiNote::MiddleC + 2);
    assertGE(r.getAveragePitch().get(), MidiNote::MiddleC - 2);
}


static void testMelodyGeneratorCanShift(int amount) {
    MelodyRow r;
    MelodyMutateState state;
    MelodyMutateStyle style;
    Scale scale = scaleCMaj();
    r.init(1, scale);

    int expectedPitch = -1;
    switch(amount) {
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
    MidiNote& note = r.getNote(0); 
    assertEQ(note.get(), expectedPitch);
}

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
}

void testMelodyGenerator() {
    testMelodyRow();
    testMelodyGeneratorMutateState();
    testMelodyGenerator2();
}

void testFirst() {
   testMelodyGenerator();
}