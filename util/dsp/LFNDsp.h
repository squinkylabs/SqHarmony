#pragma once

#include "asserts.h"

#include "Filter4pButter.h"
#include "NoiseGen.h"
#include "simd.h"

class LFNDsp {
public:
    LFNDsp(int base);

    float_4 process();

    void genStraightNoise(bool);

private:
    // Eventually will have noise gen -> geq -> decimator -> lpf
    // all simd
    NoiseGen _noiseGen;
    Filter4PButter<float_4> _lpf;
};

inline void LFNDsp::genStraightNoise(bool) {
}

inline LFNDsp::LFNDsp(int base) : _noiseGen(base) {
    _noiseGen.isPink = true;
    _lpf.setCutoffFreq(.01f);
}

inline float_4 LFNDsp::process() {
    float_4 temp = _noiseGen.get();
  //  temp = _lpf.process(temp);
    return temp;
}
