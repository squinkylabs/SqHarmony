#pragma once

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
};

inline void LFNDsp::genStraightNoise(bool) {
}

inline LFNDsp::LFNDsp(int base) : _noiseGen(base) {
    _noiseGen.isPink = true;
}

inline float_4 LFNDsp::process() {
    return _noiseGen.get();
}
