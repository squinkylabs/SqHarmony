#include "ArpegPlayer.h"
#include "asserts.h"

class TestStuff {
public:
    NoteBuffer nb;
    ArpegPlayer ap;
    TestStuff(const float* initialInput, int inputSize) : nb(inputSize + 4),
                                                          ap(&nb) {
    }
    static std::shared_ptr<TestStuff> make(const float* initialInput, int inputSize) {
        std::shared_ptr<TestStuff> ret = std::make_shared<TestStuff>(initialInput, inputSize);
        ret->ap.setMode(ArpegPlayer::Mode::UP);
        // put in the input chord
        for (int i = 0; i < inputSize; ++i) {
            float other = 50 + initialInput[i];
            ret->nb.push_back(initialInput[i], other, i);
        }
        return ret;
    }

private:
};

static void testNewNoteAbove() {
    float input[] = {1, 2, 3, 4};
    const int numInput = 4;
    auto stuff = TestStuff::make(input, numInput);

    auto x = stuff->ap.clock2();
    assert(std::get<0>(x));
    assertEQ(std::get<1>(x), 1);
    assertEQ(std::get<2>(x), 51);
    x = stuff->ap.clock2();
    assertEQ(std::get<1>(x), 2);
    assertEQ(std::get<2>(x), 52);

    stuff->nb.push_back(10, 100, 4);  // add new note
    x = stuff->ap.clock2();
    assertEQ(std::get<1>(x), 3);
    assertEQ(std::get<2>(x), 50 + 3);
    x = stuff->ap.clock2();
    assertEQ(std::get<1>(x), 4);
    assertEQ(std::get<2>(x), 50 + 4);
    x = stuff->ap.clock2();
    assertEQ(std::get<1>(x), 10);
    assertEQ(std::get<2>(x), 100);
}

#define assertPair(pair, value)  \
    assertEQ(pair.first, value); \
    assertEQ(pair.second, value + 50);

static void testNewNoteBelow() {
    const int numInput = 4;
    float input[] = {1, 2, 3, 4};
    auto stuff = TestStuff::make(input, numInput);
    auto x = stuff->ap.clock();
    assertEQ(x.first, 1);
    assertEQ(x.second, 1 + 50);
    x = stuff->ap.clock();
    assertPair(x, 2);

    stuff->nb.push_back(1.1f, 5.5f, 4);  // add new note
    x = stuff->ap.clock();
    assertPair(x, 3);
    x = stuff->ap.clock();
    assertPair(x, 4);
    x = stuff->ap.clock();
    assertPair(x, 1);

    x = stuff->ap.clock();
    assertEQ(x.first, 1.1f);
    assertEQ(x.second, 5.5f);
}

static void testRemoveNoteAbove() {
    const int numInput = 4;
    float input[] = {1, 2, 3, 4};
    auto stuff = TestStuff::make(input, numInput);
    auto x = stuff->ap.clock();
    assertPair(x, 1);
    x = stuff->ap.clock();
    assertPair(x, 2);

    stuff->nb.removeForChannel(3);

    x = stuff->ap.clock();
    assertPair(x, 3);
    x = stuff->ap.clock();
    assertPair(x, 1);
}

static void testRemoveNoteBelow() {
    const int numInput = 4;
    float input[] = {1, 2, 3, 4};
    auto stuff = TestStuff::make(input, numInput);
    auto x = stuff->ap.clock();
    assertPair(x, 1);
    x = stuff->ap.clock();
    assertPair(x, 2);

    stuff->nb.removeForChannel(0);

    x = stuff->ap.clock();
    assertPair(x, 3);
    x = stuff->ap.clock();
    assertPair(x, 4);

    x = stuff->ap.clock();
    assertPair(x, 2);
}

static void testRemoveNextNote() {
    const int numInput = 4;
    float input[] = {1, 2, 3, 4};
    auto stuff = TestStuff::make(input, numInput);
    auto x = stuff->ap.clock();
    assertPair(x, 1);
    x = stuff->ap.clock();
    assertPair(x, 2);

    stuff->nb.removeForChannel(2);

    x = stuff->ap.clock();
    assertPair(x, 4);
    x = stuff->ap.clock();
    assertPair(x, 1);
}

static void testRemoveLastNote() {
    const int numInput = 4;
    float input[] = {1, 2, 3, 4};
    auto stuff = TestStuff::make(input, numInput);
    auto x = stuff->ap.clock();
    assertPair(x, 1);
    x = stuff->ap.clock();
    assertPair(x, 2);

    x = stuff->ap.clock();
    assertPair(x, 3);

    // about to play 4, but we remove it
    stuff->nb.removeForChannel(3);

    x = stuff->ap.clock();
    assertPair(x, 1);
    x = stuff->ap.clock();
    assertPair(x, 2);
}

static void testRemoveOnlyNote() {
    const int numInput = 1;
    float input[] = {4.4f};
    auto stuff = TestStuff::make(input, numInput);
    auto x = stuff->ap.clock();
    assertPair(x, 4.4f);
    x = stuff->ap.clock();
    assertPair(x, 4.4f);

    stuff->nb.removeForChannel(0);
    assert(stuff->nb.empty());

    x = stuff->ap.clock();
    assertEQ(x.first, 0);
    assertEQ(x.second, 0);
    x = stuff->ap.clock();
    assertEQ(x.first, 0);
    assertEQ(x.second, 0);
}

void testArpegPlayer2() {
    testNewNoteAbove();

    testNewNoteBelow();
    testRemoveNoteAbove();
    testRemoveNoteBelow();
    testRemoveNextNote();
    testRemoveLastNote();
    testRemoveOnlyNote();
    ;
}