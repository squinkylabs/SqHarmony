
#include "BitDelay.h"

#include "asserts.h"



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
        delay._currentLocation = 1;      // let's work on the second bit

        delay._insertDelayInput(true);

        bool x = delay._getDelayOutput(1);      // read it back with no dely
        assertEQ(x, true);
        SQINFO("passed canAccessDelay2");
    }

     static void canAccessDelay3() {
         BitDelay delay;
         int delayAmount = 40;    // a nice uneven values
         delay.setMaxDelaySamples(100);
         delay._currentLocation = delayAmount;      // let's work on the second bit

         delay._insertDelayInput(true);

         bool x = delay._getDelayOutput(delayAmount);      // read it back with no dely
         assertEQ(x, true);

         x = delay._getDelayOutput(delayAmount -1);      // read it back with no dely
         assertEQ(x, false);

         x = delay._getDelayOutput(delayAmount + 1);      // read it back with no dely
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
        SQINFO("A");
        assertEQ(p, delay.getMaxDelaySize() - 1);
        SQINFO("B");
    }
};


void testBitDelay() {
    TestX::canExtractBit();
    TestX::canPackBit();
    TestX::testNextPtr();
    TestX::testPrevPtr();
    TestX::canAccessDelayAtZero();
    TestX::canAccessDelay2();
    TestX::canAccessDelay3();
}

#if 1
void testFirst() {
    TestX::canAccessDelay3();
}
#endif