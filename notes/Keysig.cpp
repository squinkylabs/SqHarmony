
#include "Keysig.h"

Keysig::Keysig(Roots rt) : root(rt) {
    nDegreeTable[0] = 0;  // 0 is not used
    nDegreeTable[1] = 1;
    nDegreeTable[2] = 0;
    nDegreeTable[3] = 2;
    nDegreeTable[4] = 0;
    nDegreeTable[5] = 3;
    nDegreeTable[6] = 4;
    nDegreeTable[7] = 0;
    nDegreeTable[8] = 5;
    nDegreeTable[9] = 0;
    nDegreeTable[10] = 6;
    nDegreeTable[11] = 0;
    nDegreeTable[12] = 7;
}

