
//#include "AtomicRingBuffer.h"
#include "SqRingBuffer.h"
#include "SqRingBuffer2.h"
#include "asserts.h"

template <typename TRingBufer>
static void testConstruct() {
    TRingBufer rb(false);
    assert(rb.empty());
    assert(!rb.full());
    assertEQ(rb.size(), 0);
}

template <typename TRingBufer>
static void testSimpleAccess() {
    TRingBufer rb(false);
    rb.push(55);
    assert(!rb.empty());
    assert(!rb.full());
    assertEQ(rb.size(), 1);

    int x = rb.pop();
    assertEQ(x, 55);

    assert(rb.empty());
    assert(!rb.full());
}

template <typename TRingBufer>
static void testMultiAccess() {
    TRingBufer rb(false);
    rb.push(1234);
    rb.push(5678);
    assert(!rb.empty());
    assert(!rb.full());
    assertEQ(rb.size(), 2);

    int x = rb.pop();
    assertEQ(x, 1234);
    assertEQ(rb.size(), 1);

    assert(!rb.empty());
    assert(!rb.full());

    x = rb.pop();
    assertEQ(x, 5678);

    assert(rb.empty());
    assert(!rb.full());
    assertEQ(rb.size(), 0);
}

template <typename TRingBufer>
static void testWrap() {
    TRingBufer rb(false);
    rb.push(1234);
    rb.push(5678);
    rb.pop();
    rb.pop();
    assertEQ(rb.size(), 0);
    rb.push(1);
    rb.push(2);
    rb.push(3);
    assertEQ(rb.size(), 3);

    assertEQ(rb.pop(), 1);
    assertEQ(rb.pop(), 2);
    assertEQ(rb.pop(), 3);

    assert(rb.empty());
    assertEQ(rb.size(), 0);
}

template <typename TRingBufer>
static void testFull() {
    TRingBufer rb(false);
    rb.push(1234);
    rb.push(5678);
    rb.pop();
    rb.pop();
    rb.push(1);
    rb.push(2);
    rb.push(3);
    rb.push(4);
    assert(rb.full());
    assert(!rb.empty());
    assertEQ(rb.size(), 4);

    assertEQ(rb.pop(), 1);
    assertEQ(rb.size(), 3);
    assertEQ(rb.pop(), 2);
    assertEQ(rb.pop(), 3);
    assertEQ(rb.pop(), 4);

    assert(rb.empty());
    assert(!rb.full());
    assertEQ(rb.size(), 0);
}

template <typename TRingBufer>
static void testOne() {
    const char *p = "foo";
    TRingBufer rb(false);
    rb.push(p);
    assert(!rb.empty());
    assert(rb.full());
    assertEQ(rb.size(), 1);

    assertEQ(rb.pop(), p);
    assert(rb.empty());
    assert(!rb.full());
}

template <typename TRingBuffer>
void _testRingBuffer() {
    testConstruct<TRingBuffer>();
    testSimpleAccess<TRingBuffer>();
    testMultiAccess<TRingBuffer>();
    testWrap<TRingBuffer>();
    testFull<TRingBuffer>();
}

static void testAt() {
    SqRingBuffer<int, 3> rb(false);
    rb.push(10);
    assertEQ(rb.at(0), 10);
    rb.push(11);
    assertEQ(rb.at(0), 11);
    assertEQ(rb.at(1), 10);
}

static void testAt2() {
    SqRingBuffer<int, 3> rb(false);
    rb.push(10);
    rb.push(11);
    rb.pop();
    rb.push(12);
    rb.push(13);
    assertEQ(rb.at(0), 13);
    assertEQ(rb.at(1), 12);
}

static void testOverflow() {
    SqRingBuffer<int, 3> rb(true);
    rb.push(10);
    rb.push(11);
    rb.push(12);
    rb.push(13);
    assertEQ(rb.size(), 3);
    assertEQ(rb.at(0), 13);
    assertEQ(rb.at(1), 12);
    assertEQ(rb.at(2), 11);
}

static void testChordHistory() {
    SqChordHistory h;
    h.onNewChord(200, 1);
    assert(h.haveSeen(200, 1));
    assert(!h.haveSeen(201, 1));
    assert(!h.haveSeen(200, 2));
}

static void testChordHistoryOverflow() {
    SqChordHistory h;  // default only 4
    h.onNewChord(200, 1);
    h.onNewChord(200, 2);
    h.onNewChord(200, 3);
    h.onNewChord(200, 4);
    h.onNewChord(200, 5);

    assert(h.haveSeen(200, 5));
    assert(h.haveSeen(200, 4));
    assert(h.haveSeen(200, 3));
    assert(h.haveSeen(200, 2));

    // this one pushed out (overflow)
    assert(!h.haveSeen(200, 1));
}

