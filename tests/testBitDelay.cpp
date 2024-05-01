
//#include "BitDelay.h"
#include "BitDelay2.h"
#include "asserts.h"

template <typename T>
static void testCanCall() {
    T delay;
    delay.setMaxDelaySamples(100);
    delay.getMaxDelaySize();
    delay.process(false, 0);
}

template <typename T>
static void testDelaySize() {
    T delay;
    delay.setMaxDelaySamples(100);
    const auto x = delay.getMaxDelaySize();
    assertGE(x, 100);
}

template <typename T>
static void testDelayA() {
    T delay;
    typename T::Errors err;
    delay.setMaxDelaySamples(100);
    bool b = delay.process(false, 0, &err);
    assertEQ(b, false);
}

template <typename T>
static void testDelayB() {
    T delay;
    typename T::Errors err;
    delay.setMaxDelaySamples(100);
    bool b = delay.process(true, 0, &err);
    assertEQ(b, true);
}

template <typename T>
static void testDelayC() {
    T delay;
    typename T::Errors err;
    delay.setMaxDelaySamples(100);

    delay.process(true, 1, &err);
    bool b = delay.process(true, 1, &err);
    assertEQ(b, true);
}

template <typename T>
static void testDelayD() {
    T delay;
    typename T::Errors err;
    delay.setMaxDelaySamples(100);

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
    delay.setMaxDelaySamples(100);
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
    delay.setMaxDelaySamples(100);
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
#if 0
    static void canAccessDelayAtZero() {
        BitDelay delay;
        delay.setMaxDelaySamples(0);
        delay._insertDelayInput(false);
        bool x = delay._getDelayOutput(0);
        assertEQ(x, false);

        delay._insertDelayInput(true);
        x = delay._getDelayOutput(0);
        assertEQ(x, true);
    }

    // one fixed, simple case.
    static void canAccessDelay2() {
        BitDelay delay;
        delay.setMaxDelaySamples(21);
        delay._currentLocation = 1;  // let's work on the second bit

        delay._insertDelayInput(true);

        bool x = delay._getDelayOutput(0);  // read it back with no dely
        assertEQ(x, true);
    }

    static void canAccessDelay3() {
        BitDelay delay;
        int delayAmount = 40;  // a nice uneven values
        delay.setMaxDelaySamples(100);
        delay._currentLocation = delayAmount;  // let's work on the second bit

        delay._insertDelayInput(true);

        bool x = delay._getDelayOutput(0);  // read it back with no dely
        assertEQ(x, true);

        x = delay._getDelayOutput(1);  // read it back with one sample delay
        assertEQ(x, false);
    }

    static void canExtractBit() {
        assertEQ(BitDelay::_extractBit(0, 0), false);
        assertEQ(BitDelay::_extractBit(1, 0), true);
        assertEQ(BitDelay::_extractBit(0x80000000, 0), false);
        assertEQ(BitDelay::_extractBit(0x80000000, 31), true);
        for (int i = 0; i < 32; ++i) {
            assertEQ(BitDelay::_extractBit(0, i), false);
            assertEQ(BitDelay::_extractBit(0xffffffff, i), true);

            const unsigned word = 1 << i;
            assertEQ(BitDelay::_extractBit(word, i), true);
        }
    }

    static void canPackBit() {
        // simple cases for debugging.
        assertEQ(BitDelay::_packBit(0, 0, 0), 0);
        assertEQ(BitDelay::_packBit(0, 0, 1), 1);

        assertEQ(BitDelay::_packBit(0x80, 0, 1), 0x81);
        assertEQ(BitDelay::_packBit(0x80, 0, 0), 0x80);
        for (int i = 0; i < 32; ++i) {
            assertEQ(BitDelay::_packBit(0, i, 1), 1 << i);
            assertEQ(BitDelay::_packBit(0, i, 0), 0);
            assertEQ(BitDelay::_packBit(0xffffffff, i, 1), 0xffffffff);
            assertEQ(BitDelay::_packBit(0xffffffff, i, 0), (0xffffffff & (~(1 << i))));
        }
    }

    static void testNextPtr() {
        BitDelay delay;
        delay.setMaxDelaySamples(16);
        uint32_t p = 0;
        delay._nextDelayPointer(p);
        assertEQ(p, 1);
        delay._nextDelayPointer(p);
        assertEQ(p, 2);

        p = delay.getMaxDelaySize() - 1;
        assertGT(p, 10);
        delay._nextDelayPointer(p);
        assertEQ(p, 0);
    }

    static void testPrevPtr() {
        BitDelay delay;
        delay.setMaxDelaySamples(4);
        uint32_t p = 1;
        delay._prevDelayPointer(p);
        assertEQ(p, 0);
        delay._prevDelayPointer(p);
        assertEQ(p, delay.getMaxDelaySize() - 1);
    }

    static void verifyIndexAndBit(unsigned input, unsigned expectedIndex, unsigned expectedBit) {
        const auto indexAndBit = BitDelay::_getIndexAndBit(input);
        const unsigned index = std::get<0>(indexAndBit);
        const unsigned bit = std::get<1>(indexAndBit);
        assertEQ(index, expectedIndex);
        assertEQ(bit, expectedBit);
    }

    static void getIndexAndBit() {
        verifyIndexAndBit(0, 0, 0);
        verifyIndexAndBit(1, 0, 1);
        verifyIndexAndBit(31, 0, 31);

        verifyIndexAndBit(32, 1, 0);
        verifyIndexAndBit(40, 1, 8);
        verifyIndexAndBit(41, 1, 9);

        verifyIndexAndBit(39, 1, 7);
        verifyIndexAndBit(64, 2, 0);
    }
#endif

    static void testAdvance2A() {
        BitDelay2 delay;
        const auto orig = delay._delayPointer;
        delay._advance();
        assertLT(orig, delay._delayPointer);
    }

    static void testAdvance2B() {

        BitDelay2 delay;
        delay._delayPointer =  delay._delayMemory + (BitDelay2::_delaySize - 1);
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
        assertEQ(delay._delayPointer, delay._delayMemory + (BitDelay2::_delaySize - 1));
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
        testDelaySize<T>();

        testDelayA<T>();
        testDelayB<T>();
        testDelayC<T>();
        testDelayD<T>();

        testDelay0<T>();
        testDelay20<T>();
    }
};

