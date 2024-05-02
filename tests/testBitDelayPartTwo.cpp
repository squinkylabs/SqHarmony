
#include <vector>

//#include "BitDelay.h"
#include "BitDelay2.h"
#include "asserts.h"

using TestRecord = std::tuple<bool, unsigned>;
using TestData = std::vector<TestRecord>;

template <typename T>
static void fillDelay(T& delay, const TestData& data) {
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
    SQINFO("--- validate delay ------");
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
static void testOneB() {
    runTest<T>({ {true, 1} });
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

template <typename T>
static void doTest() {
    testOne<T>();
    testOneB<T>();
    testTwo<T>();
    testThree<T>();
}

void testBitDelayPartTwo() {
   // doTest<BitDelay>();
    doTest<BitDelay2>();
}

#if 0
void testFirst() {
//   testOne<BitDelay2>();
    testOneB<BitDelay2>();
 //   testTwo<BitDelay2>();
  //  testThree<BitDelay2>();
  //   doTest<BitDelay2>();
}
#endif