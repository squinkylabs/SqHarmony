
#pragma once

//#include "Scale.h"


enum class SharpsFlatsPref {
    Sharps,
    Flats,
    DontCare
};

enum class ResolvedSharpsFlatsPref {
    Sharps,
    Flats
};


enum class UIPrefSharpsFlats {
    DefaultPlusSharps,
    DefaultPlusFlats,
    Sharps,
    Flats
};


inline UIPrefSharpsFlats float2Pref(float x) {
    if (x < .5)
        return UIPrefSharpsFlats::DefaultPlusSharps;
    if (x < 1.5)
        return UIPrefSharpsFlats::DefaultPlusFlats;
    if (x < 2.5)
        return UIPrefSharpsFlats::Sharps;

    return UIPrefSharpsFlats::Flats;
}


extern bool resolveSharpPref(UIPrefSharpsFlats pref, const class Scale& scale );
