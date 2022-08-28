#pragma once

#include <cmath>
#include <assert.h>

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
private:
    float _cv{0};
//    bool _dirty{false};
};

template <unsigned N>
inline float AdditivePitchLogic<N>::getPitch(int harmonic) const {
  //  assert(!_dirty);
    const auto lgh = std::log2(float(harmonic + 1));
    // SQINFO("harm=%d, log=%f", harmonic, lgh);
    return lgh + _cv;
}

template <unsigned N>
inline void AdditivePitchLogic<N>::setCV(float cv) {
    if (cv == _cv) {
        return;
    }

    _cv = cv;
   // _dirty = true;
}