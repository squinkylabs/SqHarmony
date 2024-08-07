#pragma once

#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <vector>

#include "ScorePitchUtils.h"
#include "SharpsFlatsPref.h"
#include "ShiftingArray.h"
#include "SqArray.h"

class Scale;
class ScoreDrawUtils;
using ScoreDrawUtilsPtr = std::unique_ptr<ScoreDrawUtils>;

/***************** a bunch of utils used only internally **************************/
namespace sdu {

class LegerLinesLocInfo {
public:
    float legerPos[3] = {0};
};

class DrawPosition {
public:
    std::function<float(const MidiNote& note, int legerLine, bool bassStaff)> noteYPosition =
        [](const MidiNote& note, int legerLine, bool bassStaff) {
            return 0.f;
        };
    std::function<LegerLinesLocInfo(const MidiNote& note, int legerLine, bool bassStaff)> llDrawInfo =
        [](const MidiNote& note, int legerLine, bool bassStaff) {
            return LegerLinesLocInfo();
        };

    float noteXPosition = 0;
    float columnWidth = 1;
};

class SymbolInfo {
public:
    std::string glyph;
    float xPosition = 0;
    float yPosition = 0;
    bool isAccidental = false;
   
    std::string toString() const;
};

class LegerLineInfo {
public:
   // ShiftingArray<SymbolInfo> symbols;
    std::vector<SymbolInfo> symbols;
    LegerLinesLocInfo legerLinesLocInfo;

    void addOne(bool isAccidental, const std::string& glyph, float xPosition, float yPosition) {
        assert(symbols.size() < 4);
        SymbolInfo si;
        si.glyph = glyph;
        si.xPosition = xPosition;
        si.yPosition = yPosition;
        si.isAccidental = isAccidental;
        symbols.push_back(si);
    }
    void sort() {
        std::sort(symbols.begin(), symbols.end(), [](const SymbolInfo& first, const SymbolInfo& second) {
            return first.isAccidental && !second.isAccidental;
        });
    }
    std::string toString() const {
        std::stringstream s;
        s << "there are " << symbols.size() << " symbols" << std::endl;
        for (unsigned i = 0; i < symbols.size(); ++i) {
            s << symbols[i].toString() << std::endl;
        }
        return s.str();
    }
};

}  // namespace sdu

using namespace sdu;

class ScoreDrawUtils {
public:
    static ScoreDrawUtilsPtr make();

    const std::map<int, LegerLineInfo> getDrawInfo(
        const DrawPosition& pos,
        const Scale& scale,
        const SqArray<int, 16>& input,
        UIPrefSharpsFlats pref);

    inline static const std::string _wholeNote = u8"\ue1d2";
    inline static const std::string _flat = u8"\ue260";
    inline static const std::string _natural = u8"\ue261";
    inline static const std::string _sharp = u8"\ue262";
    void _divideClefs(ScorePitchUtils::SpellingResults& s);
    void _adjustNoteSpacing(const DrawPosition& pos);
private:
    std::map<int, LegerLineInfo> _info;
    
};

inline std::string SymbolInfo::toString() const {
    std::string g;

    if (glyph == ScoreDrawUtils::_wholeNote) {
        g = "whole note";
    } else if (glyph == ScoreDrawUtils::_sharp) {
        g = "sharp";
    } else if (glyph == ScoreDrawUtils::_flat) {
        g = "flat";
    } else if (glyph == ScoreDrawUtils::_natural) {
        g = "natural";
    } else {
        g = "???";
    }


    std::stringstream s;
    s << "isAcc=" << isAccidental;
    s << " glyph=" << g;
    s << " x=" << xPosition << " y=" << yPosition << std::endl;
    return s.str();
}
