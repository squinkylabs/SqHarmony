
#include "ScorePitchUtils.h"


//#include "asserts.h"

static void test() {
   // ScorePitchUtils sp;
   Scale sc;
   MidiNote mn;
   ScorePitchUtils::getNotationNote(sc, mn);
}

void testScorePitchUtils() {
    test();
  
}

#if 1
void testFirst() {
    testScorePitchUtils();
}
#endif