
#include <assert.h>

#include <iostream>

#include "AudioMath.h"
// #include "LookupTableFactory.h"
#include "asserts.h"

#ifndef _MSC_VER
#include "SimdBlocks.h"
#endif

using namespace std;

static void test0() {
    assert(AudioMath::closeTo(0, 0, .000001));
    assert(AudioMath::closeTo(1000, 1001, 1.1));
    assert(!AudioMath::closeTo(1000, 1010, 1.1));
    assert(!AudioMath::closeTo(1010, 1000, 1.1));
}

static void test1() {
    assert(AudioMath::closeTo(3.145, AudioMath::Pi, .1));
    assert(AudioMath::closeTo(3.145 / 2, AudioMath::Pi_2, .1));
    assert(AudioMath::closeTo(log(10), AudioMath::Ln10, .001));
}

static void test2() {
    const double d = .0001;
    std::function<double(double)> f = AudioMath::makeFunc_Sin();
    assert(AudioMath::closeTo(f(0), 0, d));
    assert(AudioMath::closeTo(f(.5), 0, d));
    assert(AudioMath::closeTo(f(.25), 1, d));
    assert(AudioMath::closeTo(f(.75), -1, d));
    assert(AudioMath::closeTo(f(.125), 1.0 / sqrt(2), d));
}

static void test3() {
    const double d = .0001;
    std::function<double(double)> f = AudioMath::makeFunc_Exp(0, 4, 2, 32);

    assert(AudioMath::closeTo(f(0), 2, d));
    assert(AudioMath::closeTo(f(1), 4, d));
    assert(AudioMath::closeTo(f(2), 8, d));
    assert(AudioMath::closeTo(f(3), 16, d));
    assert(AudioMath::closeTo(f(4), 32, d));
    assert(f(5) > 33);
    assert(f(-1) < 1.5);
}

static void testInverseExp() {
    const double d = .0001;
    std::function<double(double)> f = AudioMath::makeFunc_InverseExp(0, 4, 2, 32);
    assertClose(f(2), 0, d);
    assertClose(f(32), 4, d);
    assertClose(f(4), 1, d);
    assertClose(f(8), 2, d);
    assertClose(f(16), 3, d);
}

static void testAudioTaper() {
    double db = -18;
    std::function<double(double)> f = AudioMath::makeFunc_AudioTaper(db);
    assertClose(f(1), 1, .001);
    assertClose(f(.25), .125, .001);
    assertClose(f(.251), .126, .001);
    assertClose(f(.249), 1.0 / 8.0, .001);
    assertClose(f(0), 0, .001);
}

static void testInverseAudioTaper(double db) {
    std::function<double(double)> f = AudioMath::makeFunc_AudioTaper(db);
    std::function<double(double)> fi = AudioMath::makeFunc_InverseAudioTaper(db);
    std::function<void(double)> test = [f, fi](double d) {
        double tap = f(d);
        double inv = fi(tap);
        assertClose(inv, d, .0001);
    };

    for (double d = 0; d <= 1; d += .01) {
        test(d);
    }
}

static void testInverseAudiTaper() {
    testInverseAudioTaper(-18);
}

// Tests both linear scalers with domain -5..5 for
// both knob and CV
static void testScalerSub(bool useScaler2) {
    AudioMath::ScaleFun<float> f = useScaler2 ? AudioMath::makeLinearScaler2<float>(-5, 5, 3, 4) : AudioMath::makeLinearScaler<float>(3, 4);
    // scale(cv, knob, trim

    // knob comes through only shifted
    assertEQ(f(0, -5, 0), 3.);
    assertEQ(f(0, 5, 0), 4.);
    assertEQ(f(0, 0, 0), 3.5);

    // cv also come through, it trim up
    assertEQ(f(-5, 0, 1), 3.);
    assertEQ(f(5, 0, 1), 4.);
    assertEQ(f(0, 0, 1), 3.5);

    // (T cv, T knob, T trim)
    // no cv if trim 0
    // knob in middle
    assertEQ(f(-5, 0, 0), 3.5);

    // neg trim inverts cv
    assertEQ(f(-5, 0, -1), 4.);

    // trim half way
    assertEQ(f(5, 0, .5), 3.75);

    // way down, should clip
    assertEQ(f(-5, -5, 1), 3);
    // way up, should clip
    assertEQ(f(5, 5, 1), 4);
}

