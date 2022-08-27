#pragma once

#include <cmath>
#include "SqLog.h"

template <unsigned N>
class AdditivePitchLogic {
public:
    /** returns the aggregate pitch in VCV standard volts.
     */
    float getPitch(int harmonic) const;
    
    // VC is VCV standard
    void setCV(float cv);
    void setOctave(int oct);
    void setSemitone(int semi);

    // 0 is no stretch, 1 is max, -1 is max "unstretch"
    void setStretch(float stretch);

    void setEvenOffset(float offset);
    void setOddOffset(float offset);
    void setOffset(float offset);


};

template <unsigned N>
inline float AdditivePitchLogic<N>::getPitch(int harmonic) const {
    const auto lgh = std::log2(float(harmonic+1));
    SQINFO("harm=%d, log=%f", harmonic, lgh);
    return lgh;
}