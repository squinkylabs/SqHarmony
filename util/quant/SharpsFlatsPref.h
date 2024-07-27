
#pragma once
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


#if 0
class AccidentalResolver {
public:
    AccidentalResolver() = delete;
    static ResolvedSharpsFlatsPref getPref() { return ResolvedSharpsFlatsPref::Sharps; }
    static ResolvedSharpsFlatsPref resolve(SharpsFlatsPref input) {
        switch(input) {
            case SharpsFlatsPref::Sharps:
                return ResolvedSharpsFlatsPref::Sharps;
            case SharpsFlatsPref::Flats:
                return ResolvedSharpsFlatsPref::Flats;
            case SharpsFlatsPref::DontCare:
                return getPref();
            default:
                assert(false);
                return getPref();
        }
    }
};
#endif