static void testScaler() {
    // a = .1 b = 3.5
    auto xx = AudioMath::makeLinearScaler<float>(3, 4);

    // both a .1, b = 3.5
    auto yy = AudioMath::makeLinearScaler2<float>(-5, 5, 3, 4);

    xx(-5, 0, 0);
    yy(-5, 0, 0);

    testScalerSub(false);
}

static void testScaler2() {
    testScalerSub(true);
}

static void testScaler3() {
    // (T cv, T knob, T trim)
    AudioMath::ScaleFun<float> f = AudioMath::makeLinearScaler2<float>(1, 31, 3, 4);
    // knob comes through shifted and scaled
    assertEQ(f(0, 1, 0), 3.);
    assertEQ(f(0, 31, 0), 4.);
    assertEQ(f(0, 16, 0), 3.5);

    // cv also come through, if trim up
    assertEQ(f(-5, 1, 1), 3.);  // cv at min, knob at min will go below min, clip
    assertEQ(f(5, 31, 1), 4.);  // cv at max, knob at max, will to above, clip

    // could use some more tests here....
    //  assertEQ(f(0, 1, 1), 3.5);
}

#if 0  // not in this version
static void testBipolarAudioScaler() {
    AudioMath::ScaleFun<float> f = AudioMath::makeScalerWithBipolarAudioTrim(3, 4);
    // scale(cv, knob, trim

    // knob comes through only shifted
    assertEQ(f(0, -5, 0), 3.);
    assertEQ(f(0, 5, 0), 4.);
    assertEQ(f(0, 0, 0), 3.5);

    // cv also come through, it trim up
    assertEQ(f(-5, 0, 1), 3.);
    assertEQ(f(5, 0, 1), 4.);
    assertEQ(f(0, 0, 1), 3.5);

    // no cv if trim 0
    assertEQ(f(-5, 0, 0), 3.5);

    // neg trim inverts cv
    assertEQ(f(-5, 0, -1), 4.);

    // trim quarter  - should be audio knee
    auto f2 = AudioMath::makeScalerWithBipolarAudioTrim(-1, 1);
    float x = f2(5, 0, .25);
    float y = (float)AudioMath::gainFromDb(LookupTableFactory<float>::audioTaperKnee());
    assertClose(x, y, .001);
}

static void testAudioScaler() {
    AudioMath::SimpleScaleFun<float> f = AudioMath::makeSimpleScalerAudioTaper(3, 4);

    float kneeGain = (float)AudioMath::gainFromDb(LookupTableFactory<float>::audioTaperKnee());

    // knob comes through with taper
    assertEQ(f(0, -5), 3.);
    assertEQ(f(0, 5), 4.);

    // 1/4, is the knee
    assertEQ(f(0, -2.5), 3.0f + kneeGain);

    // cv also come through, it trim up
    assertEQ(f(-5, 0), 3.);
    assertEQ(f(5, 0), 4.);
    assertEQ(f(-2.5, 0), 3.0f + kneeGain);
}

static void testAudioScaler2() {
    AudioMath::SimpleScaleFun<float> f = AudioMath::makeSimpleScalerAudioTaper(0, 20);

    float kneeGain = (float)AudioMath::gainFromDb(LookupTableFactory<float>::audioTaperKnee());

    // knob comes through with taper
    assertEQ(f(0, -5), 0);
    assertEQ(f(0, 5), 20);

    // 1/4, is the knee
    assertEQ(f(0, -2.5), 20 * kneeGain);

    // cv also come through, it trim up
    assertEQ(f(-5, 0), 0);
    assertEQ(f(5, 0), 20);
    assertEQ(f(-2.5, 0), 20 * kneeGain);
}
#endif

