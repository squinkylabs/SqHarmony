
#include <vector>

//#include "BitDelay.h"
#include "BitDelay2.h"
#include "asserts.h"

using TestRecord = std::tuple<bool, unsigned>;
using TestData = std::vector<TestRecord>;

static void fillDelay(BitDelay2& delay, const TestData& data) {
    for (size_t i = 0; i < data.size(); ++i) {
        const auto d = data[i];
        const bool value = std::get<0>(d);
        const unsigned count = std::get<1>(d);
        for (unsigned j=0; j<count; ++j) {
            //   bool process(bool InputClock, unsigned delay, Errors* error = nullptr);
           // SQINFO("fill with %d", value);
            delay.process(value, 0);
        }
    }
}

template <typename T>
static void validateDelayOrig(T& delay, const TestData& data) {
    SQINFO("--- validate delay orig ------");
    assert(!data.empty());
    const auto lastData = data[data.size() - 1];

    unsigned totalCount = 1;  
    for (size_t i = 0; i < data.size(); ++i) {
        const auto d = data[i];
        const bool value = std::get<0>(d);
        const unsigned count = std::get<1>(d);
        for (unsigned j = 0; j < count; ++j) {
            //   bool process(bool InputClock, unsigned delay, Errors* error = nullptr);
            const bool delayedValue = delay.process(false, totalCount);

            SQINFO("delayed = %d i=%d, j=%d totlcount=%d value = %d", delayedValue, i, j, totalCount, value);
            assertEQ(delayedValue, value);          
            totalCount += 1;    // was 2 for a while, but never worked, so....
        }
    }

    SQINFO("validate ignoring after part...");
#if 0
    for (int i = 0; i < 10; ++i) {
        bool b = delay.process(false, totalCount);
        SQINFO("after: b=%d tc=%d", b, totalCount);
        assertEQ(b, false);
        totalCount += 2;
    }
#endif

}


static unsigned countTotal(const TestData& data) {
    unsigned sum = 0;
    for (auto element : data) {
        const unsigned elementCount = std::get<1>(element);
        sum += elementCount;
    }
    return sum;
}

static void testCount() {

    const unsigned x = countTotal({
        {true, 6},
        {false, 7},
        {true, 1}
        });
    assertEQ(x, 14);
}

template <typename T>
static void validateDelay(T& delay, const TestData& data) {
    SQINFO("--- validate delay ------");

   // unsigned totalCount = 1;
    unsigned testLength = countTotal(data);

    // loop over the input records.
    for (size_t i = 0; i < data.size(); ++i) {
        const auto d = data[i];
        const bool value = std::get<0>(d);
        const unsigned count = std::get<1>(d);

        // For each record, verify the block.
        for (unsigned j = 0; j < count; ++j) {
            //   bool process(bool InputClock, unsigned delay, Errors* error = nullptr);
            const bool delayedValue = delay.process(false, testLength);

            //SQINFO("delayed = %d i=%d, j=%d testLength=%d value = %d", delayedValue, i, j, testLength, value);
            assertEQ(delayedValue, value);
        }
    }

    SQINFO("validate ignoring after part...");
#if 0
    for (int i = 0; i < 10; ++i) {
        bool b = delay.process(false, totalCount);
        SQINFO("after: b=%d tc=%d", b, totalCount);
        assertEQ(b, false);
        totalCount += 2;
    }
#endif

}



static void runTest(const TestData& td) {
    BitDelay2 del;
   // del.setMaxDelaySamples(1000);       // 1000 totally arbitrary
    fillDelay(del, td);
    validateDelay(del, td);
}

static void testOne() {
    runTest({{false, 1}});
}


static void testOneB() {
    runTest({ {true, 1} });
}

static void testTwo() {
    runTest({
        {false, 1},
        {true, 1}
        });
}

static void testThree() {
    runTest({
        {true, 1},
        {true, 1},
        {true, 1}
        });
}

static void testThreeX2() {
    runTest({
        {true, 3},
        {false, 3},
        {true, 3}
        });
}


static void testThreeX2b() {
    runTest({
        {false, 3},
        {true, 3},
        {false, 3}
        });
}

static void testGrow() {
    runTest({
        {true, 1},
        {false, 2},
        {true, 3},
        {false, 4},
        {true, 5},
        {false, 6},
        {true, 7}
        });
}

static void testGrowb() {
    runTest({
        {false, 1},
        {true, 1},
        {false, 2},
        {true, 3},
        {false, 4},
        {true, 5},
        {false, 6},
        {true, 7}
        });
}

static void testBig() {
    runTest({
        {true, 100000},
        {false, 100000},
        {true, 100000}
    });
}

static void doTest() {
    testOne();
    testOneB();
    testTwo();
    testThree();
    testThreeX2();
    testThreeX2b();
    testGrow();
    testGrowb();
    testBig();
}

void testBitDelayPartTwo() {
    testCount();
   // doTest<BitDelay>();
    doTest();
}

#if 0
void testFirst() {
 //   testCount();
 //  testOne<BitDelay2>();
 //  testOneB<BitDelay2>();
  // testTwo<BitDelay2>();
  //  testThree<BitDelay2>();
    doTest<BitDelay2>();
}
#endif