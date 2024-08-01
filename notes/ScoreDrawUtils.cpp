

#include "ScoreDrawUtils.h"

#include "ChordRecognizer.h"
#include "ScorePitchUtils.h"

ScoreDrawUtilsPtr ScoreDrawUtils::make() {
    return std::make_unique<ScoreDrawUtils>();
}

void ScoreDrawUtils::getDrawInfo(
    const Scale& scale,
    const SqArray<int, 16>& input,
    bool bassStaff,
    UIPrefSharpsFlats pref) {

    const auto info = ChordRecognizer::recognize(input);
    // oops - guess we need to split staves first???
    const auto spelling = ScorePitchUtils::findSpelling(scale, input, bassStaff, pref);
    for (unsigned pitchIterator = 0; pitchIterator < spelling.notes.numValid(); ++pitchIterator) {
        const auto notationNote = spelling.notes.getAt(pitchIterator);
      //  const MidiNote &mn = notationNote._midiNote;

        const int legerLine = notationNote._legerLine;
        LegerLineInfo llinfo;
        const auto iter = _info.find(legerLine);
        if (iter == _info.end()) {
            llinfo.numSymbols = 1;
            _info.insert( std::make_pair(legerLine,  llinfo));
        } else {
            iter->second.numSymbols++;
        }
    }
}