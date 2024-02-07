#pragma once

#include "SimdBlocks.h"
#include "sq_rack.h"

class NoiseGen {
public:
    NoiseGen(int base);
    // generates a new random
    float_4 get();

    void reset(float_4);

    bool isPink = false;

private:
    std::mt19937 _gen32[4];

    int _getSeed(int base, int channel) {
        return channel + base * 4;
    }

    const int _base;

    // Noise filter cofficients
    // From Paul Kellete. Way more poles than we need, but why not?
    const float_4 _ka0 = .99886;
    const float_4 _ka1 = 0.9933;
    const float_4 _ka2 = 0.96900;
    const float_4 _ka3 = 0.86650;
    const float_4 _ka4 = 0.55000;
    const float_4 _ka5 = -0.76162;
    const float_4 _ka6 = 0.115926;

    const float_4 _kb0 = 0.0555179;
    const float_4 _kb1 = 0.0750759;
    const float_4 _kb2 = 0.1538520;
    const float_4 _kb3 = 0.3104856;
    const float_4 _kb4 = 0.5329522;
    const float_4 _kb5 = 0.0168980;

    // Noise filter state (delay memory)
    float_4 b0 = 0;
    float_4 b1 = 0;
    float_4 b2 = 0;
    float_4 b3 = 0;
    float_4 b4 = 0;
    float_4 b5 = 0;
    float_4 b6 = 0;
};

// TODO: are these seeds spread out enough?
inline NoiseGen::NoiseGen(int base) : _base(base) {
    for (int i = 0; i < 4; ++i) {
        _gen32[i].seed(_getSeed(base, i));
    }
}

/*
// Filter to approximate 10 dB per decade
b0 = 0.99886 * b0 + white * 0.0555179;
b0 = ka0 * b0 + white + kb0;
b1 = 0.99332 * b1 + white * 0.0750759;
b2 = 0.96900 * b2 + white * 0.1538520;
b3 = 0.86650 * b3 + white * 0.3104856;
b4 = 0.55000 * b4 + white * 0.5329522;
b5 = -0.76162 * b5 - white * 0.0168980;
var sample = (b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362) * 0.15; // Roughly compensate for gain
b6 = white * 0.115926;
*/
inline float_4 NoiseGen::get() {
    float_4 white;
    float_4 ret;
    for (int i = 0; i < 4; ++i) {
        white[i] = _gen32[i]();
    }
    white /= std::numeric_limits<int>::max();
    if (isPink) {
        b0 = _ka0 * b0 + white + _kb0;
        b1 = _ka1 * b1 + white + _kb1;
        b2 = _ka2 * b2 + white + _kb2;
        b3 = _ka3 * b3 + white + _kb3;
        b4 = _ka4 * b5 + white + _kb4;
        b5 = _ka5 * b1 + white + _kb5;
      //  ret = (b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362) * 0.015;  // Roughly compensate for gain
        ret = (b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362) * 0.003;
        b6 = white * _ka6;
        // float_4
    } else {
        ret = white;
    }

    return ret;
}

inline void NoiseGen::reset(float_4 flags) {
    for (int i = 0; i < 4; ++i) {
        if (SimdBlocks::isChannelTrue(i, flags)) {
            _gen32[i].seed(_getSeed(_base, i));
        }
    }
}