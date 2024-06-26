
#include <limits>

#include "BitDelay2.h"
#include "asserts.h"

template <typename T>
static void testCanCall() {
    T delay;
    // delay.setMaxDelaySamples(100);
    //  delay.getMaxDelaySize();
    delay.process(false, 0);
}

// template <typename T>
// static void testDelaySize() {
//     T delay;
//     delay.setMaxDelaySamples(100);
//     const auto x = delay.getMaxDelaySize();
//     assertGE(x, 100);
// }

template <typename T>
static void testDelayA() {
    T delay;
    typename T::Errors err;
    // delay.setMaxDelaySamples(100);
    bool b = delay.process(false, 0, &err);
    assertEQ(b, false);
}

template <typename T>
static void testDelayB() {
    T delay;
    typename T::Errors err;
    // delay.setMaxDelaySamples(100);
    bool b = delay.process(true, 0, &err);
    assertEQ(b, true);
}

template <typename T>
static void testDelayC() {
    T delay;
    typename T::Errors err;
    //  delay.setMaxDelaySamples(100);

    delay.process(true, 1, &err);
    bool b = delay.process(true, 1, &err);
    assertEQ(b, true);
}

template <typename T>
static void testDelayD() {
    T delay;
    typename T::Errors err;
    //  delay.setMaxDelaySamples(100);

    delay.process(true, 1, &err);
    // since there is nothing in the buffer, we should get zero past end
    bool b = delay.process(false, 1, &err);
    assertEQ(b, true);
}

template <typename T>
static void testDelayE() {
    T delay;
    typename T::Errors err;
    delay.setMaxDelaySamples(100);

    // since there is nothing in the buffer, we should get zero past end
    bool b = delay.process(true, 1, &err);
    assertEQ(b, false);
}

template <typename T>
static void testDelay0() {
    T delay;
    typename T::Errors err;
    bool b = delay.process(true, 0, &err);
    assertEQ(b, true);
    assert(err == T::Errors::NoError);

    b = delay.process(false, 0, &err);
    assertEQ(b, false);
    assert(err == T::Errors::NoError);

    b = delay.process(true, 0, &err);
    assertEQ(b, true);
    assert(err == T::Errors::NoError);

    b = delay.process(true, 0, &err);
    assertEQ(b, true);
    assert(err == T::Errors::NoError);
}

template <typename T>
static void testDelay20() {
    T delay;
    typename T::Errors err;
    bool b = delay.process(true, 20, &err);
    assertEQ(b, false);
    assert(err == T::Errors::NoError);

    b = delay.process(false, 20, &err);
    assertEQ(b, false);
    assert(err == T::Errors::NoError);

    b = delay.process(true, 20, &err);
    assertEQ(b, false);
    assert(err == T::Errors::NoError);

    b = delay.process(true, 20, &err);
    assertEQ(b, false);
    assert(err == T::Errors::NoError);

    // ok, have done four cycles. So doing 15
    // more should get use to 20, the test.
    for (int i = 0; i < 16; ++i) {
        b = delay.process(true, 20, &err);
        assertEQ(b, false);
        assert(err == T::Errors::NoError);
    }

    b = delay.process(false, 20, &err);
    assertEQ(b, true);
    assert(err == T::Errors::NoError);

    b = delay.process(false, 20, &err);
    assertEQ(b, false);
    assert(err == T::Errors::NoError);

    b = delay.process(false, 20, &err);
    assertEQ(b, true);
    assert(err == T::Errors::NoError);

    b = delay.process(false, 20, &err);
    assertEQ(b, true);
    assert(err == T::Errors::NoError);
}

class TestX {
public:
    static void testAdvance2A() {
        BitDelay2 delay;
        const auto orig = delay._delayPointer;
        delay._advance();
        assertLT(orig, delay._delayPointer);
    }

