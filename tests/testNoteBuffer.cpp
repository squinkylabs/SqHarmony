
#include "NoteBuffer.h"
#include "asserts.h"

static void consistent(NoteBuffer& nb) {
    assertEQ(nb.empty(), (nb.size() == 0));
    assertEQ(nb.end() - nb.begin(), nb.size());
}

static void testNoteBuffer0() {
    NoteBuffer nb(1);
    assertEQ(nb.getCapacity(), 1);

    nb.setCapacity(2);
    nb.onChange([](const NoteBuffer* nbcb) {

    });
    assert(nb.empty());
    assertEQ(nb.size(), 0);
    consistent(nb);
}

static void testNoteBufferSize() {
    NoteBuffer nb(21);

    assertEQ(nb.getCapacity(), 21);
    consistent(nb);
    nb.setCapacity(11);
    assertEQ(nb.getCapacity(), 11);
}

static void testNoteBufferSizeCB() {
    NoteBuffer nb(10);
    int callbackCount = 0;
    nb.onChange([&callbackCount](const NoteBuffer* nbcb) {
        ++callbackCount;
    });

    nb.setCapacity(4);
    assertEQ(callbackCount, 1);
    consistent(nb);
}

static void testNoteBufferSizeCB2() {
    NoteBuffer nb(4);
    int callbackCount = 0;
    nb.onChange([&callbackCount, &nb](const NoteBuffer* nbcb) {
        assertEQ(nbcb, &nb);
        ++callbackCount;
    });

    nb.setCapacity(4);
    assertEQ(callbackCount, 0);
    consistent(nb);
}

static void testNoteBufferSizeCB3() {
    NoteBuffer nb(4);
    int callbackCount = 0;
    nb.onChange([&callbackCount](const NoteBuffer* nbcb) {
        ++callbackCount;
    });

    nb.push_back(0, 0);
    assertEQ(callbackCount, 1);
    consistent(nb);
}

static void testNoteBufferSizeCB4() {
    NoteBuffer nb(4);
    int callbackCount = 0;
    nb.onChange([&callbackCount](const NoteBuffer* nbcb) {
        ++callbackCount;
    });

    nb.push_back(0, 0);
    nb.removeForChannel(0);
    assert(nb.empty());
    assertEQ(callbackCount, 2);
}

static void testNoteBufferSize2() {
    NoteBuffer nb(2);
    nb.setCapacity(NoteBuffer::maxCapacity + 1);
    assertEQ(nb.getCapacity(), NoteBuffer::maxCapacity);
    consistent(nb);
}

static void testNoteBufferDelay() {
    NoteBuffer nb(10);
    nb.push_back(12, 9999);
    assertEQ(nb.size(), 1);
    consistent(nb);
}

static void testNoteBufferDelay2() {
    NoteBuffer nb(10);
    nb.push_back(12, 9999);
    nb.push_back(1, 123245);
    assertEQ(nb.size(), 2);
    consistent(nb);
    assertEQ(nb.begin()->cv, 12);
    assertEQ((nb.begin() + 1)->cv, 1);
}

static void testNoteBufferOverflow() {
    NoteBuffer nb(2);
    nb.push_back(1, 33);
    nb.push_back(2, 33);
    nb.push_back(3, 33);
    assertEQ(nb.size(), 2);
    assertEQ(nb.begin()->cv, 2);
    assertEQ((1 + nb.begin())->cv, 3);
}

static void testNoteBufferPushTwo() {
    NoteBuffer nb(5);
    nb.push_back(1, 33);
    nb.push_back(2, 33);
    nb.push_back(3, 44);
    assertEQ(nb.size(), 3);
    assertEQ(nb.begin()->cv, 1);
    assertEQ((1 + nb.begin())->cv, 2);
    assertEQ((2 + nb.begin())->cv, 3);
}

static void testNoteBufferRemove() {
    NoteBuffer nb(5);
    nb.push_back(1, 33);
    assertEQ(nb.size(), 1);
    nb.removeForChannel(33);
    assertEQ(nb.size(), 0);
}

static void testNoteBufferHold() {
    NoteBuffer nb(5);
    nb.setHold(true);
    nb.push_back(1, 55);
    assertEQ(nb.size(), 1);
    nb.removeForChannel(55);
    assertEQ(nb.size(), 1);
}

static void testNoteBufferHoldOff() {
    NoteBuffer nb(5);
    nb.setHold(true);
    nb.push_back(1, 7);
    nb.push_back(2, 8);
    assertEQ(nb.size(), 2);
    nb.removeForChannel(7);
    nb.removeForChannel(8);
    assertEQ(nb.size(), 2);
}

static void testNoteBufferRemoveStart() {
    NoteBuffer nb(5);
    nb.push_back(1, 9);
    nb.push_back(2, 10);
    nb.removeForChannel(9);
    assertEQ(nb.size(), 1);
    assertEQ(nb.begin()->cv, 2);
}

static void testNoteBufferRemoveMiddle() {
    NoteBuffer nb(5);
    nb.push_back(1, 91);
    nb.push_back(2, 92);
    nb.push_back(3, 93);

    nb.removeForChannel(92);
    assertEQ(nb.size(), 2);
    assertEQ(nb.begin()->cv, 1);
    assertEQ((1 + nb.begin())->cv, 3);
}

static void testNoteBufferRemoveEnd() {
    NoteBuffer nb(5);
    nb.push_back(1, 11);
    nb.push_back(2, 12);
    nb.push_back(3, 13);

    nb.removeForChannel(13);
    assertEQ(nb.size(), 2);
    assertEQ(nb.begin()->cv, 1);
    assertEQ((1 + nb.begin())->cv, 2);
}

#if 0
static void testNoteBufferFindMedian() {
    NoteBuffer nb(5);
    nb.push_back(1, 11);
    nb.push_back(2, 11);
    nb.push_back(3, 11);
    nb.push_back(4, 11);
    nb.push_back(5, 11);

    const int idx = nb.findMedian();
    assertEQ(idx, 2);
}
#endif

void testNoteBuffer() {
    testNoteBuffer0();
    testNoteBufferSize();
    testNoteBufferSizeCB();
    testNoteBufferSizeCB2();
    testNoteBufferSizeCB3();
    testNoteBufferSizeCB4();
    testNoteBufferSize2();
    testNoteBufferDelay();

    testNoteBufferPushTwo();
    testNoteBufferOverflow();
    testNoteBufferRemove();
    testNoteBufferHold();
    testNoteBufferHoldOff();
    testNoteBufferRemoveStart();
    testNoteBufferRemoveMiddle();
    testNoteBufferRemoveEnd();

  //  testNoteBufferFindMedian();
}