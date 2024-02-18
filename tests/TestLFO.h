
#pragma once

#include <assert.h>

#define _USE_MATH_DEFINES
//#include <math.h>
#include <cmath>

class TestLFO {
public:
    // Normalized freq: 1 = fs.
    void setFreq(float f) { 
        assert(f >= 0 && f < .5);
        _freq = f * 2 * float(M_PI);
    }

    // if false, sets cosine.
    void setSin(bool sineFlag) {
        _phase = sineFlag ? (.25f * 2 * float(M_PI)) : .0f;
    }

    // 1 will go from 0..1
    void setAmp(float a) { 
        assert(a >= 0);
        _amp = a;
    }

    float process() {
        const auto ret = std::cos(_phase);
        _phase += _freq;
        while (_phase >= 2 * M_PI) {
            _phase -= 2 * float(M_PI);
        }
        return ret * _amp;
    }
private:
    float _freq = 0;
    float _amp = 0;
    float _phase = 0;
};