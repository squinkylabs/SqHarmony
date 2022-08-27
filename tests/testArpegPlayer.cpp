
#include "ArpegPlayer.h"
#include "asserts.h"

static void testArpegPlayerOneNote() {
    NoteBuffer nb(1);
    nb.push_back(5, 12, 23);
    ArpegPlayer ap(&nb);
    ap.setMode(ArpegPlayer::Mode::UP);
    const auto x = ap.clock();
    assertEQ(x.first, 5);
    assertEQ(x.second, 12);
    assert(!ap.empty());
}

#define assertPair(p, a, b) \
    assertEQ(p.first, a);   \
    assertEQ(p.second, b);

static void testArpegPlayerTwoNotes() {
    NoteBuffer nb(4);
    nb.push_back(5, 99, 55);
    nb.push_back(100, 8, 56);
    ArpegPlayer ap(&nb);
    ap.setMode(ArpegPlayer::Mode::UP);

    auto x = ap.clock();
    assertPair(x, 5, 99);

    x = ap.clock();
    assertPair(x, 100, 8);

    // back to start
    x = ap.clock();
    assertPair(x, 5, 99);

    assert(!ap.empty());
}

static void testArpegPlayerZeroNotes() {
    NoteBuffer nb(1);
    nb.push_back(5, 111, 55);
    nb.removeForChannel(55);

    ArpegPlayer ap(&nb);
    const auto x = ap.clock();
    assertPair(x, 0, 0);
    assert(ap.empty());
}

// no pause, simpler version
static void testArpegSubx(ArpegPlayer::Mode mode, const std::pair<float, float>* input, int numInput, const std::pair<float, float>* expectedOutput, int numOutput) {
    const int requiredSize = numInput + 2;  // 2 is "just because
    NoteBuffer nb(requiredSize);
    ArpegPlayer ap(&nb);
    ap.setMode(mode);

    for (int i = 0; i < numInput; ++i) {
        nb.push_back(input[i].first, input[i].second, i);
    }

    for (int i = 0; i < numOutput; ++i) {
        const auto expected = expectedOutput[i];
        const auto actual = ap.clock();
        assertPair(actual, expected.first, expected.second);
    }
}

static void testArpegPauseSub2(ArpegPlayer::Mode mode,
                               const float* input,
                               int numInput,
                               const std::pair<float, float>* expectedOutputBefore,
                               int numOutputBefore,
                               bool withClockInBetween

) {
    NoteBuffer nb(numInput + 4);
    ArpegPlayer ap(&nb);
    ap.setMode(mode);

    // put in the input chord
    for (int i = 0; i < numInput; ++i) {
        nb.push_back(input[i], 0, i);
    }

    // verify initial output
    for (int i = 0; i < numOutputBefore; ++i) {
        const auto expected = expectedOutputBefore[i];
        const auto actual = ap.clock();
        assertPair(actual, expected.first, expected.second);
    }

    // remove the input
    for (int i = 0; i < numInput; ++i) {
        //  nb.push_back(input[i], i);
        nb.removeForChannel(i);
    }
    assert(nb.empty());

    if (withClockInBetween) {
        assert(withClockInBetween);
        // one clock to make it work
        auto x = ap.clock();
        assertPair(x, 0, 0);
    }

    // add the input back
    for (int i = 0; i < numInput; ++i) {
        nb.push_back(input[i], 0, i);
    }

    // verify the post o
    for (int i = 0; i < numOutputBefore; ++i) {
        const auto expected = expectedOutputBefore[i];
        const auto actual = ap.clock();
        assertPair(actual, expected.first, expected.second);
    }
}

