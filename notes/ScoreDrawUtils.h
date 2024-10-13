#pragma once

#include <algorithm>
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

class DrawPositionParams {
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
    float noteColumnWidth = 1;
    float accidentalColumnWidth = 3.5f;
    float accidentalHeight = 30;
};

class SymbolInfo {
public:
    SymbolInfo() {}
    SymbolInfo(const std::string gly) : glyph(gly) {}
    std::string glyph;
    float xPosition = 0;
    float yPosition = 0;
    std::string toString() const;
};

class LegerLineInfo {
public:
    ShiftingArray<SymbolInfo> notes;
    ShiftingArray<SymbolInfo> accidentals;
    LegerLinesLocInfo legerLinesLocInfo;

    void addNote(const std::string& glyph, float xPosition, float yPosition) {
        SymbolInfo si;
        si.glyph = glyph;
        si.xPosition = xPosition;
        si.yPosition = yPosition;
        notes.push_back(si);
    }
    void addAccidental(const std::string& glyph, float xPosition, float yPosition) {
        SymbolInfo si;
        si.glyph = glyph;
        si.xPosition = xPosition;
        si.yPosition = yPosition;
        accidentals.push_back(si);
    }

    std::string toString() const {
        std::stringstream s;
        s << "there are " << notes.size() << " notes" << std::endl;
        for (unsigned i = 0; i < notes.size(); ++i) {
            s << notes[i].toString() << std::endl;
        }
        s << "there are " << accidentals.size() << " accidentals" << std::endl;
        for (unsigned i = 0; i < accidentals.size(); ++i) {
            s << accidentals[i].toString() << std::endl;
        }
        return s.str();
    }
};

}  // namespace sdu

using namespace sdu;


class ScoreDrawUtils {
public:
    static ScoreDrawUtilsPtr make();

    // The first thing is the line number, the second is bass clef
   using DrawInfo = std::vector< std::tuple<int, bool, LegerLineInfo>>;
    const DrawInfo getDrawInfo(
        const DrawPositionParams& pos,
        const Scale& scale,
        const SqArray<int, 16>& input,
        UIPrefSharpsFlats pref);

    inline static const std::string _wholeNote = u8"\ue1d2";
    inline static const std::string _flat = u8"\ue260";
    inline static const std::string _natural = u8"\ue261";
    inline static const std::string _sharp = u8"\ue262";
    void _divideClefs(ScorePitchUtils::SpellingResults& s);
    void _adjustNoteSpacing(const DrawPositionParams& pos);
    void _adjustAccidentalSpacing(const DrawPositionParams& pos);

    using infoMap = std::map<int, LegerLineInfo>;
    using iterator = infoMap::iterator;
    using const_iterator = infoMap::const_iterator;

    infoMap _infoTrebleClef;
    infoMap _infoBassClef;
   

private:
    void _adjustNoteSpacing(const DrawPositionParams& pos, infoMap&);
    void _adjustNoteSpacing(
        iterator nextLine,
        iterator line,
        const DrawPositionParams& pos);    
    
    void _adjustAccidentalSpacing(const DrawPositionParams& pos, infoMap&);
    static void _adjustAccidentalSpacing(
        const DrawPositionParams& pos,
        iterator currentLine,
        const std::vector<const LegerLineInfo*>& refLines);

    void _adjustAccidentalSpacing(
        const DrawPositionParams& pos,
        const infoMap& info,
        iterator  currentLine);

    static bool _linesCouldOverlap(const DrawPositionParams& pos, const LegerLineInfo& a, const LegerLineInfo& b);

    static float _getYPosition(const LegerLineInfo&);
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
    } else if (glyph.empty()) {
        g = "-";
    } else {
        g = "???";
    }

    std::stringstream s;
    s << "glyph=" << g;
    s << " x=" << xPosition << " y=" << yPosition;
    return s.str();
}
