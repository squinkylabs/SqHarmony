#pragma once

#include <assert.h>

#include <cmath>

#include "SqLog.h"

template <unsigned N>
class AdditivePitchLogic {
public:
    /** returns the aggregate pitch in VCV standard volts.
     */
    float getPitch(unsigned harmonic) const;

    // VC is VCV standard
    void setCV(float cv);
    void setOctave(int oct);
    void setSemitone(int semi);
    void setFine(float fine);

    // 0 is no stretch, 1 is max, -1 is max "unstretch"
    void setStretch(float stretch);

    void setEvenOffset(float offset);
    void setOddOffset(float offset);
    void setOffset(float offset);

private:
    float _cv{0};
    float _octave{0};
    float _semitone{0};
    float _stretch{0};
    float _fine{0};
};

template <unsigned N>
inline float AdditivePitchLogic<N>::getPitch(unsigned harmonic) const {
    //  assert(!_dirty);
    if (harmonic >= N) {
        return 0;
    }

    float stretchedHarmonic = float(harmonic) * (1.f + .1f * _stretch);
    //const auto lgh = std::log2(float(harmonic + 1));
    const auto lgh = std::log2(stretchedHarmonic + 1);

    // SQINFO("harm=%d, log=%f", harmonic, lgh);
    return lgh + _cv + _octave + (_semitone / 12) + (_fine / 12);
}

template <unsigned N>
inline void AdditivePitchLogic<N>::setCV(float cv) {
    _cv = cv;
}

template <unsigned N>
inline void AdditivePitchLogic<N>::setOctave(int octave) {
    _octave = float(octave);
}

template <unsigned N>
inline void AdditivePitchLogic<N>::setSemitone(int semi) {
    _semitone = float(semi);
}

template <unsigned N>
inline void AdditivePitchLogic<N>::setFine(float fine) {
    _fine = fine;
}

template <unsigned N>
inline void AdditivePitchLogic<N>::setStretch(float stretch) {
    _stretch = stretch;
}

template <unsigned N>
 void AdditivePitchLogic<N>::setEvenOffset(float offset) {

}

 template <unsigned N>
void AdditivePitchLogic<N>::setOddOffset(float offset) {

}