static void testArpegPauseSub(ArpegPlayer::Mode mode,
                              const std::pair<float, float>* input,
                              int numInput,
                              const std::pair<float, float>* expectedOutput,
                              int numOutput) {
    testArpegSubx(mode, input, numInput, expectedOutput, numOutput);

    // everyone should output zero when fed with it
    std::pair<float, float> zeros[3] = {std::make_pair(0.f, 0.f)};
    testArpegSubx(mode, input, 0, zeros, 2);

    static bool b = false;
    if (!b) {
        printf("skipping pause for now\n");
        b = true;
    }
    // testArpegPauseSub2(mode, input, numInput, expectedOutput, numOutput, true);
    //  testArpegPauseSub2(mode, input, numInput, expectedOutput, numOutput, false);
}

static void testArpegPlayerUp() {
    auto mode = ArpegPlayer::Mode::UP;

    //  float input[] = {10, 9, 8};
    std::pair<float, float> input[] = {
        std::make_pair(10.f, 101.f),
        std::make_pair(9.f, 901.f),
        std::make_pair(8.f, 801.f)};

    std::pair<float, float> expectedOutput[] = {
        std::make_pair(8.f, 801.f),
        std::make_pair(9.f, 901.f),
        std::make_pair(10.f, 101.f),
        std::make_pair(8.f, 801.f),
        std::make_pair(9.f, 901.f),
        std::make_pair(10.f, 101.f)};

    testArpegPauseSub(mode, input, 3, expectedOutput, 6);

    std::pair<float, float> input2[] = {
        std::make_pair(10.f, 101.f)};
    std::pair<float, float> expectedOutput2[] = {
        std::make_pair(10.f, 101.f),
        std::make_pair(10.f, 101.f),
        std::make_pair(10.f, 101.f),
        std::make_pair(10.f, 101.f)};

    testArpegSubx(mode, input2, 1, expectedOutput2, 4);

    std::pair<float, float> input3[] = {
        std::make_pair(10.f, 101.f),
        std::make_pair(11.f, 111.f)};

    std::pair<float, float> expectedOutput3[] = {
        std::make_pair(10.f, 101.f),
        std::make_pair(11.f, 111.f),
        std::make_pair(10.f, 101.f),
        std::make_pair(11.f, 111.f)};
    testArpegSubx(mode, input3, 2, expectedOutput3, 4);
}

static void testArpegPlayerDown() {
    const auto mode = ArpegPlayer::Mode::DOWN;
    std::pair<float, float> input[] = {
        std::make_pair<float, float>(10, 910),
        std::make_pair<float, float>(9, 909),
        std::make_pair<float, float>(8, 908)};

    std::pair<float, float> expectedOutput[] = {
        std::make_pair<float, float>(10, 910),
        std::make_pair<float, float>(9, 909),
        std::make_pair<float, float>(8, 908),
        std::make_pair<float, float>(10, 910),
        std::make_pair<float, float>(9, 909),
        std::make_pair<float, float>(8, 908),
        std::make_pair<float, float>(10, 910)};

    testArpegPauseSub(mode, input, 3, expectedOutput, 7);

    std::pair<float, float> input2[] = {
        std::make_pair<float, float>(10, 910)};
    std::pair<float, float> expectedOutput2[] = {
        std::make_pair<float, float>(10, 910),
        std::make_pair<float, float>(10, 910),
        std::make_pair<float, float>(10, 910),
        std::make_pair<float, float>(10, 910)};
    testArpegSubx(mode, input2, 1, expectedOutput2, 4);

    std::pair<float, float> input3[] = {
        std::make_pair<float, float>(10, 910),
        std::make_pair<float, float>(11, 911)};
    std::pair<float, float> expectedOutput3[] = {
        std::make_pair<float, float>(11, 911),
        std::make_pair<float, float>(10, 910),
        std::make_pair<float, float>(11, 911),
        std::make_pair<float, float>(10, 910)};
    testArpegSubx(mode, input3, 2, expectedOutput3, 4);
}

