#pragma once

#include <string>

#include "AudioMath.h"
#include "SqLog.h"

template <unsigned N>
class AdditiveModLogic {
public:
    void setADSRValue(unsigned adsr, float value);
    void setADSRTrimValue(unsigned adsr, unsigned harmonic, float value);
    void setHarmonicBaseLevel(unsigned harmonic, float value);

    float getCombinedADSR(unsigned harmonic) const;
  
};

template <unsigned N>
inline float AdditiveModLogic<N>::getCombinedADSR(unsigned harmonic) const {
    return 0;
}