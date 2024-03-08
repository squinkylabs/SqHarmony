#include "rack.hpp"

using namespace rack;

// #define _ADD
#define _PM
// #define _MULT
//#define _LFN
//#define _H2

extern Plugin* pluginInstance;

extern Model* modelHarmony1;
extern Model* modelArpeggiator1;
#ifdef _ADD
extern Model* modelAdditive;
#endif
#ifdef _PM
extern Model* modelPhasePatterns;
#endif
#ifdef _MULT
extern Model* modelMult;
#endif
#ifdef _LFN
extern Model* modelLFN2;
#endif
#ifdef _H2
extern Model* modelHarmony2;
#endif