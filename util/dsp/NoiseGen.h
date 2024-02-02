#pragma once

#include "sq_rack.h"

class NoiseGen {
public:
    // generates a new random
    float_4 get();
private:
    std::mt19937 gen32[4];
};

inline float_4 NoiseGen::get() {
    float_4 ret;
    for (int i=0; i<4; ++i) {
        ret[i] = gen32[i]();
    }
    //return {gen32[3]()};
    return ret;
}