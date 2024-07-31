


#include "ScoreDrawUtils.h"

#include "MidiNote.h"

#include "asserts.h"

static void test1() {
    SqArray<int, 16> test = { MidiNote::MiddleC };
    ScoreDrawUtilsPtr utils = ScoreDrawUtils::make();
    utils->getDrawInfo(test);
    assertEQ(utils->_info.size(), 1);
}

void testScoreDrawUtils() {
    test1();
}


#if 1
void testFirst() {
    testScoreDrawUtils();
}

#endif