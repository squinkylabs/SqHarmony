

#include "MidiNote.h"
#include "asserts.h"

static void testMidiNoteCtor() {
    MidiNote m;
    assert(m.get() == 64);

    MidiNote m2(12);
    assert(m2.get() == 12);
}

//------------------- let's test c on up
static void testMidiStaffC() {
    MidiNote mn(MidiNote::MiddleC);
    int ll = mn.getLegerLine(false);
    assertEQ(ll, -2);
    ll = mn.getLegerLine(SharpsFlatsPref::Flats, false);
    assertEQ(ll, -2);
    ll = mn.getLegerLine(SharpsFlatsPref::Sharps, false);
    assertEQ(ll, -2);
    ll = mn.getLegerLine(SharpsFlatsPref::DontCare, false);
    assertEQ(ll, -2);
}

static void testMidiStaffCSharp() {
    MidiNote mn(MidiNote::MiddleC + 1);
    int ll = mn.getLegerLine(false);
    assertEQ(ll, -2);
    ll = mn.getLegerLine(SharpsFlatsPref::Flats, false);
    assertEQ(ll, -1);
    ll = mn.getLegerLine(SharpsFlatsPref::Sharps, false);
    assertEQ(ll, -2);
    ll = mn.getLegerLine(SharpsFlatsPref::DontCare, false);
    assertEQ(ll, -2);
}

static void testMidiStaffD() {
    MidiNote mn(MidiNote::MiddleC + MidiNote::D);
    int ll = mn.getLegerLine(false);
    assertEQ(ll, -1);
    ll = mn.getLegerLine(SharpsFlatsPref::Flats, false);
    assertEQ(ll, -1);
    ll = mn.getLegerLine(SharpsFlatsPref::Sharps, false);
    assertEQ(ll, -1);
    ll = mn.getLegerLine(SharpsFlatsPref::DontCare, false);
    assertEQ(ll, -1);
}

static void testMidiStaffDSharp() {
    MidiNote mn(MidiNote::MiddleC + MidiNote::D + 1);
    int ll = mn.getLegerLine(false);
    assertEQ(ll, -1);
    ll = mn.getLegerLine(SharpsFlatsPref::Flats, false);
    assertEQ(ll, 0);
    ll = mn.getLegerLine(SharpsFlatsPref::Sharps, false);
    assertEQ(ll, -1);
    ll = mn.getLegerLine(SharpsFlatsPref::DontCare, false);
    assertEQ(ll, -1);
}

static void testMidiStaffE() {
    MidiNote mn(MidiNote::MiddleC + 4);  // e above middle C
    int ll = mn.getLegerLine(false);
    assertEQ(ll, 0);
    ll = mn.getLegerLine(SharpsFlatsPref::Flats, false);
    assertEQ(ll, 0);
}

static void testMidiStaffEFlat() {
    MidiNote mn(MidiNote::MiddleC + 3);  // e flat above middle C
    int ll = mn.getLegerLine(false);
    assertEQ(ll, -1);  // default to d#
    ll = mn.getLegerLine(SharpsFlatsPref::Flats, false);
    assertEQ(ll, 0);

    ll = mn.getLegerLine(SharpsFlatsPref::Sharps, false);
    assertEQ(ll, -1);

    ll = mn.getLegerLine(SharpsFlatsPref::DontCare, false);
    assertEQ(ll, -1);
}

static void testMidiStaffF() {
    MidiNote mn(MidiNote::MiddleC + MidiNote::F);  // F above middle C
    int ll = mn.getLegerLine(false);
    assertEQ(ll, 1);  // default to F
    ll = mn.getLegerLine(SharpsFlatsPref::Flats, false);
    assertEQ(ll, 1);

    ll = mn.getLegerLine(SharpsFlatsPref::Sharps, false);
    assertEQ(ll, 1);

    ll = mn.getLegerLine(SharpsFlatsPref::DontCare, false);
    assertEQ(ll, 1);
}

static void testMidiStaffFSharp() {
    MidiNote mn(MidiNote::MiddleC + MidiNote::F + 1);
    int ll = mn.getLegerLine(false);
    assertEQ(ll, 1);  // default to F#

    ll = mn.getLegerLine(SharpsFlatsPref::Flats, false);
    assertEQ(ll, 2);  // G flat

    ll = mn.getLegerLine(SharpsFlatsPref::Sharps, false);
    assertEQ(ll, 1);

    ll = mn.getLegerLine(SharpsFlatsPref::DontCare, false);
    assertEQ(ll, 1);
}

static void testMidiStaffG() {
    MidiNote mn(MidiNote::MiddleC + MidiNote::G);  // G above middle C
    int ll = mn.getLegerLine(false);
    assertEQ(ll, 2);  // default to F
    ll = mn.getLegerLine(SharpsFlatsPref::Flats, false);
    assertEQ(ll, 2);
    ll = mn.getLegerLine(SharpsFlatsPref::Sharps, false);
    assertEQ(ll, 2);
    ll = mn.getLegerLine(SharpsFlatsPref::DontCare, false);
    assertEQ(ll, 2);
}

static void testMidiStaffGSharp() {
    MidiNote mn(MidiNote::MiddleC + MidiNote::G + 1);  // G# above middle C
    int ll = mn.getLegerLine(false);
    assertEQ(ll, 2);
    ll = mn.getLegerLine(SharpsFlatsPref::Flats, false);
    assertEQ(ll, 3);
    ll = mn.getLegerLine(SharpsFlatsPref::Sharps, false);
    assertEQ(ll, 2);
    ll = mn.getLegerLine(SharpsFlatsPref::DontCare, false);
    assertEQ(ll, 2);
}