static void testRingBuffer2basic() {
    SqRingBuffer2 rb(false, 4);
    assert(rb.empty());
    assert(!rb.full());
}

static void testRingBuffer2basic2() {
    SqRingBuffer2 rb(false, 4);
    rb.push(2);
    assert(!rb.empty());
    assert(!rb.full());
}

static void testRingBuffer2basic3() {
    SqRingBuffer2 rb(false, 4);
    rb.push(1);
    rb.push(2);
    rb.push(3);
    assert(!rb.full());
    rb.push(4);
    assert(rb.full());
    assert(!rb.empty());
    assertEQ(rb.size(), 4);
}

static void testRingBuffer2basic4() {
    SqRingBuffer2 rb(false, 4);
    rb.push(1);
    rb.push(2);
    assertEQ(rb.pop(), 1);
    rb.push(3);
    rb.push(4);
    assertEQ(rb.pop(), 2);
    assertEQ(rb.pop(), 3);
    assertEQ(rb.size(), 1);
}

static void testRingBuffer2Wrap() {
    SqRingBuffer2 rb(false, 4);
    rb.push(1);
    assertEQ(rb.size(), 1);
    rb.push(2);
    assertEQ(rb.size(), 2);
    rb.push(3);
    assertEQ(rb.size(), 3);
    rb.pop();
    assertEQ(rb.size(), 2);
    rb.push(4);
    assertEQ(rb.size(), 3);
    assert(!rb.full());

    rb.push(5);
    assertEQ(rb.size(), 4);
    auto i = rb.size();
    assert(rb.full());
}

static void testRingBuffer2ChangeSize() {
    SqRingBuffer2 rb(false, 4);
    rb.setSize(1);
    assert(rb.empty());
    assert(!rb.full());
    rb.push(0);
    assert(!rb.empty());
    assert(rb.full());
}

static void testRingBuffer2ChangeSize2() {
    SqRingBuffer2 rb(false, 4);
    rb.push(0);
    rb.push(0);
    rb.push(0);
    rb.setSize(1);
    assert(!rb.empty());
    assert(rb.full());
    assertEQ(rb.size(), 1);
}

static void test2At() {
    SqRingBuffer2 rb(false, 3);
    rb.push(10);
    assertEQ(rb.at(0), 10);
    rb.push(11);
    assertEQ(rb.at(0), 11);
    assertEQ(rb.at(1), 10);
}

static void test2At2() {
    SqRingBuffer2 rb(false, 4);
    rb.push(10);
    rb.push(11);
    rb.pop();
    rb.push(12);
    rb.push(13);
    assertEQ(rb.at(0), 13);
    assertEQ(rb.at(1), 12);
}

static void test2Overflow() {
    SqRingBuffer2 rb(true, 3);
    rb.push(10);
    rb.push(11);
    rb.push(12);
    rb.push(13);
    assertEQ(rb.size(), 3);
    assertEQ(rb.at(0), 13);
    assertEQ(rb.at(1), 12);
    assertEQ(rb.at(2), 11);
}

static void testRingBuffer2Size1() {
    SqRingBuffer2 rb(true, 3);
    rb.setSize(1);
    assert(!rb.full());
    assert(rb.empty());
    rb.push(10);
    assert(rb.full());
    assert(!rb.empty());
    assertEQ(rb.pop(), 10);
    assert(!rb.full());
    assert(rb.empty());

    rb.push(11);
    assert(rb.full());
    assertEQ(rb.pop(), 11);

    rb.push(12);
    assert(rb.full());
    assertEQ(rb.pop(), 12);
}

void testRingBuffer() {
    testConstruct<SqRingBuffer<int, 4>>();
    testConstruct<SqRingBuffer<char *, 1>>();
    //  testConstruct<AtomicRingBuffer<int, 4>>();
    // testConstruct<AtomicRingBuffer<char *, 1>>();

    _testRingBuffer<SqRingBuffer<int, 4>>();
    //   _testRingBuffer<AtomicRingBuffer<int, 4>>();

    testOne<SqRingBuffer<const char *, 1>>();
    testAt();

    testAt2();
    testOverflow();
    //   testOne<AtomicRingBuffer<const char *, 1 >>();

    testRingBuffer2basic();
    testRingBuffer2basic2();
    testRingBuffer2basic3();

    testRingBuffer2Wrap();
    testRingBuffer2ChangeSize();
    testRingBuffer2ChangeSize2();
    test2At();
    test2At2();
    test2Overflow();
    testRingBuffer2basic4();
    testRingBuffer2Size1();

    testChordHistory();
    testChordHistoryOverflow();
}
