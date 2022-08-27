#pragma once

template <unsigned N>
class AdditivePitchLogic {
public:
    /** returns the aggregate pitch in VCV standard volts.
     */
    float getPitch(int harmonic);
};

template <unsigned N>
inline float AdditivePitchLogic<N>::getPitch(int harmonic) {
    return 0;
}