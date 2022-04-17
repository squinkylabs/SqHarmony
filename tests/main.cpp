
#include <assert.h>
#include <malloc.h>
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
extern void testHarmonyChords2();
extern void testRingBuffer();
extern void testChord4Manager();
extern void testTempo();

int main(const char**, int) {
#if 0
    specialDumpList();
#else
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

    if (_heapchk() != _HEAPOK) {
        printf("bad heap\n");
        assert(false);
    }
    else {
        printf("mem all good\n");
    }

    int i = 0;
    _HEAPINFO hinfo;
    int heapstatus;
    hinfo._pentry = NULL;
    while ((heapstatus = _heapwalk(&hinfo)) == _HEAPOK) {
       // printf("walk %d\n", i++);
    }

    switch (heapstatus) {
    case _HEAPEMPTY:
        printf("OK - empty heap\n");
        break;
    case _HEAPEND:
        printf("OK - end of heap\n");
        break;
    case _HEAPBADPTR:
        printf("ERROR - bad pointer to heap\n");
        break;
    case _HEAPBADBEGIN:
        printf("ERROR - bad start of heap\n");
        break;
    case _HEAPBADNODE:        
        printf("ERROR - bad node in heap\n");
        break;
    default:
        printf("UNK\n");
    }

}