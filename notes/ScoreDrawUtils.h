#pragma once

#include <functional>
#include <map>
#include <memory>

#include "ScorePitchUtils.h"
#include "SharpsFlatsPref.h"
#include "SqArray.h"

class Scale;
class ScoreDrawUtils;
using ScoreDrawUtilsPtr = std::unique_ptr<ScoreDrawUtils>;
class ScoreDrawUtils {
public:
    class DrawPosition {
    public:
        std::function<float(const MidiNote& note, int legerLine, bool bassStaff)> noteYPosition;
        float noteXPosition = 0;
    };
    static ScoreDrawUtilsPtr make();
    // (scale, input, bassStaff, pref
    void getDrawInfo(
        const DrawPosition& pos,
        const Scale& scale,
        const SqArray<int, 16>& input,
        UIPrefSharpsFlats pref);

    class LegerLineInfo {
    public:
        class SymbolInfo {
        public:
            std::string glyph;
            float xPosition;
            float yPosition;
        };

        SymbolInfo symbols[4];
        unsigned numSymbols = 0;
        void addOne(const std::string& glyph, float xPosition, float yPosition) {
            assert(numSymbols < 4);
            SymbolInfo si;
            si.glyph = glyph;
            si.xPosition = xPosition;
            si.yPosition = yPosition;
            symbols[numSymbols++] = si;
        }
    };

    std::map<int, LegerLineInfo> _info;

private:
    void _divideClefs(ScorePitchUtils::SpellingResults& s);

    const std::string _wholeNote = u8"\ue1d2";
    const std::string _flat = u8"\ue260";
    const std::string _natural = u8"\ue261";
    const std::string _sharp = u8"\ue262";

};
