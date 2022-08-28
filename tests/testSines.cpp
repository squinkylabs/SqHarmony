
#include "SinesVCO.h"

#include "asserts.h"

static void testSines0()
{
    SinesVCO<float_4> v;
    float_4 deltaT = 1.f / 44100.f;
    float_4 x = v.process(deltaT);
   simd_assertClose(x, float_4(0), .00001);
}


static void testSines1()
{
    SinesVCO<float_4> v;
    float_4 pitch(0);
    pitch[1] = 2;
    v.setPitch(pitch, 44100.f);
    float_4 deltaT = 1.f / 44100.f;
    float_4 x = v.process(deltaT);

    float_4 expected(.036f);
    expected[1] = .146f;
    simd_assertClose(x, expected, .001);
   
}

void testSines()
{
    testSines0();
    testSines1();
}