static void testFold() {
    float last = 0;
    const float deltaX = 0.05f;
    for (float x = 0; x < 15; x += deltaX) {
        float y = AudioMath::fold(x);
        float absChange = std::abs(y - last);
        last = y;
        assertLE(absChange, deltaX + .0001f);
    }
}

static void testFoldNegative() {
    float last = 0;
    const float deltaX = 0.05f;
    for (float x = 0; x > -15; x -= deltaX) {
        float y = AudioMath::fold(x);
        float absChange = std::abs(y - last);
        last = y;
        assertLE(absChange, deltaX + .0001f);
    }
}

#ifndef _MSC_VER
void pr(float_4 x);

static void testFoldSSE() {
    // a few well know fixed values for sanity
    simd_assertEQ(SimdBlocks::fold(0), float_4(0));
    simd_assertEQ(SimdBlocks::fold(.5), float_4(.5));
    simd_assertEQ(SimdBlocks::fold(-.5), float_4(-.5));
    simd_assertEQ(SimdBlocks::fold(.9), float_4(.9));
    simd_assertEQ(SimdBlocks::fold(-.9), float_4(-.9));
    simd_assertClose(SimdBlocks::fold(1.2), float_4(.8), .0001);
    simd_assertClose(SimdBlocks::fold(-1.2), float_4(-.8), .0001);
}

static void testFoldSSE2() {
    const float deltaX = 0.05f;
    for (float x = -15; x < 15; x += deltaX) {
        const float y = AudioMath::fold(x);
        const float_4 y4 = SimdBlocks::fold(x);
        simd_assertClose(y4, float_4(y), .0001);
    }
}

static void testFoldSSE3() {
    const float test[4] = {.1, 2.3, -1, -40.3};
    float output[4] = {0, 0, 0, 0};

    float_4 input = float_4::load(test);
    float_4 temp = SimdBlocks::fold(input);
    temp.store(output);

    for (int i = 0; i < 4; ++i) {
        assertClose(output[i], AudioMath::fold(test[i]), .0001);
    }
}

static void testWrapPhase() {
    for (float x = 0; x < 10; x += .1) {
        float_4 x4(x);
        simd_assertGE(SimdBlocks::wrapPhase01(x4), float_4(0));
        simd_assertLE(SimdBlocks::wrapPhase01(x4), float_4(1));
    }
}

static void testWrapPhase2() {
    simd_assertEQ(SimdBlocks::wrapPhase01(0), float_4(0));
    simd_assertEQ(SimdBlocks::wrapPhase01(0.1), float_4(0.1));
    simd_assertEQ(SimdBlocks::wrapPhase01(0.9), float_4(0.9));

    simd_assertEQ(SimdBlocks::wrapPhase01(1.0), float_4(0));
    simd_assertClose(SimdBlocks::wrapPhase01(1.2), float_4(0.2), .00001);
    simd_assertClose(SimdBlocks::wrapPhase01(1.9), float_4(0.9), .00001);

    simd_assertClose(SimdBlocks::wrapPhase01(1000.9), float_4(0.9), .0001);

    simd_assertEQ(SimdBlocks::wrapPhase01(-.1), float_4(.9));
}
#endif

static void testNormalizeProduct() {
    float data1[] = {1, 1, 1, 1};
    float data2[] = {1, 10, 100};
    AudioMath::normalizeProduct(data1, 4);
    AudioMath::normalizeProduct(data2, 3);

    for (int i = 0; i < 4; ++i) {
        assertEQ(data1[i], 1);
    }

    assertClose(data2[0] * data2[1] * data2[2], 1, .001);
    assertClose(data2[0] * 10, data2[1], .001);
    assertClose(data2[1] * 10, data2[2], .001);
}

static void testDistributeEvenly() {
    float buffer[10];
    AudioMath::distributeEvenly(buffer, 4, 1.f);
    for (int i = 0; i < 4; ++i) {
        assertEQ(buffer[i], 1);
    }
}

