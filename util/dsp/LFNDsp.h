#pragma once

#include "simd.h"
#include "NoiseGen.h"

class LFNDsp {
public:
    LFNDsp(int base);

    float_4 process();
private:
    NoiseGen _noiseGen;
};

inline LFNDsp::LFNDsp(int base) : _noiseGen(base) {
    _noiseGen.isPink = true;
}

inline float_4 LFNDsp::process() {
    return _noiseGen.get();
}
