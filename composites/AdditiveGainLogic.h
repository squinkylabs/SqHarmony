#pragma once

#include "AudioMath.h"
#include "SqLog.h"
#include <string>

template <unsigned N>
class AdditiveGainLogic {
public:
    AdditiveGainLogic();

    void setEven(float);
    void setOdd(float);

    /**
     * slope 1.f is flat
     * slope 0 is really rolled off
     */
    void setSlope(float);
    void setHarmonic(unsigned n, float value);
    //
    float getLevel(unsigned n) const;

    const float defaultLevel = .8f;

    void dump(const std::string& s) const;
private:
    float harmonicLevels[N];
    float _slope = 1;
   
};

template <unsigned N>
inline AdditiveGainLogic<N>::AdditiveGainLogic() {
    for (unsigned i=0; i<N; ++i) {
        harmonicLevels[i] = defaultLevel;
    }
}

template <unsigned N>
inline void AdditiveGainLogic<N>::setHarmonic(unsigned harmonic, float value) {
    if (harmonic >= N) {
        return;
    } 
    harmonicLevels[harmonic] = value;
}

template <unsigned N>
inline void AdditiveGainLogic<N>::setSlope(float value) {
    _slope = value;
}

template <unsigned N>
inline float AdditiveGainLogic<N>::getLevel(unsigned harmonic) const {
    //  assert(!_dirty);
    if (harmonic >= N) {
        return 0;
    }
    float value = harmonicLevels[harmonic];

    const float slopeAttenuationDB = harmonic * (1 - _slope);
    const float slopeGain = (float) AudioMath::gainFromDb(-slopeAttenuationDB);
   // SQINFO("slope=%f, harm=%d slopeGain =%f sadb=%f", _slope, harmonic, slopeGain, slopeAttenuationDB);
    value *= slopeGain;
    return value;
}

template <unsigned N>
inline void AdditiveGainLogic<N>::dump(const std::string& s) const {
    SQINFO("dumping AGL %s", s.c_str());
    for (unsigned i=0; i<N; ++i) {
        SQINFO("hlev[%d] = %f", i, harmonicLevels[i]);
    }
    SQINFO("slope=%f", _slope);
}