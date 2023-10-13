
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
// extern void testNoteBufferSorter();
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
extern void testTempo();
extern void testGrayCode();
extern void testAdditivePitchLogic();
extern void testSines();
extern void testAdditive();
extern void testAdditiveGainLogic();
extern void testAdditiveModLogic();
extern void testFreqMeasure();

int main(const char**, int) {
    //  testGrayCode();
    //  std::exit(0);
#if 0
    specialDumpList();
#else
    testFreqMeasure();
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

    testArpegPlayer2();
    testArpegPlayer();
    testArpegRhythmPlayer();
    testArpegComposite();

#if 1
    testNotes();
    testScale();
    testScaleNotes();
    testScaleQuantizer();
    testKeysig();
    testHarmonyNote();
    testScaleRelativeNote();
    testChordRelativeNote();
    testChord();
    testChord4Manager();
    testHarmonySong();
    testHarmonyChords();
    testHarmonyChords2();
    testHarmonyChordsRandom();
#endif
    testHarmonyComposite();
    printf("put back test progression?\n");

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