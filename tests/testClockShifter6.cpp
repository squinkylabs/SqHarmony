#include "ClockShifter6.h"
#include "asserts.h"

static void testCanCreate() {
    ClockShifter6 c;
    c.setMaxDelaySamples(1);
    c.process(0, false);
}

static void canReturnSizeError() {
    ClockShifter6 c;
    c.setMaxDelaySamples(1);

    ClockShifter6::Errors err;

    c.process(false, 100, &err);
    assert(err == ClockShifter6::Errors::ExceededDelaySize);

    c.process(false, 1, &err);
    assert(err == ClockShifter6::Errors::NoError);
}

class TestX {
public:
    static void canDelayZero() {
        ClockShifter6 c;
        c.setMaxDelaySamples(0);
        c._insertDelayInput(false);
        bool x = c._getDelayOutput(0);
        assertEQ(x, false);

        c._insertDelayInput(true);
        x = c._getDelayOutput(0);
        assertEQ(x, true);
    }
    static void canExtractBit() {
        assertEQ(ClockShifter6::_extractBit(0, 0), false);
        assertEQ(ClockShifter6::_extractBit(1, 0), true);

        assertEQ(ClockShifter6::_extractBit(0x80000000, 0), false);
        assertEQ(ClockShifter6::_extractBit(0x80000000, 31), true);

        for (int i=0; i<32; ++i) {
            assertEQ(ClockShifter6::_extractBit(0, i), false);
            assertEQ(ClockShifter6::_extractBit(0xffffffff, i), true);

            const unsigned word = 1 << i;
            assertEQ(ClockShifter6::_extractBit(word, i), true);
        }
    }

    // t(uint32_t word, unsigned bit, value);
    static void canPackBit() {
        // simple cases for debugging.
        assertEQ(ClockShifter6::_packBit(0, 0, 0), 0);
        assertEQ(ClockShifter6::_packBit(0, 0, 1), 1);

        assertEQ(ClockShifter6::_packBit(0x80, 0, 1), 0x81);
        assertEQ(ClockShifter6::_packBit(0x80, 0, 0), 0x80);
         for (int i=0; i<32; ++i) {
            assertEQ(ClockShifter6::_packBit(0, i, 1),  1 << i);
            assertEQ(ClockShifter6::_packBit(0, i, 0),  0);
            assertEQ(ClockShifter6::_packBit(0xffffffff, i, 1),  0xffffffff);
            assertEQ(ClockShifter6::_packBit(0xffffffff, i, 0),  (0xffffffff & (~(1 << i))));
         }
    }
};

static void canDelayZero() {
    ClockShifter6 c;
    c.setMaxDelaySamples(0);
    ClockShifter6::Errors err;
    bool x = c.process(true, 0, &err);
    assertEQ(x, true);

    x = c.process(false, 0, &err);
    assertEQ(x, false);
}

void testClockShifter6() {
    TestX::canExtractBit();
    TestX::canPackBit();
  //  TestX::canDelayZero();

   // testCanCreate();
   // canReturnSizeError();
   // canDelayZero();
}