static void testArpegPlayerUp_Down() {
    const auto mode = ArpegPlayer::Mode::UP_DOWN_DBL;
    const std::pair<float, float> input[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(6, 16),
        std::make_pair<float, float>(5, 15)};
    std::pair<float, float> expectedOutput[] = {
        std::make_pair<float, float>(5, 15),
        std::make_pair<float, float>(6, 16),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(6, 16),
        std::make_pair<float, float>(5, 15),
        std::make_pair<float, float>(5, 15),
        std::make_pair<float, float>(6, 16),
        std::make_pair<float, float>(7, 17)};

    testArpegPauseSub(mode, input, 6, expectedOutput, 15);

    std::pair<float, float> expectedOutput5[] = {
        std::make_pair<float, float>(6, 16),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(6, 16),
        std::make_pair<float, float>(6, 16),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(10, 20)};
    testArpegPauseSub(mode, input, 5, expectedOutput5, 15);

    std::pair<float, float> expectedOutput4[] = {
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 18)};
    testArpegPauseSub(mode, input, 4, expectedOutput4, 15);

    std::pair<float, float> expectedOutput3[] = {
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19)};
    testArpegPauseSub(mode, input, 3, expectedOutput3, 11);

    std::pair<float, float> expectedOutput2[] = {
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(9, 19)};
    testArpegPauseSub(mode, input, 2, expectedOutput2, 9);


    std::pair<float, float> expectedOutput1[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20)};
    testArpegPauseSub(mode, input, 1, expectedOutput1, 4);
}

static void testArpegPlayerUpDown() {

    const auto mode = ArpegPlayer::Mode::UPDOWN;
    std::pair<float, float> input[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(7, 17)};
    std::pair<float, float> expectedOutput[] = {
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(10, 20)};
    testArpegPauseSub(mode, input, 4, expectedOutput, 10);

    std::pair<float, float> input2[] = {
        std::make_pair<float, float>(10, 20)};
    std::pair<float, float> expectedOutput2[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20)};
    testArpegSubx(mode, input2, 1, expectedOutput2, 4);

    std::pair<float, float> input3[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(11, 911)};
    std::pair<float, float> expectedOutput3[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(11, 911),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(11, 911),
        std::make_pair<float, float>(10, 20)};
    testArpegSubx(mode, input3, 2, expectedOutput3, 5);

    std::pair<float, float> input4[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 18)};
    std::pair<float, float> expectedOutput4[] = {
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19)};
    testArpegSubx(mode, input4, 3, expectedOutput4, 8);
}

static void testArpegPlayerDownUp() {
    const auto mode = ArpegPlayer::Mode::DOWNUP;
    std::pair<float, float> input[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(7, 17)};

    std::pair<float, float> expectedOutput[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19)};
    testArpegPauseSub(mode, input, 4, expectedOutput, 8);

    std::pair<float, float> input2[] = {
        std::make_pair<float, float>(10, 20)};
    std::pair<float, float> expectedOutput2[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20)};
    testArpegSubx(mode, input2, 1, expectedOutput2, 4);

    std::pair<float, float> input3[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(11, 911),
    };
    std::pair<float, float> expectedOutput3[] = {
        std::make_pair<float, float>(11, 911),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(11, 911),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(11, 911),
    };
    testArpegSubx(mode, input3, 2, expectedOutput3, 5);

    std::pair<float, float> input4[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 18)};
    std::pair<float, float> expectedOutput4[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(9, 19)};
    testArpegSubx(mode, input4, 3, expectedOutput4, 8);

    std::pair<float, float> input5[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(6, 16),
        std::make_pair<float, float>(5, 15)};
    std::pair<float, float> expectedOutput5[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(6, 16),
        std::make_pair<float, float>(5, 15),
        std::make_pair<float, float>(6, 16),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 18),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(6, 16),
        std::make_pair<float, float>(5, 15)};
    testArpegSubx(mode, input5, 6, expectedOutput5, 16);
}

