
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
            delay.process(value, 0);
        }
    }
}

template <typename T>
static void validateDelay(T& delay, const TestData& data) {
    assert(!data.empty());
    const auto lastData = data[data.size() - 1];


    for (size_t i = 0; i < data.size(); ++i) {
        const auto d = data[i];
        const bool value = std::get<0>(d);
        const unsigned count = std::get<1>(d);
        for (unsigned j = 0; j < count; ++j) {
            //   bool process(bool InputClock, unsigned delay, Errors* error = nullptr);
            const bool delayedValue = delay.process(false, value);

            SQINFO("delayed = %d i=%d, j=%d", delayedValue, i, j);
        }
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

static void doTest() {
    testOne<BitDelay>();
    testOne<BitDelay2>();
}

void testBitDelayPartTwo() {
    doTest();
}

#if 1
void testFirst() {
    testOne<BitDelay>();
}
#endif