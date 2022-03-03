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
    printf("\n--- testNewNoteAbove\n");

    float input[] = {1, 2, 3, 4};
    const int numInput = 4;
    auto stuff = TestStuff::make(input, numInput);

    auto x = stuff->ap.clock();
    assertEQ(x.first, 1);
    assertEQ(x.second, 51);
    x = stuff->ap.clock();
    assertEQ(x.first, 2);
    assertEQ(x.second, 52);

    //
    printf("about to push new note at 26\n");
    stuff->nb.push_back(10, 100, 4);  // add new note
    x = stuff->ap.clock();
    assertEQ(x.first, 3);
    assertEQ(x.second, 50 + 3);
    x = stuff->ap.clock();
    assertEQ(x.first, 4);
    assertEQ(x.second, 50+4);
    x = stuff->ap.clock();
    assertEQ(x.first, 10);
    assertEQ(x.second, 100);
}

#if 0
static void testNewNoteBelow() {
    printf("\n--- testNewNoteBelow\n");

    const int numInput = 4;
    float input[] = {1, 2, 3, 4};
    auto stuff = TestStuff::make(input, numInput);
    float x = stuff->ap.clock();
    assertEQ(x, 1);
    x = stuff->ap.clock();
    assertEQ(x, 2);

    printf("about to push new note at 26\n");
    stuff->nb.push_back(1.1f, 0, 4);  // add new note
    x = stuff->ap.clock();
    assertEQ(x, 3);
    x = stuff->ap.clock();
    assertEQ(x, 4);
    x = stuff->ap.clock();
    assertEQ(x, 1);

    x = stuff->ap.clock();
    assertEQ(x, 1.1f);
}

static void testRemoveNoteAbove() {
    printf("\n--- testNewNoteAbove\n");

    const int numInput = 4;
    float input[] = {1, 2, 3, 4};
    auto stuff = TestStuff::make(input, numInput);
    float x = stuff->ap.clock();
    assertEQ(x, 1);
    x = stuff->ap.clock();
    assertEQ(x, 2);

    printf("about to remove at note at 81\n");
    stuff->nb.removeForChannel(3);

    x = stuff->ap.clock();
    assertEQ(x, 3);
    x = stuff->ap.clock();
    assertEQ(x, 1);
}

static void testRemoveNoteBelow() {
    printf("\n--- testNewNoteBelow\n");

    const int numInput = 4;
    float input[] = {1, 2, 3, 4};
    auto stuff = TestStuff::make(input, numInput);
    float x = stuff->ap.clock();
    assertEQ(x, 1);
    x = stuff->ap.clock();
    assertEQ(x, 2);

    printf("about to remove at note at 101\n");
    stuff->nb.removeForChannel(0);

    x = stuff->ap.clock();
    assertEQ(x, 3);
    x = stuff->ap.clock();
    assertEQ(x, 4);

    x = stuff->ap.clock();
    assertEQ(x, 2);
}

static void testRemoveNextNote() {
    printf("\n--- testRemoveNextNote\n");

    const int numInput = 4;
    float input[] = {1, 2, 3, 4};
    auto stuff = TestStuff::make(input, numInput);
    float x = stuff->ap.clock();
    assertEQ(x, 1);
    x = stuff->ap.clock();
    assertEQ(x, 2);

    printf("about to remove at note at 124\n");
    stuff->nb.removeForChannel(2);

    x = stuff->ap.clock();
    assertEQ(x, 4);
    x = stuff->ap.clock();
    assertEQ(x, 1);
}

static void testRemoveLastNote() {
    printf("\n--- testRemoveLastNote\n");

    const int numInput = 4;
    float input[] = {1, 2, 3, 4};
    auto stuff = TestStuff::make(input, numInput);
    float x = stuff->ap.clock();
    assertEQ(x, 1);
    x = stuff->ap.clock();
    assertEQ(x, 2);

    x = stuff->ap.clock();
    assertEQ(x, 3);

    // about to play 4, but we remove it
    printf("about to remove at note at 144\n");
    stuff->nb.removeForChannel(3);

    x = stuff->ap.clock();
    assertEQ(x, 1);
    x = stuff->ap.clock();
    assertEQ(x, 2);
}

static void testRemoveOnlyNote() {
    printf("\n--- testRemoveLastNote\n");

    const int numInput = 1;
    float input[] = {4.4f};
    auto stuff = TestStuff::make(input, numInput);
    float x = stuff->ap.clock();
    assertEQ(x, 4.4f);
    x = stuff->ap.clock();
    assertEQ(x, 4.4f);

    printf("about to remove at note at 170\n");
    stuff->nb.removeForChannel(0);
    assert(stuff->nb.empty());

    x = stuff->ap.clock();
    assertEQ(x, 0);
    x = stuff->ap.clock();
    assertEQ(x, 0);
}
#endif

void testArpegPlayer2() {
    testNewNoteAbove();
    
#if 0
    testNewNoteBelow();
    testRemoveNoteAbove();
    testRemoveNoteBelow();
    testRemoveNextNote();
    testRemoveLastNote();
    testRemoveOnlyNote();
#endif
    printf("finish me tap2\n");
}