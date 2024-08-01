#pragma once

#include "SqArray.h"
#include "SharpsFlatsPref.h"

#include <map>
#include <memory>



class Scale;
class ScoreDrawUtils;
using ScoreDrawUtilsPtr = std::unique_ptr<ScoreDrawUtils>;
class ScoreDrawUtils {
public:
    static ScoreDrawUtilsPtr make();
    // (scale, input, bassStaff, pref
    void getDrawInfo(
        const Scale& scale,
        const SqArray<int, 16>& input,
        bool bassStaff,
        UIPrefSharpsFlats pref);

    class LegerLineInfo {
    public:
        class SymbolInfo {
            char glyph[4];
            float xPosition;
            float yPosition;
        };

        SymbolInfo symbols[4];
        unsigned numSymbols = 0;

    };
    std::map<int, LegerLineInfo> _info;
};


