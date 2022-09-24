
#include "ArpegRhythmPlayer.h"
#include "asserts.h"

static void testArpegRPSub(int length, const float* input, int numInput, const float* expectedOutput, int numOutput) {
    NoteBuffer nb(20);
    ArpegPlayer ap(&nb);
    ap.setMode(ArpegPlayer::Mode::UP);
    ArpegRhythmPlayer arp(&ap);

    arp.setLength(length);

    for (int i = 0; i < numInput; ++i) {
        nb.push_back(input[i], input[i] + 11, i);
    }

    for (int i = 0; i < numOutput; ++i) {
        const float expected = expectedOutput[i];
        const auto actual = arp.clock();
        // printf("i=%d actual=%f expected=%f\n", i, actual, expected);
        assertEQ(actual.first, expected);
        assertEQ(actual.second, expected + 11);
    }
}

static void testArpegRhythmPlayer0() {
    NoteBuffer nb(1);
    ArpegPlayer ap(&nb);
    ArpegRhythmPlayer arp(&ap);
    arp.setLength(0);
    arp.clock();
}

static void testArpegRhythmPlayer1() {
    float input[] = {1, 2, 3};
    float expectedOutput[] = {1, 2};

    testArpegRPSub(0, input, 3, expectedOutput, 2);
}

// testArpegRPSub(int length, float* input, int numInput, float* expectedOutput, int numOutput)
static void testArpegRhythmPlayer2() {
    float input[] = {1, 2, 3};
    float expectedOutput[] = {1, 2, 1};

    testArpegRPSub(2, input, 3, expectedOutput, 3);
}

#define assertPair(p, a, b) \
    assertEQ(p.first, a);   \
    assertEQ(p.second, b);

static void testArpegRhythmPlayerReset() {
    NoteBuffer nb(20);
    ArpegPlayer ap(&nb);
    ap.setMode(ArpegPlayer::Mode::UP);
    ArpegRhythmPlayer arp(&ap);

    const int numInput = 4;
    const int len = 3;
    const float input[] = {1, 2, 3, 4};
    arp.setLength(len);

    for (int i = 0; i < numInput; ++i) {
        nb.push_back(input[i], input[i] + 3, i);
    }

    auto x = arp.clock();
    assertEQ(x.first, 1);
    x = arp.clock();
    assertPair(x, 2, 5);
    arp.reset();

    x = arp.clock();
    assertPair(x, 1, 4);
    x = arp.clock();
    assertPair(x, 2, 5);
    x = arp.clock();
    assertPair(x, 3, 6);
}

void testArpegRhythmPlayer() {
    testArpegRhythmPlayer0();
    testArpegRhythmPlayer1();
    testArpegRhythmPlayer2();
    testArpegRhythmPlayerReset();
}