static void testArpegPlayerDown_Up() {
    const auto mode = ArpegPlayer::Mode::DOWN_UP_DBL;
    const std::pair<float, float> input[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 188),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(6, 16),
        std::make_pair<float, float>(5, 15)};

    const std::pair<float, float> expectedOutput[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 188),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(6, 16),
        std::make_pair<float, float>(5, 15),
        std::make_pair<float, float>(5, 15),
        std::make_pair<float, float>(6, 16),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(8, 188),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 188)};
    testArpegPauseSub(mode, input, 6, expectedOutput, 15);

    std::pair<float, float> expectedOutput5[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 188),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(6, 16),
        std::make_pair<float, float>(6, 16),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(8, 188),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 188),
        std::make_pair<float, float>(7, 17)};
    testArpegPauseSub(mode, input, 5, expectedOutput5, 14);

    std::pair<float, float> expectedOutput4[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 188),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(8, 188),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 188),
        std::make_pair<float, float>(7, 17),
        std::make_pair<float, float>(7, 17)};
    testArpegPauseSub(mode, input, 4, expectedOutput4, 13);

    std::pair<float, float> expectedOutput3[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 188),
        std::make_pair<float, float>(8, 188),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 188),
        std::make_pair<float, float>(8, 188),
        std::make_pair<float, float>(9, 19)};
    testArpegPauseSub(mode, input, 3, expectedOutput3, 11);

    std::pair<float, float> expectedOutput2[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(9, 19)};
    testArpegPauseSub(mode, input, 2, expectedOutput2, 7);

    std::pair<float, float> expectedOutput1[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(10, 20),
    };
    testArpegPauseSub(mode, input, 1, expectedOutput1, 4);
}

static void testArpegPlayerUpPause() {
    const std::pair<float, float> input[] = {
        std::make_pair<float, float>(10, 20),
        std::make_pair<float, float>(9, 19),
        std::make_pair<float, float>(8, 188)};
    const std::pair<float, float> expectedOutput[] = {
        std::make_pair<float, float>(8, 188),
        std::make_pair<float, float>(9, 19)};
    testArpegPauseSub(ArpegPlayer::Mode::UP,
                      input, 3, expectedOutput, 2);
}

static void testArpegPlayerINSIDE_OUT() {
    const auto mode = ArpegPlayer::Mode::INSIDE_OUT;
    const std::pair<float, float> input[] = {
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(3, 4),
        std::make_pair<float, float>(2, 3),
        std::make_pair<float, float>(1, 2)};

    const std::pair<float, float> expectedOutput5[] = {
        std::make_pair<float, float>(3, 4),
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(2, 3),
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(1, 2)};
    testArpegPauseSub(mode, input, 5, expectedOutput5, 5);

    const std::pair<float, float> expectedOutput4[] = {
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(3, 4),
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(2, 3)};
    testArpegPauseSub(mode, input, 4, expectedOutput4, 4);

    const std::pair<float, float> expectedOutput3[] = {
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(3, 4)};
    testArpegPauseSub(mode, input, 3, expectedOutput3, 3);

    // this doesn't seem quite right.??
    const std::pair<float, float> expectedOutput2[] = {
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(4, 5)};
    testArpegPauseSub(mode, input, 2, expectedOutput2, 2);

    const std::pair<float, float> expectedOutput1[] = {
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(5, 6)};
    testArpegPauseSub(mode, input, 1, expectedOutput1, 3);
}

static void testArpegPlayerOUTSIDE_IN() {
    const auto mode = ArpegPlayer::Mode::OUTSIDE_IN;
    const std::pair<float, float> input[] = {
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(3, 4),
        std::make_pair<float, float>(2, 3),
        std::make_pair<float, float>(1, 2)};

    const std::pair<float, float> expectedOutput5[] = {
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(2, 3),
        std::make_pair<float, float>(3, 4)};
    testArpegPauseSub(mode, input, 5, expectedOutput5, 5);

    const std::pair<float, float> expectedOutput4[] = {
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(2, 3),
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(3, 4)};
    testArpegPauseSub(mode, input, 4, expectedOutput4, 4);

    const std::pair<float, float> expectedOutput3[] = {
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(3, 4),
        std::make_pair<float, float>(4, 5)};
    testArpegPauseSub(mode, input, 3, expectedOutput3, 3);

    const std::pair<float, float> expectedOutput2[] = {
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(4, 5)};
    testArpegPauseSub(mode, input, 2, expectedOutput2, 2);

    const std::pair<float, float> expectedOutput1[] = {
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(5, 6)};
    testArpegPauseSub(mode, input, 1, expectedOutput1, 3);
}

