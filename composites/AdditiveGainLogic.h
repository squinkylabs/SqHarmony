#pragma once


template <unsigned N>
class AdditiveGainLogic {
public:
    void setEven(float);
    void setOdd(float);
    void setSlope(float);
    void setHarmonic(unsigned n, float value);
    //
    float getLevel(unsigned n) const;

    const float defaultLevel = .8f;

};

template <unsigned N>
inline float AdditiveGainLogic<N>::getLevel(unsigned harmonic) const {
    //  assert(!_dirty);
    if (harmonic >= N) {
        return 0;
    }
    return defaultLevel;
}