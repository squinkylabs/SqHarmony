
#include <assert.h>
#include <stdio.h>

#include <cstdlib>
extern void testHarmonyNote();
extern void testScaleRelativeNote();
extern void testChord();
extern void testChordRelativeNote();
extern void testHarmonySong();
extern void testHarmonyChords();
extern void testScaleQuantizer();
extern void testNotes();
extern void testScale();
extern void testScaleNotes();
extern void testHarmonyComposite();
extern void testKeysig();

extern void specialDumpList();
extern void testProgressions();
extern void testNoteBuffer();
extern void testArpegPlayer();
extern void testArpegPlayer2();
extern void testArpegComposite();
extern void testArpegRhythmPlayer();
extern void testSeqClock();
extern void testGateDelay();
extern void testHarmonyChordsRandom();
extern void testHarmonyChords2();
extern void testRingBuffer();
extern void testChord4Manager();
// All the above are used.
extern void testTempo();
extern void testGrayCode();
extern void testAdditivePitchLogic();
extern void testSines();
extern void testAdditive();
extern void testAdditiveGainLogic();
extern void testAdditiveModLogic();
extern void testFreqMeasure();
extern void testFreqMeasure2();
extern void testResettablePhaseAccumulator();
extern void testOneShot();
extern void testPhasePatterns();
extern void testPhasePatternsPoly();
extern void testOneShotSampleTimer();

extern void testSchmidtTrigger();

extern void testShiftCalc();
extern void testClockMult();
extern void testMultiplier();
extern void testRatchet();
extern void testAllChords(bool);
extern void testHarmonyChords2023();
extern void testPitchKnowledge();
extern void testRawChordGenerator();
extern void testChord4();
extern void testNumberFormatter();
extern void testTwister();
extern void testFirst();
extern void testNoiseGen();
extern void testLFNDsp();
extern void testButter();
extern void testPinkFilter();
extern void testTestLFO();
extern void testClockShifter6();
extern void testClockShifter6d();
extern void testBitDelay();
extern void testBitDelayPartTwo();
extern void testHarmony2();
extern void testHarmony2B();
extern void testHarmony2C();
extern void testSharpFlatMonitor();
extern void testUpdater();
extern void testShiftMath();
extern void testAudioMath();
extern void testDifferenticalClockCounter();
extern void testDeGlitch();
extern void testDeGlitch();
extern void testChordRecognizer();
extern void testChordRecognizer2();
extern void testChordRecognizer3();
extern void testVisualizer();
extern void testPESConverter();
extern void testScorePitchUtils();
extern void testLegerScore();
extern void testNotationNote();
extern void testMidiNote();
extern void testSqArray();
extern void testShiftingArray();
extern void testSharpsFlatsPref();
extern void testScoreDrawUtils();

#include "SqLog.h"

#ifdef _DEBUG
bool doLongRunning = false;
#else
bool doLongRunning = true;
#endif

int logLevel = 0;

int main(const char**, int) {
    //  testGrayCode();
    //  std::exit(0);
#if 0
    specialDumpList();
#else

    if (doLongRunning) {
        SQINFO("doing special long run");
    }

#if 0
    printf("running only one for temp debug\n");
    //assert(false);
    assert(SqLog::errorCount == 0);
    testFirst();
    std::exit(0);
#endif
    testAudioMath();
    testUpdater();

    testTwister();
    testPinkFilter();
    testNoiseGen();
    testButter();
    testLFNDsp();
    testSchmidtTrigger();
    testNumberFormatter();
    testShiftMath();
    testTestLFO();
    testOneShot();
    testOneShotSampleTimer();
    testResettablePhaseAccumulator();
    testFreqMeasure();
    testFreqMeasure2();
    testShiftCalc();

    testDifferenticalClockCounter();
    testSharpsFlatsPref();
    testNotationNote();
    testScorePitchUtils();
    testChordRecognizer3();
    testChordRecognizer();
    testChordRecognizer2();
    testPESConverter();
    testSharpFlatMonitor();
    testVisualizer();
    testScoreDrawUtils();

    testClockMult();
    testMultiplier();
    testRatchet();
    testBitDelay();
    testBitDelayPartTwo();
    testDeGlitch();
    testClockShifter6();
    testClockShifter6d();

    testPhasePatterns();
    testPhasePatternsPoly();

  
    testSqArray();
    testShiftingArray();
    testNotes();
    testLegerScore();
    testScaleNotes();
    testScale();
    testMidiNote();
    testScaleRelativeNote();
    testScaleQuantizer();
    testHarmony2();
    testHarmony2B();
    testHarmony2C();

    testHarmonyNote();
    testLegerScore();
#if 0
    printf("XX: leaving early for clock stuff");
    std::exit(0);
#endif

    testAdditiveModLogic();
    testAdditivePitchLogic();
    testAdditiveGainLogic();
    testSines();
    testAdditive();
    testTempo();
    testRingBuffer();
    testGateDelay();
    testSeqClock();
    testNoteBuffer();
    testKeysig();

    testArpegPlayer2();
    testArpegPlayer();
    testArpegRhythmPlayer();
    testArpegComposite();
#if 1
    testRawChordGenerator();
    testPitchKnowledge();
    testChord4();
    testChord();
    testChordRelativeNote();
    testHarmonyChords();
    testHarmonyChords2();
    testHarmonyChords2023();

    // fprintf(stderr, "skipping random for now\n");
    testHarmonyChordsRandom();
    testAllChords(doLongRunning);
#endif
    testHarmonyComposite();
    testProgressions();
    testChord4Manager();
    // printf("put back test progression?\n");

#endif
}

#include <nanovg.h>

// crazy linker issue
#if !defined(__PLUGIN)
NVGcolor nvgRGB(unsigned char r, unsigned char g, unsigned char b) {
    return nvgRGBA(r, g, b, 255);
}

NVGcolor nvgRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    NVGcolor color;
    // Use longer initialization to suppress warning.
    color.r = r / 255.0f;
    color.g = g / 255.0f;
    color.b = b / 255.0f;
    color.a = a / 255.0f;
    return color;
}
#endif