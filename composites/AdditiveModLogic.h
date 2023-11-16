#pragma once

#include <string>

#include "AudioMath.h"
#include "SqLog.h"

//#define _LOG 1

template <unsigned N>
class AdditiveModLogic {
public:
    const unsigned numADSR = 3;
    AdditiveModLogic();
    void setADSRValue(unsigned adsr, float value, bool connected);
    void setADSRTrimValue(unsigned adsr, unsigned harmonic, float value);
    //  void setHarmonicBaseLevel(unsigned harmonic, float value);

    float getCombinedADSR(unsigned harmonic) const;

private:
    float harmonicTrimADSR[3][N] = {{0}};
    float adsrValue[3] = {0};
    bool adsrConnected[3] = {0};
    //    float harmonicBaseLevel[N] = {0}
};

template <unsigned N>
inline AdditiveModLogic<N>::AdditiveModLogic() {
    assert(numADSR == 3);
}

template <unsigned N>
inline float AdditiveModLogic<N>::getCombinedADSR(unsigned harmonic) const {
#ifdef _LOG
    if (harmonic == _LOG) {
        SQINFO("AdditiveModLogic<N>::getCombinedADSR harm=%d", harmonic);
    }
#endif
    float value = 1;

    for (unsigned i = 0; i < numADSR; ++i) {
#ifdef _LOG
        if (harmonic == _LOG) {
            SQINFO(" for adsr %d, is connected %d (value=%f)", i, adsrConnected[i], value);
        }
#endif
        if (adsrConnected[i]) {
            float x = adsrValue[i] * harmonicTrimADSR[i][harmonic];
            value *= x;
#ifdef _LOG
            if (harmonic == _LOG) {
                SQINFO(" adsr value = %f, trim = %f, value = %f", adsrValue[i], harmonicTrimADSR[i][harmonic], value);
            }
#endif
        }
    }
#ifdef _LOG
    if (harmonic == _LOG) {
        SQINFO(" leaveing get combined, value = %f", value);
    }
#endif
    return value;
}

template <unsigned N>
void AdditiveModLogic<N>::setADSRValue(unsigned adsr, float value, bool connected) {
    assert(adsr < numADSR);
    adsrValue[adsr] = value;
    adsrConnected[adsr] = connected;

#ifdef _LOG
    if (true) {
        SQINFO("AdditiveModLogic<N>::setADSRValue adsr=%d, value=%f, connected=%d", adsr, value, connected);
    }
#endif
}

template <unsigned N>
void AdditiveModLogic<N>::setADSRTrimValue(unsigned adsr, unsigned harmonic, float value) {
    assert(adsr < numADSR);
    harmonicTrimADSR[adsr][harmonic] = value;
}

/*
template <unsigned N>
void AdditiveModLogic<N>::setHarmonicBaseLevel(unsigned harmonic, float value) {

}
*/