    static void testAdvance2B() {
        BitDelay2 delay;
        delay._delayPointer = delay._delayMemory + (BitDelay2::_defaultDelaySize - 1);
        // write, so make sure bounds checker is happy
        delay._delayPointer->count = 123456;
        delay._advance();
        assertEQ(delay._delayPointer, delay._delayMemory);
    }

    static void testDecrement2A() {
        BitDelay2 delay;
        delay._advance();
        delay._advance();
        delay._decrement();
        assertEQ(delay._delayPointer, delay._delayMemory + 1);
    }

    static void testDecrement2B() {
        BitDelay2 delay;
        delay._decrement();
        assertEQ(delay._delayPointer, delay._delayMemory + (BitDelay2::_defaultDelaySize - 1));
    }

    static void testWriteRingBuffer() {
        BitDelay2 delay;
        delay._currentCount = 5;
        delay._currentValue = true;
        delay._writeToRingBuffer();
        assertEQ(delay._delayPointer, delay._delayMemory + 1);
        assertEQ(delay._validDelayEntries, 1);
    }
};

template <typename T>
class BitDelayTest {
public:
    static void test() {
        testCanCall<T>();
        // testDelaySize<T>();

        testDelayA<T>();
        testDelayB<T>();
        testDelayC<T>();
        testDelayD<T>();

        testDelay0<T>();
        testDelay20<T>();
    }
};

void testBitDelay2Only() {
    TestX::testAdvance2A();
    TestX::testAdvance2B();
    TestX::testDecrement2A();
    TestX::testDecrement2B();
    TestX::testWriteRingBuffer();
}

static void testZeroDelay() {
    BitDelay2 delay;
    bool b = delay.process(false, 0);
    assertEQ(b, false);
    b = delay.process(false, 0);
    assertEQ(b, false);

    b = delay.process(true, 0);
    assertEQ(b, true);
    b = delay.process(true, 0);
    assertEQ(b, true);
}

static void testMovingOne() {
    BitDelay2 delay;
    bool b = delay.process(true, 0);
    assertEQ(b, true);
    b = delay.process(false, 1);
    assertEQ(b, true);
    b = delay.process(false, 2);
    assertEQ(b, true);
    b = delay.process(false, 3);
    assertEQ(b, true);
}

static void testWayPastEnd() {
    BitDelay2 delay;

    // put a little stuff in there
    delay.process(true, 0);
    delay.process(false, 0);
    delay.process(true, 0);

    bool b = delay.process(true, std::numeric_limits<unsigned>::max());
    assertEQ(b, false);
    b = delay.process(true, 100);
    assertEQ(b, false);
}

static void testPool() {
    BitDelay2 delay;
    delay.process(true, 0);
    delay.process(true, 0);
    delay.process(true, 0);
    // now delay has true2, true1, true0.

    // after this delay has true3, true2, true1, true0.
    bool b = delay.process(true, 1);
    assertEQ(b, true);
    b = delay.process(true, 2);
    assertEQ(b, true);
    b = delay.process(true, 3);
    assertEQ(b, true);
    b = delay.process(true, 4);
    assertEQ(b, true);
}

static void testAlt() {
    BitDelay2 delay;
    bool b;

    unsigned base = 0;
    for (int i = 0; i < 10; ++i) {
        b = delay.process(false, 0);  //
        assertEQ(b, false);
        b = delay.process(true, 1);
        assertEQ(b, false);

        b = delay.process(false, 1);
        assertEQ(b, true);

        b = delay.process(false, 2);
        assertEQ(b, true);
    }
    //  true, 3 > f
    //   */
    // }
}

