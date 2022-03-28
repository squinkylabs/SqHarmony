
#include <stdio.h>
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
extern void testRingBuffer();

int main(const char**, int) {
#if 0
    specialDumpList();
#else
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
    testHarmonySong();
    testHarmonyChords();
    testHarmonyChordsRandom();
#endif
    testHarmonyComposite();
    printf("put back test progression?\n");

#endif
}