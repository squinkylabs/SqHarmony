#include "LargeGapGrayCode.h"
#include "asserts.h"

static void test0() {
   	LargeGapGrayCode lggc;
	std::cout << "####################################################################" << std::endl;
	std::cout << "####### 1)  Printing statistics for all Large-Gap Gray Codes #######" << std::endl;
	std::cout << "####################################################################" << std::endl << std::endl;
	lggc.printAllStatistics();
}

void testGrayCode2() {
    test0();
    assert(false);
}