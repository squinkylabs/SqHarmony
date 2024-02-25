
#include "BitDelay.h"

#include "asserts.h"



class TestX {
public:
    static void canDelayZero() {
        BitDelay delay;
        delay.setMaxDelaySamples(0);
        delay._insertDelayInput(false);
        bool x = delay._getDelayOutput(0);
        assertEQ(x, false);

        delay._insertDelayInput(true);
        x = delay._getDelayOutput(0);
        assertEQ(x, true);
    }

    static void canDelay(unsigned delaySamples) {
        BitDelay delay;
        delay.setMaxDelaySamples(delaySamples);

        delay._insertDelayInput(true);
        for (unsigned i = 0; i < delaySamples + 10; ++i) {
            bool x = delay._getDelayOutput(i);
            assertEQ(x, (i == delaySamples));
            delay._insertDelayInput(false);
        }
    }

    static void canDelay() {
        for (unsigned i = 0; i < 200; ++i) {
            canDelay(i);
        }
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
};


void testBitDelay() {
    TestX::canExtractBit();
    TestX::canPackBit();
    TestX::canDelayZero();
    TestX::canDelay();
}