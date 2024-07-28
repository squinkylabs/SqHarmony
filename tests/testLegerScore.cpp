

#include "NotationNote.h"
#include "Scale.h"
#include "ScorePitchUtils.h"


#include "asserts.h"




// inline std::tuple<ScaleNote, ScorePitchUtils::Accidental> ScorePitchUtils::getNotationNote(const Scale& scale, const MidiNote& midiNote) {

static void test1() {
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Minor);
    MidiNote mn(75);

    // This should give us the second degree of C minor, unadjusted, whic is E flat.
    const auto notationNote = ScorePitchUtils::getNotationNote(scale, mn, false);
  //  const auto scaleNote = notationNote._scaleNote;
    const auto accidental = notationNote._accidental;

// this one gives us E flat, which is the normal second degree.
// Q, but how would be know what leger line to draw this on?
   // assertEQ(scaleNote.getDegree(), 2);
  //  assert(scaleNote.getAdjustment() == ScaleNote::RelativeAdjustment::none);

    // so we expect to be on E, in treble clef.
    const auto legerLine = mn.getLegerLine(ResolvedSharpsFlatsPref::Sharps, false);
    assertEQ(legerLine, 0);
   
}

void testLegerScore() {
    //test1();
}


#if 0
void testFirst() {
    test1();
}
#endif