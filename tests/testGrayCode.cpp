#include "asserts.h"
#include "GcGenerator.h"
#include "GcUtils.h"


static void testGetWithRollover() {
    std::vector<uint16_t> x;
    x.push_back(100);
    x.push_back(200);

    auto y = GcUtils::getWithRollover(x, 0);
    assert(y == 100);
    y = GcUtils::getWithRollover(x, 1);
    assert(y == 200);
    y = GcUtils::getWithRollover(x, 2);
    assert(y == 100);
     y = GcUtils::getWithRollover(x, 3);
    assert(y == 200);
     y = GcUtils::getWithRollover(x, 4);
    assert(y == 100);
}

// static unsigned getShortestRun(unsigned numBits, std::vector<uint16_t>& data);
static void testShortestRun0() {
    std::vector<uint16_t> x;
    auto y = GcUtils::getShortestRun(4, x);
    assert(y == -1);

    x.push_back(1);
    y = GcUtils::getShortestRun(4, x);
    assert(y == 1);         // infinite loop of "1" -> just linear length
    assert(x.size() == 1);
}

static void testShortestRun1() {
    std::vector<uint16_t> x;
    x.push_back(1);
    x.push_back(1);
    x.push_back(1);
    x.push_back(0);
    x.push_back(1);
    x.push_back(1);
    x.push_back(1);
    auto y = GcUtils::getShortestRun(1, x);
    assert(y == 1);
    assert(x.size() == 7);
}

static void testShortestRun2() {
    std::vector<uint16_t> x;
    x.push_back(0);
    x.push_back(1);
    x.push_back(1);
    x.push_back(0);
    x.push_back(0);
    auto y = GcUtils::getShortestRun(1, x);
    assert(y == 2);

}

// static bool onlyDifferByOneBit(unsigned totalCount, const std::vector<uint16_t>& data1, unsigned candidate);


static void testDifferByOneEmpty() {
    const unsigned numBits = 4;
    std::vector<uint16_t> x;
    auto y = GcUtils::onlyDifferByOneBit(6, numBits, x);
    assert(y == false);

    x.push_back(1);
    y = GcUtils::onlyDifferByOneBit(6, numBits, x);
    assert(y == false);

}


static void testDifferByOne0() {
    const unsigned numBits = 5;
    std::vector<uint16_t> x;
    x.push_back(0);
    x.push_back(1);
    const auto y = GcUtils::onlyDifferByOneBit(6, numBits, x);
    assert(y == true);
}

static void testDifferByOne1() {
    const unsigned numBits = 5;
    std::vector<uint16_t> x;
    x.push_back(1);  // a little artificial, since first entry is always 0 in "normal" use..
    x.push_back(3);
    x.push_back(1);
    const auto y = GcUtils::onlyDifferByOneBit(3, numBits, x);
    assert(y == false);
}

static void testDifferByOne2() {
    const unsigned numBits = 2;
    std::vector<uint16_t> x;
    x.push_back(0);  // a little artificial, since first entry is always 0 in "normal" use..
    x.push_back(1);
    x.push_back(3);
    const auto y = GcUtils::onlyDifferByOneBit(4, numBits, x);
    assert(y == true);
}

static void testNumBitsDifferent0() {
    assert(0 == GcUtils::numBitsDifferent(1, 0, 0));
    assert(0 == GcUtils::numBitsDifferent(4, 0xa, 0xa));
    assert(0 == GcUtils::numBitsDifferent(2, 4, 0));
}

static void testNumBitsDifferent1() {
    assert(1 == GcUtils::numBitsDifferent(1, 0, 1));
    assert(1 == GcUtils::numBitsDifferent(4, 0x8, 0));
    assert(2 == GcUtils::numBitsDifferent(4, 0x8, 1));
}

static void testTransitionData0() {
    std::vector<uint16_t> x;
    auto y = GcUtils::getTranstionData(5, x);
    assert(y.first == 0);
    assert(y.second == 0);
    assert(x.size() == 0);

    x.push_back(0);
    y = GcUtils::getTranstionData(1, x);
    assert(y.first == 0);
    assert(y.second == 0);
    y = GcUtils::getTranstionData(10, x);
    assert(y.first == 0);
    assert(y.second == 0);
    assert(x.size() == 1);

    x.push_back(1);
    y = GcUtils::getTranstionData(1, x);
    assert(y.first == 1);
    assert(y.second == 1);
    assert(x.size() == 2);

    y = GcUtils::getTranstionData(2, x);
    assert(y.first == 0);
    assert(y.second == 1);
    assert(x.size() == 2);
}

static void testTransitionDataWrap0() {
    std::vector<uint16_t> x;
    auto y = GcUtils::getTranstionDataWithWrap(5, x);
    assert(y.first == 0);
    assert(y.second == 0);
    assert(x.size() == 0);

    x.push_back(0);
    y = GcUtils::getTranstionDataWithWrap(1, x);
    assert(y.first == 0);
    assert(y.second == 0);
    y = GcUtils::getTranstionDataWithWrap(10, x);
    assert(y.first == 0);
    assert(y.second == 0);
    assert(x.size() == 1);
}

static void testTransitionDataWrap1() {
    std::vector<uint16_t> x;
    x.push_back(0);
    x.push_back(0);
    x.push_back(1);
    x.push_back(1);

    auto y = GcUtils::getTranstionDataWithWrap(1, x);
    assert(y.first == 2);
    assert(y.second == 2);
    assert(x.size() == 4);
}

static void testTransitionData1() {
    std::vector<uint16_t> x;
    x.push_back(0);
    x.push_back(1);
    x.push_back(0);
    x.push_back(1);
    auto y = GcUtils::getTranstionData(5, x);
    assert(y.first == 0);
    assert(y.second == 3);
    assert(x.size() == 4);
}

static void testTransitionData2() {
    // empty vector
    std::vector<uint16_t> x;
    uint16_t d = 12;
    auto y = GcUtils::getTranstionData(1, x, d);
    assert(y.first == 0);
    assert(y.second == 0);
}

static void testDump() {
    printf("\n");
    GcUtils::dumpBinary(12, 0xfa);
    printf("\n");
    GcUtils::dumpBinary(12, 1);
    printf("\n");
    GcUtils::dumpBinary(12, 2);
    printf("\n");
}
static void testUtils() {
    testNumBitsDifferent0();
    testNumBitsDifferent1();
    testTransitionData0();
    testTransitionData1();
    testTransitionData2();

    testDifferByOneEmpty();
    testDifferByOne0();
    testDifferByOne1();
    testDifferByOne2();

    testTransitionDataWrap0();
    testTransitionDataWrap1();
    testDump();
    testGetWithRollover();
    testShortestRun0();
    testShortestRun1();
    testShortestRun2();
}


void gc_fill() {
    SQINFO("test: about to generate");

    /** retults:
    * nBits 1, unbalcanced, state size is 2, var=2,2, shortest run = 1
    * Bits 1, balcanced, state size is 2, var=2,2, shortest run = 1
    * nBits 2, unbalcanced,state size =4 var=2,2, shortest run = 2
    * nBits 2, balanced,state size =4 var=2,2, shortest run = 2
    * nBits 4, unbalanced, state size = 16, var = 2,8, shortest run = 2;
    * nBits 4, balance, var=4,4 shortest run = 2;
    * 
    */
    GcGenerator gc(4, true);
   // GcGenerator gc(4, true);
    gc.dump();
}


void testGrayCode() {
    testUtils();
    gc_fill();
   // assert(false);

}