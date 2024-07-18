
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