static void testArpegPlayerOrderPlayed() {
    const auto mode = ArpegPlayer::Mode::ORDER_PLAYED;
    const std::pair<float, float> input[] = {
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(30, 30),
        std::make_pair<float, float>(2, 3),
        std::make_pair<float, float>(1, 2)};

    const std::pair<float, float> expectedOutput5[] = {
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(30, 30),
        std::make_pair<float, float>(2, 3),
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(30, 30),
        std::make_pair<float, float>(2, 3),
        std::make_pair<float, float>(1, 2)};
    testArpegPauseSub(mode, input, 5, expectedOutput5, 10);

    const std::pair<float, float> expectedOutput4[] = {
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(30, 30),
        std::make_pair<float, float>(2, 3),
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(30, 30),
        std::make_pair<float, float>(2, 3),
    };
    testArpegPauseSub(mode, input, 4, expectedOutput4, 8);

    const std::pair<float, float> expectedOutput3[] = {
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(30, 30),
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(30, 30)};
    testArpegPauseSub(mode, input, 3, expectedOutput3, 6);

    const std::pair<float, float> expectedOutput2[] = {
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(4, 5)};
    testArpegPauseSub(mode, input, 2, expectedOutput2, 2);

    const std::pair<float, float> expectedOutput1[] = {
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(5, 6)};
    testArpegPauseSub(mode, input, 1, expectedOutput1, 3);
}

static void testArpegPlayerREPEAT_BOTTOM() {
    const auto mode = ArpegPlayer::Mode::REPEAT_BOTTOM;
    const std::pair<float, float> input[] = {
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(2, 3),
        std::make_pair<float, float>(3, 4),
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(5, 6)};

    const std::pair<float, float> expectedOutput5[] = {
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(2, 3),
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(3, 4),
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(2, 3)};
    testArpegPauseSub(mode, input, 5, expectedOutput5, 10);

    const std::pair<float, float> expectedOutput4[] = {
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(2, 3),
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(3, 4),
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(2, 3)};
    testArpegPauseSub(mode, input, 4, expectedOutput4, 8);

    const std::pair<float, float> expectedOutput3[] = {
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(2, 3),
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(3, 4),
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(2, 3)};
    testArpegPauseSub(mode, input, 3, expectedOutput3, 6);

    const std::pair<float, float> expectedOutput2[] = {
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(2, 3),
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(2, 3)};
    testArpegPauseSub(mode, input, 2, expectedOutput2, 4);

    const std::pair<float, float> expectedOutput1[] = {
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(1, 2)};
    testArpegPauseSub(mode, input, 1, expectedOutput1, 3);
}

static void testArpegPlayerREPEAT_TOP() {
    const auto mode = ArpegPlayer::Mode::REPEAT_TOP;
    const std::pair<float, float> input[] = {
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(2, 3),
        std::make_pair<float, float>(3, 4),
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(5, 6)};

    const std::pair<float, float> expectedOutput5[] = {
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(2, 3),
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(3, 4),
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(2, 3)};
    testArpegPauseSub(mode, input, 5, expectedOutput5, 12);

    const std::pair<float, float> expectedOutput4[] = {
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(2, 3),
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(3, 4),
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(1, 2)};
    testArpegPauseSub(mode, input, 4, expectedOutput4, 8);

    const std::pair<float, float> expectedOutput3[] = {
        std::make_pair<float, float>(3, 4),
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(3, 4),
        std::make_pair<float, float>(2, 3),
        std::make_pair<float, float>(3, 4),
        std::make_pair<float, float>(1, 2)};
    testArpegPauseSub(mode, input, 3, expectedOutput3, 6);

    const std::pair<float, float> expectedOutput2[] = {
        std::make_pair<float, float>(2, 3),
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(2, 3),
        std::make_pair<float, float>(1, 2)};
    testArpegPauseSub(mode, input, 2, expectedOutput2, 4);

    const std::pair<float, float> expectedOutput1[] = {
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(1, 2)};
    testArpegPauseSub(mode, input, 1, expectedOutput1, 3);
}