static void testFT() {
    BitDelay2 delay;
    bool b;
    delay.process(false, 0);
    delay.process(true, 0);

    // now delay is false | true   (old to new)
    // so delay[0] = f
    // delay[1] = t
    //
    // Or, in internals state: current value = t, count = 1
    // memory[0] = f:1
    // extract the first false. It's one back and it's delayed one, so 2
    b = delay.process(false, 1 + 1);
    assertEQ(b, false);

    // now delay is false | true | false
    // current value = f, count = 1
    // memory[0] = f:1
    // memory[1] = t:1

    // now extract the true
    b = delay.process(false, 1 + 1);
    assertEQ(b, true);

    b = delay.process(false, 4);
    assertEQ(b, false);

    for (int i = 0; i < 5; ++i) {
        b = delay.process(false, 2 + i);
        assertEQ(b, false);
    }
}

static void testTFFFF() {
    BitDelay2 delay;
    bool b;
    delay.process(true, 0);
    delay.process(false, 0);
    delay.process(false, 0);
    delay.process(false, 0);
    delay.process(false, 0);

    b = delay.process(false, 5);
    assertEQ(b, true);

    b = delay.process(false, 5);
    assertEQ(b, false);
}

static void testDelayByTwo() {
    BitDelay2 delay;
    bool b;
    b = delay.process(true, 2);
    assertEQ(b, false);
    b = delay.process(false, 2);
    assertEQ(b, false);
    b = delay.process(false, 2);
    assertEQ(b, true);
    b = delay.process(true, 2);
    assertEQ(b, false);
    b = delay.process(false, 2);
    assertEQ(b, false);
    b = delay.process(false, 2);
    assertEQ(b, true);

    b = delay.process(false, 2);
    assertEQ(b, false);
    b = delay.process(false, 2);
    assertEQ(b, false);
    b = delay.process(false, 2);
    assertEQ(b, false);
}

static void testLongTerm() {
    BitDelay2 delay;
    bool b;
    delay.setRLESize(4);  // Make it really short. Will input more than this.

    b = delay.process(true, 1);
    assertEQ(b, false);

    b = delay.process(false, 1);
    assertEQ(b, true);
    b = delay.process(true, 1);
    assertEQ(b, false);
    b = delay.process(false, 1);
    assertEQ(b, true);
    b = delay.process(true, 1);
    assertEQ(b, false);
    b = delay.process(false, 1);
    assertEQ(b, true);
    b = delay.process(true, 1);
    assertEQ(b, false);
    b = delay.process(false, 1);
    assertEQ(b, true);
}

static void testPastEnd() {
    BitDelay2 delay;
    bool b;

    delay.process(false, 0);
    delay.process(false, 0);
    delay.process(false, 0);
    delay.process(false, 0);

    delay.process(true, 0);
    delay.process(true, 0);
    delay.process(true, 0);
    delay.process(true, 0);

    // get the first one past end
    b = delay.process(true, 9);
    assertEQ(b, false);
}

static void testPastEnd2() {
    BitDelay2 delay;
    bool b;

    delay.process(true, 0);
    delay.process(true, 0);
    delay.process(false, 0);
    delay.process(false, 0);

    // get the first one past end
    b = delay.process(true, 5);
    assertEQ(b, false);
}

static void testAtEnd() {
    BitDelay2 delay;
    bool b;

    delay.process(true, 0);
    delay.process(true, 0);
    delay.process(false, 0);
    delay.process(false, 0);

    // get the last one at
    b = delay.process(true, 4);
    assertEQ(b, true);
}

void testBitDelay() {
    testBitDelay2Only();
    BitDelayTest<BitDelay2>::test();
    testZeroDelay();
    testMovingOne();
    testWayPastEnd();
    testPool();
    testAlt();
    testFT();
    testTFFFF();
    testDelayByTwo();
    testLongTerm();
    testPastEnd();
    testPastEnd2();
    testAtEnd();
}

#if 0
void testFirst() {
    SQINFO("----test");
    // testAlt();
    //  testFT();
    //  testFT();
    // testTFFFF();
    //   testBitDelay();
    //  testDelayByTwo();
    //testLongTerm();
    testAtEnd();
    SQINFO("----end");
}
#endif