#if 0
static void testBitDelayOnly() {
    assert(false);  // bit delay doesn't work
    TestX::canExtractBit();
    TestX::getIndexAndBit();
    TestX::canPackBit();
    TestX::testNextPtr();
    TestX::testPrevPtr();
    TestX::canAccessDelayAtZero();
    TestX::canAccessDelay2();
    TestX::canAccessDelay3();
}
#endif

void testBitDelay2Only() {
    TestX::testAdvance2A();
    TestX::testAdvance2B();
    TestX::testDecrement2A();
    TestX::testDecrement2B();
    TestX::testWriteRingBuffer();
}

void testBitDelay() {
    // testBitDelayOnly();
    testBitDelay2Only();
   // BitDelayTest<BitDelay>::test();
    BitDelayTest<BitDelay2>::test();
}

#if 0
static void showDelay4() {
    BitDelay delay;
    delay.setMaxDelaySamples(100);
    int time = 4;
    for (int i=0; i< 8; ++i) {
        const bool b = delay.process(false, time);
        SQINFO("t=%d, input=false delay = %d out=%d", i, time, b);
    }
     for (int i=0; i< 8; ++i) {
        const bool b = delay.process(true, time);
        SQINFO("t=%d, input=true delay = %d out=%d", i + 8, time, b);
    }
}
#endif

#if 0
void testFirst() {
    //  testBitDelay();
    //   testDelay0<BitDelay2>();
    // TestX::canAccessDelay3();
    //   BitDelayTest<BitDelay2>::test();
    // BitDelayTest<BitDelay>::test();

       testDelayD<BitDelay2>();
   // testBitDelay2Only();
}
#endif