// test "coin toss" - expect 50/50
static void testRandom1(AudioMath::RandomUniformFunc rand, float tolerance) {
    for (int iter = 0; iter < 10; ++iter) {
        int high = 0;
        int low = 0;

        for (int i = 0; i < 10000; ++i) {
            float x = rand();
            if (x >= .5f) {
                high++;
            } else {
                low++;
            }
        }
        // A little strange I had to make the threshold so high
        // 4 works with vc, old random
        assertClosePct(float(high), float(low), tolerance);
    }
}

static void testRandom1() {
    testRandom1(AudioMath::random(), 5.f);
    // 2 ok for VS
    testRandom1(AudioMath::random_better(), 3.f);
}

static void testRandom2(AudioMath::RandomUniformFunc rand, float tolerancePct) {
    //   auto rand = AudioMath::random();

    bool last = false;
    int tt = 0;
    int tf = 0;
    int ff = 0;
    int ft = 0;
    const int iterations = 10000;

    for (int i = 0; i < iterations; ++i) {
        float x = rand();
        bool b = x >= .5f;
        if (b && last) {
            tt++;
        } else if (b && !last) {
            ft++;
        } else if (!b && !last) {
            ff++;
        } else if (!b && last) {
            tf++;
        } else {
            assert(false);
        }
        last = b;
    }

    const float expected = float(iterations) / 4.f;
    //  const float tolerancePct = 4.f;
    assertClosePct(tt, expected, tolerancePct);
    assertClosePct(tf, expected, tolerancePct);
    assertClosePct(ff, expected, tolerancePct);
    assertClosePct(ft, expected, tolerancePct);
}

static void testRandom2() {
    testRandom2(AudioMath::random(), 4.f);
    testRandom2(AudioMath::random_better(), 3.f);
}

static void testRandom3(AudioMath::RandomUniformFunc rand, float tolerance) {
    const int iterations = 100000;

    int trueInARow = 0;
    int fourInARow = 0;
    int eightInARow = 0;

    int falseInARow = 0;
    int fourfInARow = 0;
    int eightfInARow = 0;

    for (int i = 0; i < iterations; ++i) {
        float x = rand();
        bool b = x >= .5f;
        // SQINFO("b=%d", b);
        if (b) {
            trueInARow++;
            falseInARow = 0;
        } else {
            trueInARow = 0;
            falseInARow++;
        }

        if (trueInARow == 4) {
            fourInARow++;
        }
        if (trueInARow == 8) {
            eightInARow++;
        }

        if (falseInARow == 4) {
            fourfInARow++;
        }
        if (falseInARow == 8) {
            eightfInARow++;
        }
    }
#if 0
    //SQINFO("iter=%d,%d,%d,%d,%d", iterations, iterations / 2, iterations / 4, iterations / 8, iterations / 16);
    //SQINFO("iter=%d,%d,%d,%d,%d", iterations / 32, iterations / 64, iterations / 128, iterations / 256, iterations / 512);
    //SQINFO(".5**4 = %f, .5**8=%f", std::pow(.5, 4), std::pow(.5, 8));
    //SQINFO("iterX .5**4 = %f, .5**8=%f", iterations * std::pow(.5, 4), iterations * std::pow(.5, 8));
    //SQINFO("four = %d, eight=%d", fourInARow, eightInARow);
    //SQINFO("four F = %d, eightF=%d", fourfInARow, eightfInARow);
#endif

    assertClosePct(fourInARow, iterations / 32, tolerance);
    assertClosePct(eightInARow, iterations / 512, tolerance * 8);
    assertClosePct(fourfInARow, iterations / 32, tolerance);
    assertClosePct(eightfInARow, iterations / 512, tolerance * 8);

    // assert(false);
}

static void testRandom3() {
    testRandom3(AudioMath::random(), 2);
    testRandom3(AudioMath::random_better(), 1);
}

