
#include <vector>

#include "BitDelay.h"
#include "BitDelay2.h"
#include "asserts.h"

using TestRecord = std::tuple<bool, unsigned>;
using TestData = std::vector<TestRecord>;

template <typename T>
static void fillDelay(T& delay, const TestData& data) {
    //  assert(!data.empty());
    // const auto firstData = data[0];
    for (size_t i = 0; i < data.size(); ++i) {
        const auto d = data[i];
        const bool value = std::get<0>(d);
        const unsigned count = std::get<1>(d);
        for (unsigned j=0; j<count; ++j) {
            //   bool process(bool InputClock, unsigned delay, Errors* error = nullptr);
            SQINFO("fill with %d", value);
            delay.process(value, 0);
        }
    }
}

template <typename T>
static void validateDelay(T& delay, const TestData& data) {
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

            // assert delayed == d
            assertEQ(delayedValue, value);

            SQINFO("delayed = %d i=%d, j=%d totlcount=%d value = %d", delayedValue, i, j, totalCount, value);
            totalCount += 2;    // we just processed one, and we just added one.s
        }
    }

    for (int i = 0; i < 10; ++i) {
        bool b = delay.process(false, totalCount);
        SQINFO("after: b=%d tc=%d", b, totalCount);
        assertEQ(b, false);
        totalCount += 2;
    }

}

template <typename T>
static void runTest(const TestData& td) {
    T del;
    del.setMaxDelaySamples(1000);       // 1000 totally arbitrary
    fillDelay<T>(del, td);
    validateDelay(del, td);
}

template <typename T>
static void testOne() {
    runTest<T>({{false, 1}});
}

template <typename T>
static void testTwo() {
    runTest<T>({
        {false, 1},
        {true, 1}
        });
}

template <typename T>
static void testThree() {
    runTest<T>({
        {true, 1},
        {true, 1},
        {true, 1}
        });
}
static void doTest() {
    testOne<BitDelay>();
    testOne<BitDelay2>();
}

void testBitDelayPartTwo() {
    doTest();
}

#if 1

static void foo() {
    BitDelay delay;
    delay.setMaxDelaySamples(100);

    delay.process(true, 0);
    delay.process(true, 0);
    delay.process(true, 0);

    // param2:

    // 0 -> false
    // 1 -> true
    // 2 -> true
    // 3 -> true
    // 4 -> false
    // first output on first poll at 1
    bool a = delay.process(false, 1);
    SQINFO("a = %d", a);

    // 0 -> false
    // 1 -> false
    // 2 -> true
    // 3 -> true
    // 4 -> true
    // 5 -> false
    // second output on second poll at 3
    
    a = delay.process(false, 5);
    SQINFO("a = %d", a);

}



void testFirst() {
   testThree<BitDelay>();
   //foo();
}
#endif