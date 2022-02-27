
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
//extern void testNoteBufferSorter();
extern void testArpegPlayer();
extern void testArpegPlayer2();
extern void testArpegComposite();
extern void testArpegRhythmPlayer();
extern void testSeqClock();
extern void testGateDelay();

int main(const char**, int) {
    
#if 0
    specialDumpList();
#else
    testGateDelay();
    testSeqClock();
    testNoteBuffer();

    testArpegPlayer2();
    testArpegPlayer();
    testArpegRhythmPlayer();
   
    testArpegComposite();

#if 0
    printf("testing notes 22\n");
    testNotes();
     printf("testing scales 24\n");
    testScale();
     printf("testing scale notes 26\n");
    testScaleNotes();
    printf("testing scale q\n");
    testScaleQuantizer();

    printf("testing h note\n");
    testHarmonyNote();
     printf("testing srn\n");
    testScaleRelativeNote();
     printf("testing crn\n");
    testChordRelativeNote();
     printf("testing keysig\n");
    testKeysig();
     printf("testing chord\n");
    testChord();
     printf("testing h song\n");
    testHarmonySong();
     printf("testing h chords\n");
    testHarmonyChords();
   
    testHarmonyComposite();
#endif
#endif
}