static void testModBipolar() {
    assertEQ(AudioMath::modBipolarAndLimit(4, 5), 4);
    assertEQ(AudioMath::modBipolarAndLimit(5, 4), 1);
    assertEQ(AudioMath::modBipolarAndLimit(5, 5), 0);

    assertEQ(AudioMath::modBipolarAndLimit(0, 10), 0);
    assertEQ(AudioMath::modBipolarAndLimit(10, 10), 0);
    assertEQ(AudioMath::modBipolarAndLimit(20, 10), 0);
    assertEQ(AudioMath::modBipolarAndLimit(-10, 10), 0);
    assertEQ(AudioMath::modBipolarAndLimit(-20, 10), 0);

    assertEQ(AudioMath::modBipolarAndLimit(1, 10), 1);
    assertEQ(AudioMath::modBipolarAndLimit(11, 10), 1);
    assertEQ(AudioMath::modBipolarAndLimit(21, 10), 1);
    assertEQ(AudioMath::modBipolarAndLimit(-9, 10), 1);
    assertEQ(AudioMath::modBipolarAndLimit(-19, 10), 1);
    assertEQ(AudioMath::modBipolarAndLimit(-29, 10), 1);

    assertEQ(AudioMath::modBipolarAndLimit(-1, 57), 56);
}


static void testMakeFunc_QuantizeAndWrap() {
    #if 0
    const auto func = AudioMath::makeFunc_QuantizeAndWrap<float>(10);
    assertEQ(func(0), 0);
    assertEQ(func(.9), 9);
    assertEQ(func(1), 0);
    assertEQ(func(1.1), 1);

    assertEQ(func(.1), 1);
    assertEQ(func(.051), 1);
    assertEQ(func(.049), 0);

    assertEQ(func(-1), 0);
    assertEQ(func(-2), 0);
    assertEQ(func(-30), 0);

    assertEQ(func(-.5), 5);
    assertEQ(func(-.4), 6);
#endif
}

static void testFloat2int() {
    assertEQ(AudioMath::float2int(0), 0);
    assertEQ(AudioMath::float2int(1), 1);
    assertEQ(AudioMath::float2int(-44), -44);

    assertEQ(AudioMath::float2int(0.49), 0);
    assertEQ(AudioMath::float2int(0.51), 1)

    assertEQ(AudioMath::float2int(10.49), 10);
    assertEQ(AudioMath::float2int(10.51), 11)

    assertEQ(AudioMath::float2int(-10.49), -10);
    assertEQ(AudioMath::float2int(-10.51), -11)
}

static void testFloat2intScale() {
    assertEQ(AudioMath::float2intScale(0, 0), 0);
    assertEQ(AudioMath::float2intScale(0, 10), 0);
    assertEQ(AudioMath::float2intScale(10, 0), 0);

    assertEQ(AudioMath::float2intScale(1, 1), 1);
    assertEQ(AudioMath::float2intScale(1, 12), 12);

    assertEQ(AudioMath::float2intScale(1.5, 12), 18);
    assertEQ(AudioMath::float2intScale(1.51, 12), 18);

}


void testAudioMath() {
    test0();
    test1();
    test2();
    test3();
    testInverseExp();
    testAudioTaper();
    testInverseAudiTaper();
    testScaler();
    testScaler2();
    testScaler3();
    // testBipolarAudioScaler();
    // testAudioScaler();
    // testAudioScaler2();
    // testFold();
    // testFoldNegative();
    // testNormalizeProduct();
    // testDistributeEvenly();
#ifndef _MSC_VER
    testFoldSSE();
    testFoldSSE2();
    testFoldSSE3();
    testWrapPhase();
    testWrapPhase2();
#endif
    testRandom1();
    testRandom2();
    testRandom3();
    testFloat2int();
    testFloat2intScale();
    testModBipolar();
    testMakeFunc_QuantizeAndWrap();
}

#if 1
void testFirst() {
    testFloat2intScale();
    // makeFunc_QuantizeAndWrap
  //  testModBipolar();
  //  testMakeFunc_QuantizeAndWrap();
}
#endif