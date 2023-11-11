
#include "PitchKnowledge.h"
#include "SqLog.h"
#include "asserts.h"

static void testCanGetName() { 
    for (int i = 0; i < 127; ++i) {
        std::string s = PitchKnowledge::nameOfAbs(i);
        // ; Ex; C2, C#2, C# - 1
        assert(s.size() == 2 || s.size() == 3 || s.size() == 4);
    }
    std::string s = PitchKnowledge::nameOfAbs(0);
    assertEQ(s, "C-2");  // Octave names a little strange here. But risky to change this old code.
}

 static void testPitchFromName() {
  //  std::string s;
    int pitch = PitchKnowledge::pitchFromName("C2");
    assertEQ(pitch, 0);
    pitch = PitchKnowledge::pitchFromName("C#2");
    assertEQ(pitch, 1)
    pitch = PitchKnowledge::pitchFromName("C1");
    assertEQ(pitch, -12);
    pitch = PitchKnowledge::pitchFromName("C0");
    assertEQ(pitch, -12 * 2);

    pitch = PitchKnowledge::pitchFromName("C-1");
    assertEQ(pitch, -12 * 3);
 }
static void testRoundTrip() {
    assert(false);
}

void testPitchKnowledge() {
    testCanGetName();
    testPitchFromName();
    testRoundTrip();
}