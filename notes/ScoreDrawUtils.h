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
            float xPosition = 0;
            float yPosition = 0;
            bool isAccidental = false;
        };

        SymbolInfo symbols[4];
        unsigned numSymbols = 0;
        void addOne(bool isAccidental, const std::string& glyph, float xPosition, float yPosition) {
            assert(numSymbols < 4);
            SymbolInfo si;
            si.glyph = glyph;
            si.xPosition = xPosition;
            si.yPosition = yPosition;
            si.isAccidental = isAccidental;
            symbols[numSymbols++] = si;
        }
        void sort() {
            std::sort(symbols, symbols + numSymbols, [](const SymbolInfo& first, const SymbolInfo& second) {
                return first.isAccidental && !second.isAccidental;

            });
        }
    };

    std::map<int, LegerLineInfo> _info;

    inline static const std::string _wholeNote = u8"\ue1d2";
    inline static const std::string _flat = u8"\ue260";
    inline static const std::string _natural = u8"\ue261";
    inline static const std::string _sharp = u8"\ue262";

private:
    void _divideClefs(ScorePitchUtils::SpellingResults& s);
};
