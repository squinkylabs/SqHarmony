#pragma once

#include "sq_rack.h"

class NoiseGen {
public:
    NoiseGen(int base);
    // generates a new random
    float_4 get();

    void reset(float_4);
private:
    std::mt19937 gen32[4];
};

inline NoiseGen::NoiseGen(int base) {
    for (int i=0; i<4; ++i) {
        gen32[i].seed(i + base * 4);
    }
}

inline float_4 NoiseGen::get() {
    float_4 ret;
    for (int i=0; i<4; ++i) {
        ret[i] = gen32[i]();
    }
    return ret / std::numeric_limits<int>::max();
}

inline void NoiseGen::reset(float_4 flags) {
    
}