
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

static void testCanGetRelativeName() {
    for (int i = 0; i < 127; ++i) {
        std::string s = PitchKnowledge::nameOfShort(i);
        // ; Ex; C, C#, C# - 1
        assert(s.size() == 1 || s.size() == 2 || s.size() == 4);
    }
    std::string s = PitchKnowledge::nameOfShort(0);
    assertEQ(s, "C"); 
}

static void testPitchFromName() {
    int pitch = PitchKnowledge::pitchFromName("C2");
    assertEQ(pitch, 48);
    
    pitch = PitchKnowledge::pitchFromName("C#2");
    assertEQ(pitch, 49);
    
    pitch = PitchKnowledge::pitchFromName("C1");
    assertEQ(pitch, 48 - 12);
    
    pitch = PitchKnowledge::pitchFromName("C0");
    assertEQ(pitch, 48 - 12 * 2);

    pitch = PitchKnowledge::pitchFromName("C-1");
    assertEQ(pitch, 48 - 12 * 3);
}
static void testRoundTrip() {
    for (int i = 0; i < 127; ++i) {
        std::string s = PitchKnowledge::nameOfAbs(i);
        const int pitch = PitchKnowledge::pitchFromName(s);
        assertEQ(pitch, i);
    }
}

void testPitchKnowledge() {
    testCanGetName();
    testCanGetRelativeName();
    testPitchFromName();
    testRoundTrip();
}