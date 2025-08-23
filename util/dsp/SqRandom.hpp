
#pragma once

#include "sq_rack.h"
#include <cstdint>

// Just used to work around a bug in VCV sdk. 
// Thin wrapper around rack::random::Xoroshiro128Plus.
// Lets us use std::uniform_real_distribution.
class myxoro {
public:
    using result_type = uint64_t;
    void seed(result_type s1, result_type s2) { _random.seed(s1, s2); }
    result_type min() { return _random.min(); }
    result_type max() { return _random.max(); }
    result_type operator()() { return _random(); }

private:
    rack::random::Xoroshiro128Plus _random;
};

class SqRandom {
public:
    SqRandom(uint64_t seed1, uint64_t seed2) : _uniform(0, 1) {
        _random.seed(seed1, seed2);
    }
    // returns 0...1
    double generateDouble() {
        const auto x = _uniform(_random);
        return x;
    }
    // generates random between 0..num-1
    int generateInteger(int num) {
        assert(num >= 1);
        return std::trunc(generateDouble() * (num));
    }
private:
    myxoro _random;
    std::uniform_real_distribution<double> _uniform;
};