static void testMidiStaffA() {
    MidiNote mn(MidiNote::MiddleC + MidiNote::A);
    int ll = mn.getLegerLine(false);
    assertEQ(ll, 3);
    ll = mn.getLegerLine(SharpsFlatsPref::Flats, false);
    assertEQ(ll, 3);
    ll = mn.getLegerLine(SharpsFlatsPref::Sharps, false);
    assertEQ(ll, 3);
    ll = mn.getLegerLine(SharpsFlatsPref::DontCare, false);
    assertEQ(ll, 3);
}

static void testMidiStaffASharp() {
    MidiNote mn(MidiNote::MiddleC + MidiNote::A + 1);
    int ll = mn.getLegerLine(false);
    assertEQ(ll, 3);
    ll = mn.getLegerLine(SharpsFlatsPref::Flats, false);
    assertEQ(ll, 4);
    ll = mn.getLegerLine(SharpsFlatsPref::Sharps, false);
    assertEQ(ll, 3);
    ll = mn.getLegerLine(SharpsFlatsPref::DontCare, false);
    assertEQ(ll, 3);
}

static void testMidiStaffB() {
    MidiNote mn(MidiNote::MiddleC + MidiNote::B);  // G# above middle C
    int ll = mn.getLegerLine(false);
    assertEQ(ll, 4);
    ll = mn.getLegerLine(SharpsFlatsPref::Flats, false);
    assertEQ(ll, 4);
    ll = mn.getLegerLine(SharpsFlatsPref::Sharps, false);
    assertEQ(ll, 4);
    ll = mn.getLegerLine(SharpsFlatsPref::DontCare, false);
    assertEQ(ll, 4);
}

static void testMidiStaffC2() {
    MidiNote mn(MidiNote::MiddleC + 12);
    const int ll = mn.getLegerLine(false);
    assertEQ(ll, 5);
}

static void testMidiStaffC3() {
    MidiNote mn(MidiNote::MiddleC + 2 * 12);
    const int ll = mn.getLegerLine(false);
    assertEQ(ll, 12);
}

static void testMidiStaffLowA() {
    MidiNote mn(MidiNote::MiddleC - 3);
    const int ll = mn.getLegerLine(false);
    assertEQ(ll, -4);
}

static void testMidiBassStaffA() {
    MidiNote mn(MidiNote::MiddleC - 3);  // A below middle C. top of bass
    const int ll = mn.getLegerLine(true);
    assertEQ(ll, 8);
}

static void testMidiStaffAll(bool bassStaff) {
    int lastLL = 0;
    for (int i = 0; i < 128; ++i) {
        MidiNote mn(i);
        const int ll = mn.getLegerLine(bassStaff);
        if (i != 0) {
            assert((ll == lastLL) || (ll == (lastLL + 1)));
        }
        lastLL = ll;
    }
}

static void testMidiStaffX() {
    MidiNote mn(MidiNote::MiddleC);
    const int ll = mn.getLegerLine(false);
    MidiNote mn2(MidiNote::MiddleC + 2);  // D
    const int ll2 = mn2.getLegerLine(false);
    MidiNote mn3(MidiNote::MiddleC + 4);  // E
    const int ll3 = mn3.getLegerLine(false);
}

static void testMidiIsBlackKey() {
    assertEQ(MidiNote(MidiNote::C).isBlackKey(), false);
    assertEQ(MidiNote(MidiNote::C + 1).isBlackKey(), true);
    assertEQ(MidiNote(MidiNote::D).isBlackKey(), false);
    assertEQ(MidiNote(MidiNote::D + 1).isBlackKey(), true);
    assertEQ(MidiNote(MidiNote::E).isBlackKey(), false);
    assertEQ(MidiNote(MidiNote::F).isBlackKey(), false);
    assertEQ(MidiNote(MidiNote::F + 1).isBlackKey(), true);
    assertEQ(MidiNote(MidiNote::G).isBlackKey(), false);
    assertEQ(MidiNote(MidiNote::G + 1).isBlackKey(), true);
    assertEQ(MidiNote(MidiNote::A).isBlackKey(), false);
    assertEQ(MidiNote(MidiNote::A + 1).isBlackKey(), true);
    assertEQ(MidiNote(MidiNote::B).isBlackKey(), false);

    assertEQ(MidiNote(MidiNote::C + 24).isBlackKey(), false);
    assertEQ(MidiNote(MidiNote::C + 1 + 48).isBlackKey(), true);
}

//testPitchFromLeger(bool bassStaff, int legerLine, SharpsFlatsPref accidental);
static void testPitchFromLeger() {
    auto x = MidiNote::pitchFromLeger(false, -2, SharpsFlatsPref::DontCare);
    assertEQ( std::get<0>(x), true);
    assertEQ( std::get<1>(x), MidiNote::C3);
}

void testMidiNote() {
    testMidiNoteCtor();
    testMidiStaffX();

    // C on up
    testMidiStaffC();
    testMidiStaffCSharp();
    testMidiStaffD();
    testMidiStaffDSharp();
    testMidiStaffE();
    testMidiStaffEFlat();
    testMidiStaffF();
    testMidiStaffFSharp();
    testMidiStaffG();
    testMidiStaffGSharp();
    testMidiStaffA();
    testMidiStaffASharp();
    testMidiStaffB();

    testMidiStaffC2();
    testMidiStaffC3();
    testMidiStaffLowA();
    // testMidiBassStaffA();
    testMidiStaffAll(false);
    testMidiStaffAll(true);
    testMidiIsBlackKey();
    testPitchFromLeger();
}

#if 1
void testFirst() {
    //testMidiNote();
    testPitchFromLeger();
}
#endif