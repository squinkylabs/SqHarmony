
#include "BitDelay.h"
#include "asserts.h"

static void testCanCall() {
    BitDelay delay;
    delay.setMaxDelaySamples(100);
    delay.getMaxDelaySize();
    delay.process(false, 0);
}

static void testDelaySize() {
    BitDelay delay;
    delay.setMaxDelaySamples(100);
    const auto x = delay.getMaxDelaySize();
    assertGE(x, 100);
}

static void testDelay0() {
    BitDelay delay;
    BitDelay::Errors err;
    delay.setMaxDelaySamples(100);
    bool b = delay.process(true, 0, &err);
    assertEQ(b, true);
    assert(err == BitDelay::Errors::NoError);

    b = delay.process(false, 0, &err);
    assertEQ(b, false);
    assert(err == BitDelay::Errors::NoError);

    b = delay.process(true, 0, &err);
    assertEQ(b, true);
    assert(err == BitDelay::Errors::NoError);

    b = delay.process(true, 0, &err);
    assertEQ(b, true);
    assert(err == BitDelay::Errors::NoError);
}

static void testDelay20() {
    BitDelay delay;
    BitDelay::Errors err;
    delay.setMaxDelaySamples(100);
    bool b = delay.process(true, 20, &err);
    assertEQ(b, false);
    assert(err == BitDelay::Errors::NoError);

    b = delay.process(false, 20, &err);
    assertEQ(b, false);
    assert(err == BitDelay::Errors::NoError);

    b = delay.process(true, 20, &err);
    assertEQ(b, false);
    assert(err == BitDelay::Errors::NoError);

    b = delay.process(true, 20, &err);
    assertEQ(b, false);
    assert(err == BitDelay::Errors::NoError);

    // ok, have done four cycles. So doing 15
    // more should get use to 20, the test.
    for (int i = 0; i < 16; ++i) {
        b = delay.process(true, 20, &err);
        assertEQ(b, false);
        assert(err == BitDelay::Errors::NoError);
    }

    b = delay.process(false, 20, &err);
    assertEQ(b, true);
    assert(err == BitDelay::Errors::NoError);

    b = delay.process(false, 20, &err);
    assertEQ(b, false);
    assert(err == BitDelay::Errors::NoError);

    b = delay.process(false, 20, &err);
    assertEQ(b, true);
    assert(err == BitDelay::Errors::NoError);

    b = delay.process(false, 20, &err);
    assertEQ(b, true);
    assert(err == BitDelay::Errors::NoError);
}

class TestX {
public:
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
};

void testBitDelay() {
    TestX::canExtractBit();
    TestX::getIndexAndBit();
    TestX::canPackBit();
    TestX::testNextPtr();
    TestX::testPrevPtr();
    TestX::canAccessDelayAtZero();
    TestX::canAccessDelay2();
    TestX::canAccessDelay3();

    testCanCall();
    testDelaySize();
    testDelay0();
    testDelay20();
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


void testFirst() {
    showDelay4();
    //TestX::canAccessDelay3();
}
#endif