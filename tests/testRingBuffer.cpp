
//#include "AtomicRingBuffer.h"
#include "SqRingBuffer.h"
#include "asserts.h"

template <typename TRingBufer>
static void testConstruct() {
    TRingBufer rb;
    assert(rb.empty());
    assert(!rb.full());
    assertEQ(rb.size(), 0);
}

template <typename TRingBufer>
static void testSimpleAccess() {
    TRingBufer rb;
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
    TRingBufer rb;
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
    TRingBufer rb;
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
    TRingBufer rb;
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
    TRingBufer rb;
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
    SqRingBuffer<int, 3> rb;
    rb.push(10);
    assertEQ(rb.at(0), 10);
    rb.push(11);
    assertEQ(rb.at(0), 11);
    assertEQ(rb.at(1), 10);
}

static void testAt2() {
    SqRingBuffer<int, 3> rb;
    rb.push(10);
    rb.push(11);
    rb.pop();
    rb.push(12);
    rb.push(13);
    assertEQ(rb.at(0), 13);
    assertEQ(rb.at(1), 12);
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
    //   testOne<AtomicRingBuffer<const char *, 1 >>();
}
