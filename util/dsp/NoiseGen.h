#pragma once


#include "SimdBlocks.h"
#include "sq_rack.h"
#include "PinkFilter.h"

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
    PinkFilter<float_4> _pinkFilter;
 
    //  std::uniform_real_distribution<> dis( 0.0, 1.0);
};

// TODO: are these seeds spread out enough?
inline NoiseGen::NoiseGen(int base) : _base(base) {
  //  SQINFO("ng ctor, max=%f min=%f", double(std::mt19937::max()), double(std::mt19937::min()));
  //  SQINFO("max32= %f", double(std::numeric_limits<int>::max()));
  //  SQINFO("max32u= %f", double(std::numeric_limits<unsigned int>::max()));
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
    std::uniform_real_distribution<double> distribution(-.5, .5);
    float_4 white=0;
    float_4 ret;
    for (int i = 0; i < 4; ++i) {
        white[i] = (distribution(_gen32[i]));
    }

    if (isPink) {
        ret = _pinkFilter.process(white);

    } else {
        ret = white;
    }

    ret *= 4;
    return ret;
}

inline void NoiseGen::reset(float_4 flags) {
    for (int i = 0; i < 4; ++i) {
        if (SimdBlocks::isChannelTrue(i, flags)) {
            _gen32[i].seed(_getSeed(_base, i));
        }
    }
}