
#pragma once

#include <assert.h>
#include <cmath>

#include "AudioMath.h"

class TestLFO {
public:
    // Normalized freq: 1 = fs.
    void setFreq(float f) { 
        assert(f >= 0 && f < .5);
        _freq = f * 2 * float(AudioMath::Pi);
    }

    // if false, sets cosine.
    void setSin(bool sineFlag) {
        // .25 had wrong sign.
        _phase = sineFlag ? (.75f * 2 * float(AudioMath::Pi)) : .0f;
    }

    // 1 will go from 0..1
    void setAmp(float a) { 
        assert(a >= 0);
        _amp = a;
    }

    float process() {
        const auto ret = std::cos(_phase);
        _phase += _freq;
        while (_phase >= 2 * AudioMath::Pi) {
            _phase -= 2 * float(AudioMath::Pi);
        }
        return ret * _amp;
    }
private:
    float _freq = 0;
    float _amp = 0;
    float _phase = 0;
};