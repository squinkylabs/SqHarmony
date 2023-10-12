#include "LargeGapGrayCode.h"
#include "asserts.h"


void printBinaryCode(const std::vector<std::bitset<20> >& code, int bitWidth)
{
	printf("vector has %zd elements\n", code.size());
	assert(bitWidth < 20);
	const auto trim = 20 - bitWidth;
	for (auto i = 0; i < code.size(); ++i) {
		const std::bitset<20>& x = code[i];
		const std::string s = x.to_string();
		const std::string s2 = s.substr(trim, bitWidth);
		printf("word %3d = %s\n", i, s2.c_str());
		
	}

#if 0
	for (int j = 0; j < bitWidth; ++j)
	{
		for (int i = 0; i < pow(2, bitWidth); i++)
		{
			std::cout << code[i][j];
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
#endif
}

static void test0() {
   	LargeGapGrayCode lggc;
	std::cout << "####################################################################" << std::endl;
	std::cout << "####### 1)  Printing statistics for all Large-Gap Gray Codes #######" << std::endl;
	std::cout << "####################################################################" << std::endl << std::endl;
	lggc.printAllStatistics();

    std::cout << std::endl << std::endl;
	std::cout << "####################################################################" << std::endl;
	std::cout << "#######  2)  Printing statistics for different 16-bit codes  #######" << std::endl;
	std::cout << "####################################################################" << std::endl << std::endl;
	lggc.printStatisticsHeader();

	std::cout << "####################################################################" << std::endl;
	std::cout << "####### 3)  stats for 14, 2, 3, 1                            #######" << std::endl;
	std::cout << "####################################################################" << std::endl << std::endl;
	lggc.createCodeFromTheorem1(14, 2, 3, 1);
	std::cout << std::endl << std::endl;
	lggc.printStatistics(16);

	std::cout << "####################################################################" << std::endl;
	std::cout << "####### 3)  stats for 8, 8, 129, 127                         #######" << std::endl;
	std::cout << "####################################################################" << std::endl << std::endl;

	std::cout << std::endl << std::endl;
	lggc.createCodeFromTheorem1(8, 8, 129, 127);
	lggc.printStatistics(16);


	std::cout << "####################################################################" << std::endl;
	std::cout << "####### 3)  stats for 9, 7, 65, 63                         #######" << std::endl;
	std::cout << "####################################################################" << std::endl << std::endl;

	std::cout << std::endl << std::endl;
	lggc.createCodeFromTheorem1(9, 7, 65, 63);
	lggc.printStatistics(16);

    std::cout << "####################################################################" << std::endl;
	std::cout << "####### 3)  stats for 11, 5, 21, 11                         #######" << std::endl;
	std::cout << "####################################################################" << std::endl << std::endl;
	std::cout << std::endl << std::endl;
	lggc.createCodeFromTheorem1(11, 5, 21, 11);
	lggc.printStatistics(16);

	std::cout << std::endl << std::endl;
	std::cout << "####################################################################" << std::endl;
	std::cout << "#######       3)  Printing generated 7-bit binary code       #######" << std::endl;
	std::cout << "#######             Note: MinGap = 5 and MaxGap = 11         #######" << std::endl;
	std::cout << "####################################################################" << std::endl << std::endl;
	//printBinaryCode(lggc.getBinaryCode(7), 7);
	printBinaryCode(lggc.getBinaryCode(6), 6);
}

void testGrayCode2() {
    test0();
    assert(false);
}