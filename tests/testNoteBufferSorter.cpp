

#include "NoteBufferSorter.h"
#include "NoteBuffer.h"
#include "asserts.h"

#if 0
static void testNoteBufferSorter0() {
    NoteBuffer nb(10);
    nb.push_back(2);
    NoteBufferSorter::sortAscending(nb);
    assertEQ(*nb.begin(), 2);
} 

static void testNoteBufferSorterAlready() {
    NoteBuffer nb(10);
    nb.push_back(2);
    nb.push_back(3);
    nb.push_back(4);
    NoteBufferSorter::sortAscending(nb);
    assertEQ(*nb.begin(), 2);
    assertEQ(*(nb.begin() + 1), 3);
    assertEQ(*(nb.begin() + 2), 4);
}

 static void testNoteBufferSorterUnsorted() {
    NoteBuffer nb(10);
    nb.push_back(4);
    nb.push_back(3);
    nb.push_back(2);
    NoteBufferSorter::sortAscending(nb);
    assertEQ(*nb.begin(), 2);
    assertEQ(*(nb.begin() + 1), 3);
    assertEQ(*(nb.begin() + 2), 4);
 }

void testNoteBufferSorter() {
    testNoteBufferSorter0();
    testNoteBufferSorterAlready();
    testNoteBufferSorterUnsorted();
}
#endif