static void testArpegPlayerSHUFFLE(bool withTrigger) {
    const auto mode = ArpegPlayer::Mode::SHUFFLE;

    const int numInput = 12;
    const std::pair<float, float> input[numInput] = {
        std::make_pair<float, float>(1, 2),
        std::make_pair<float, float>(2, 3),
        std::make_pair<float, float>(3, 4),
        std::make_pair<float, float>(4, 5),
        std::make_pair<float, float>(5, 6),
        std::make_pair<float, float>(6, 7),
        std::make_pair<float, float>(7, 8),
        std::make_pair<float, float>(8, 9),
        std::make_pair<float, float>(9, 10),
        std::make_pair<float, float>(10, 11),
        std::make_pair<float, float>(11, 12),
        std::make_pair<float, float>(12, 13)};
    std::pair<float, float> outputFirst[numInput] = {
        std::make_pair<float, float>(0, 0)};
    std::pair<float, float> outputSecond[numInput] = {
        std::make_pair<float, float>(0, 0)};

    NoteBuffer nb(numInput + 4);
    ArpegPlayer ap(&nb);
    ap.setMode(mode);

    // put in the input chord
    for (int i = 0; i < numInput; ++i) {
        nb.push_back(input[i].first, input[i].second, i);
    }

    bool didSee[numInput] = {false};

    // SQINFO("-- test will run first loop");
    for (int i = 0; i < numInput; ++i) {
        const auto x = ap.clock();
        assert(x.first > 0);
        assert(x.first < 13);
        outputFirst[i] = x;

        if (withTrigger && (i == 1)) {
            ap.armReShuffle();      // re-arm while we are in the middle of first pass, so second will see it.
        }

        // now search the input of this one
        for (int j = 0; j < numInput; ++j) {
            if (input[j] == x) {
                assert(!didSee[j]);
                didSee[j] = true;
            }
        }
    }
    //SQINFO("-- test will compare results 1");
    for (int i = 0; i < numInput; ++i) {
        assert(didSee[i]);
        assert(outputFirst[i].first > 0); 
    }

    //SQINFO("-- test will read second pass");
    for (int i = 0; i < numInput; ++i) {
        const auto x = ap.clock();
        assert(x.first > 0);
        assert(x.first < 13);
        outputSecond[i] = x;
    }

    bool anyDifferent = false;
    for (int i = 0; i < numInput; ++i) {
        if (outputFirst[i] != outputSecond[i]) {
            anyDifferent = true;
        }
    }
    if (withTrigger) {
        assert(anyDifferent);
    } else {
        assert(!anyDifferent);
    }
}

void testArpegPlayer() {
    testArpegPlayerOneNote();
    testArpegPlayerTwoNotes();
    testArpegPlayerZeroNotes();
    testArpegPlayerUp();
    testArpegPlayerDown();
    testArpegPlayerUpDown();
    testArpegPlayerDownUp();

    testArpegPlayerUp_Down();
    testArpegPlayerDown_Up();
    testArpegPlayerINSIDE_OUT();
    testArpegPlayerOUTSIDE_IN();
    testArpegPlayerOrderPlayed();
    testArpegPlayerREPEAT_BOTTOM();
    testArpegPlayerREPEAT_TOP();
        // just for now!
    testArpegPlayerSHUFFLE(true);
    testArpegPlayerSHUFFLE(false);
}
