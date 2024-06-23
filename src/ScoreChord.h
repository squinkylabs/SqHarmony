#pragma once

#include "ScorePitchUtils.h"
#include "VisualizerModule.h"

/*
 * TODO:
 *  rename the old SRN::Accidental to something else (Adjustment?) (done)
 *  make new class ScorePitchUtils (done)
 *      (SRN, Accidental) getAccidental(scale, MidiNote)
 *      (SRN, Accidental ) or none getOtherSpelling(SRN, Accidental)
 *
 * get single pitch notation working with accidental drawing (done).
 * add keysig select to ui. (done)
 * implement keysig select in composite. (done)
 *
 * implement zoom so we can see stuff (done)
 * why is keysig UI broken (done)
 * make changing key sig re-draw (done)
 *
 * notate all notes. (done)
 * sort pitches (done)
 * make note not overlap with last in stacked close chords. (done)
 * draw double notes, when required (done)
 * find and fix 0,0,7 bug. (done)
 *
 * make it look decent.
 *      make outer frame correct size (done)
 *      make staff draw correct (done)
 *      make bar lines draw correctly (done)
 *      fix key sig spacing (done)
 *      put notes in correct x pos. (done, but could be better?)
 *      make ledger lines look right (better)
 *
 * put in the natural and accidental if overlap.
 * assign notes to correct staff (done)
 * put in 8va 8vb markings
 * don't draw notes off page
 *
 * enharmonic spelling:
 *      use flats in flat keys.
 *      try to space identifiable chords.
 *      try to space all chords.
 *
 * Bugs:
 *      cmajor chord in C# Major - accidentals get on key sig.
 *      C and E in C major - doesn't draw the C
 */

class ScoreChord : public app::LightWidget, public Dirty {
public:
    ScoreChord(VisualizerModule *m) : _module(m) {
        ++_refCount;
    }
    ~ScoreChord() {
        --_refCount;
    }
    void draw(const DrawArgs &args) override;
    bool isDirty() const override {
        // SQINFO("Score::isDirty will ret %d", scoreIsDirty);
        return _scoreIsDirty;
    }
    void step() override;

private:
    bool _scoreIsDirty = false;
    unsigned _changeParam = 100000;
    static int _refCount;
    bool _whiteOnBlack = false;

    VisualizerModule *const _module = nullptr;
    MidiNote _lastScaleBase;
    Scale::Scales _lastScaleMode;

    const std::string _wholeNote = u8"\ue1d2";
    const std::string _staffFiveLines = u8"\ue014";
    const std::string _gClef = u8"\ue050";
    const std::string _fClef = u8"\ue062";
    const std::string _ledgerLine = u8"\ue022";
    const std::string _ledgerLineWide = u8"\ue023";
    const std::string _flat = u8"\ue260";
    const std::string _natural = u8"\ue261";
    const std::string _sharp = u8"\ue262";

    const float _zoom = 1.6;

    const float _topMargin = 27.5f * _zoom;
    const float _yTrebleStaff = _topMargin + 0;
    const float _yBassStaff = _yTrebleStaff + (25 * _zoom);    // 21.5 looks ok - a little wide
    const float _yTrebleClef = _yTrebleStaff - (3.3 * _zoom);  // 3 a little low, 4 way high
    const float _yBassClef = _yBassStaff - (10 * _zoom);       // 11 too much
    const float _yNoteInfo = _yBassStaff + (17 * _zoom);       // 0  high 12 for a long time...

    // X axis pos
    const float _leftMargin = 4.5f;
    const float _xStaff = _leftMargin;
    const float _xClef = _xStaff + 2;
    const float _xBarlineEnd = 83.5;
    const float _xBarlineFirst = _leftMargin;
    const float _xKeysig = 16 * _zoom;           // x position of the first accidental in the key signature.
    const float _deltaXAccidental = -8 * _zoom;  // accidental drawn this far from note, in x di
    const float _noteXIndent = 6;                // Distance from the keysig to the first note, horizontally.

    const float _ySpaceBetweenLines = 1.67f * _zoom;           // 1.7 slightly high
                                                               // 1.65 low
    const float _barlineHeight = _yBassStaff + (-14 * _zoom);  // 13.5 sticks up  15 gap

    float _drawMusicNonNotes(const DrawArgs &args) const;  // returns x pos at end of ksig
    void _drawNotes(const DrawArgs &args, float xPosition) const;
    void _drawNotesOnStaff(const DrawArgs &args, ConstScalePtr scale, float xPosition, bool bassStaff, const int *begin, const int *end) const;
    float _noteY(const MidiNote &note, bool bassStaff) const;

    class YInfo {
    public:
        float position = 200;
        float ledgerPos[3] = {};
    };
    YInfo noteYInfo(const MidiNote &note, bool bassStaff) const;
    void drawLedgerLinesForNotes(const DrawArgs &args, const YInfo &uInfo, float xPos) const;
    void drawOneNote(
        const DrawArgs &args,
        const MidiNote &note,
        const std::tuple<ScaleNote, ScorePitchUtils::Accidental> &notationNote,
        const YInfo &yInfo,
        float xPosition,
        bool offsetNote) const;
    void drawTwoNotes(
        const DrawArgs &args,
        const MidiNote &note,
        const std::tuple<ScaleNote, ScorePitchUtils::Accidental> &notationNote,
        const std::tuple<ScaleNote, ScorePitchUtils::Accidental> &notationNote2,
        const YInfo &yInfo,
        float xPosition) const;

    void _drawStaff(const DrawArgs &args, float y) const;
    void _drawBarLine(const DrawArgs &args, float x, float y) const;
    void _drawAccidental(const DrawArgs &args, float x, float y, ScorePitchUtils::Accidental accidental) const;
    ;
    /**
     * @return float width of key signature
     */
    std::pair<float, float> drawKeysig(const DrawArgs &args, ConstScalePtr scale, bool trebleClef, float y) const;

    float noteXPos(int noteNumber, std::pair<float, float> _keysigLayout) const;

    NVGcolor getForegroundColor() const;
    NVGcolor getBackgroundColor() const;

    void filledRect(NVGcontext *vg, NVGcolor color, float x, float y, float w, float h, float rounding) const;
    void drawHLine(NVGcontext *vg, NVGcolor color, float x, float y, float length, float width) const;
    void drawVLine(NVGcontext *vg, NVGcolor color, float x, float y, float length, float width) const;

    void prepareFontMusic(const DrawArgs &args) const;
    void prepareFontText1(const DrawArgs &args) const;
    void prepareFontText2(const DrawArgs &args) const;
};

int ScoreChord::_refCount = 0;

NVGcolor ScoreChord::getForegroundColor() const {
    return _whiteOnBlack ? nvgRGB(0xff, 0xff, 0xff) : nvgRGB(0, 0, 0);
}

NVGcolor ScoreChord::getBackgroundColor() const {
    return _whiteOnBlack ? nvgRGB(0, 0, 0) : nvgRGB(0xff, 0xff, 0xff);
}

void ScoreChord::drawVLine(NVGcontext *vg, NVGcolor color, float x, float y, float length, float width) const {
    nvgBeginPath(vg);
    nvgMoveTo(vg, x, y);
    nvgLineTo(vg, x, y - length);
    nvgStrokeColor(vg, color);
    nvgStrokeWidth(vg, width);
    nvgStroke(vg);
    nvgClosePath(vg);
}

void ScoreChord::drawHLine(NVGcontext *vg, NVGcolor color, float x, float y, float length, float width) const {
    nvgBeginPath(vg);
    nvgMoveTo(vg, x, y);
    nvgLineTo(vg, x + length, y);
    nvgStrokeColor(vg, color);
    nvgStrokeWidth(vg, width);
    nvgStroke(vg);
    nvgClosePath(vg);
}

void ScoreChord::filledRect(NVGcontext *vg, NVGcolor color, float x, float y, float w, float h, float rounding) const {
    w = std::min(w, 88.f);  //  clip with to 80, temp (TODO: do we need this?)
                            //  SQINFO("rect width=%f h=%f", w, h);
    nvgFillColor(vg, color);
    nvgBeginPath(vg);
    nvgRoundedRect(vg, x, y, w, h, rounding);
    nvgFill(vg);
}

void ScoreChord::prepareFontMusic(const DrawArgs &args) const {
    std::string fontPath("res/");
    fontPath += "Bravura.otf";
    // Get font
    std::shared_ptr<Font> font = APP->window->loadFont(asset::plugin(pluginInstance, fontPath.c_str()));
    if (!font) {
        WARN("Score font didn't load\n");
        return;
    }
    nvgFontFaceId(args.vg, font->handle);
    const float fontSize = 54 * _zoom;
    nvgFontSize(args.vg, fontSize);
}

inline void ScoreChord::draw(const DrawArgs &args) {
    nvgScissor(args.vg, RECT_ARGS(args.clipBox));
    const float left = _drawMusicNonNotes(args);
    const float right = _xBarlineEnd;
    const float xPosition = ((left + right) / 2.f) - _zoom * 5;

    _drawNotes(args, xPosition);
    _scoreIsDirty = false;

    Widget::draw(args);
}

float ScoreChord::_drawMusicNonNotes(const DrawArgs &args) const {
    NVGcolor color = getBackgroundColor();
    filledRect(args.vg, color, 0, 0, box.size.x, box.size.y, 5);
    prepareFontMusic(args);
    color = getForegroundColor();

    nvgFillColor(args.vg, color);

    _drawStaff(args, _yTrebleStaff);
    nvgText(args.vg, _xClef, _yTrebleClef, _gClef.c_str(), NULL);

    _drawStaff(args, _yBassStaff);
    nvgText(args.vg, _xClef, _yBassClef, _fClef.c_str(), NULL);

    float keysigWidth = 0;
    float keysigEnd = 0;

    if (_module) {
        const auto scale = _module->getScale();
        const auto a = drawKeysig(args, scale, true, _yTrebleStaff);
        const auto b = drawKeysig(args, scale, false, _yBassStaff);
        //    SQINFO("!! in non music, a.first = %f, second=%f", a.first, a.second);
        keysigWidth = std::max(keysigWidth, a.first);
        keysigWidth = std::max(keysigWidth, b.first);

        keysigEnd = std::max(keysigWidth, a.second);
        keysigEnd = std::max(keysigWidth, b.second);
    }

    _drawBarLine(args, _xBarlineEnd, _yBassStaff);
    _drawBarLine(args, _xBarlineFirst, _yBassStaff);
    // SQINFO("draw non notes returning ksig end %f", keysigEnd);
    return keysigEnd;
}

inline ScoreChord::YInfo ScoreChord::noteYInfo(const MidiNote &note, bool bassStaff) const {
    YInfo ret;
    if (note.get() < 10) {
        return ret;
    }

    float y = 0;

    const int ledgerLine = note.getLedgerLine(bassStaff);
    const float staffBasePos = bassStaff ? _yBassStaff : _yTrebleStaff;

    if (ledgerLine < -1) {
        ret.ledgerPos[0] = staffBasePos + (2.f * _ySpaceBetweenLines);
    }
    if (ledgerLine < -3) {
        ret.ledgerPos[1] = staffBasePos + (4.f * _ySpaceBetweenLines);
    }
    if (ledgerLine < -5) {
        ret.ledgerPos[2] = staffBasePos + (6.f * _ySpaceBetweenLines);
    }
    if (ledgerLine > 9) {
        ret.ledgerPos[0] = staffBasePos + (-10.f * _ySpaceBetweenLines);
    }
    if (ledgerLine > 11) {
        ret.ledgerPos[1] = staffBasePos + (-12.f * _ySpaceBetweenLines);
    }
    if (ledgerLine > 13) {
        ret.ledgerPos[2] = staffBasePos + (-14.f * _ySpaceBetweenLines);
    }

    y -= ledgerLine * _ySpaceBetweenLines;
    y += staffBasePos;

    ret.position = y;
    return ret;
}

inline void ScoreChord::_drawBarLine(const DrawArgs &args, float x, float y) const {
    auto color = getForegroundColor();
    drawVLine(args.vg, color, x, y, _barlineHeight, .75f);
}

inline float ScoreChord::noteXPos(int noteNumber, std::pair<float, float> _keysigLayout) const {
    const float keysigXEnds = _keysigLayout.second;
    const float totalWidth = box.size.x - 2 * _leftMargin;
    const float totalWidthForNotes = totalWidth - keysigXEnds;
    const float delta = totalWidthForNotes / 8.5;
    const float firstNotePosition = _leftMargin + keysigXEnds + _noteXIndent;

    float x = firstNotePosition + noteNumber * delta;
    if (noteNumber > 3) {
        // little bump into the next bar. Used to be a while delta, the /2 is new.
        x += (delta / 2.f);
    }
    return x;
}

inline std::pair<float, float> ScoreChord::drawKeysig(const DrawArgs &args, ConstScalePtr scale, bool treble, float y) const {
    const auto info = scale->getScoreInfo();
    float width = 0;
    float pos = 0;
    const MidiNote *accidentals = nullptr;
    bool areFlats = false;
    int num = 0;
    if (info.numFlats == 0 && info.numSharps == 0) {
        // cmaj, we are good.
    } else if (info.numFlats == 0) {
        areFlats = false;
    } else if (info.numSharps == 0) {
        areFlats = true;
    } else {
        areFlats = info.numFlats < info.numSharps;
    }

    if (areFlats) {
        accidentals = treble ? info.flatsInTrebleClef : info.flatsInBassClef;
        num = info.numFlats;
    } else {
        accidentals = treble ? info.sharpsInTrebleClef : info.sharpsInBassClef;
        num = info.numSharps;
    }

    //  SQINFO("in draw keysig, _xClef=%f, _xClefWidth=%f padding after = %f", _xClef, _xClefWidth, _paddingAfterKeysig);
    const float widthOfSharpFlat = (areFlats ? 3.0 : 3.5) * _zoom;
    const char *character = (areFlats) ? _flat.c_str() : _sharp.c_str();
    if (num) {
        float w = 0;
        float p = 0;
        for (int i = 0; i < num; ++i) {
            const float x = _xKeysig + w;
            const int note = accidentals[i].get();
            const float yf = _noteY(MidiNote(note), !treble);
            nvgText(args.vg, x, yf, character, NULL);
            w += widthOfSharpFlat;
            //    SQINFO("in loop, w=%f", w);
            p = std::max(p, x + widthOfSharpFlat);
        }
        width = std::max(width, w);
        pos = std::max(pos, p);
    } else {
        width = 0;
        pos = _xKeysig;
    }
    //  SQINFO("return width=%f pos=%f", width, pos);
    return std::make_pair(width, pos);
}

float ScoreChord::_noteY(const MidiNote &note, bool bassStaff) const {
    float y = 0;
    const float staffBasePos = bassStaff ? _yBassStaff : _yTrebleStaff;
    const int ledgerLine = note.getLedgerLine(bassStaff);
    y -= ledgerLine * _ySpaceBetweenLines;
    y += staffBasePos;
    return y;
}

inline void ScoreChord::_drawStaff(const DrawArgs &args, float yBase) const {
    const float x = _xStaff;
    const float length = _xBarlineEnd - _leftMargin;
    const auto color = getForegroundColor();
    for (int i = 0; i < 5; ++i) {
        float y = yBase - 2.f * float(i) * _ySpaceBetweenLines;
        drawHLine(args.vg, color, x, y, length, .5f);
    }
}

inline void ScoreChord::step() {
    if (_module) {
        unsigned changeParam = _module->getChangeParam();
        if (changeParam != _changeParam) {
            _changeParam = changeParam;
            SQINFO("ScoreChord::step sees change!");
            this->_scoreIsDirty = true;
        }

        const auto scale = _module->getScale();
        const auto scaleInfo = scale->get();
        if (scaleInfo.first.get() != _lastScaleBase.get() ||
            scaleInfo.second != _lastScaleMode) {
            SQINFO("see scale change!!");
            _lastScaleBase = scaleInfo.first;
            _lastScaleMode = scaleInfo.second;
            _scoreIsDirty = true;
        }
    }
    Widget::step();
}

inline void ScoreChord::drawLedgerLinesForNotes(const DrawArgs &args, const YInfo &yInfo, float xPosition) const {
    for (int i = 0; i < 3; ++i) {
        if (yInfo.ledgerPos[i] != 0) {
            nvgText(args.vg, xPosition, yInfo.ledgerPos[i], _ledgerLineWide.c_str(), NULL);
        }
    }
}

inline void ScoreChord::_drawAccidental(const DrawArgs &args, float xPosition, float yPosition, ScorePitchUtils::Accidental accidental) const {
    SQINFO("drawAccidental x=%f, acciental=%d", xPosition, int(accidental));
    if (accidental == ScorePitchUtils::Accidental::none) {
        SQINFO("ret early from draw accid");
        return;
    }
    std::string symbol = "";
    switch (accidental) {
        case ScorePitchUtils::Accidental::sharp:
            symbol = _sharp;
            break;
        case ScorePitchUtils::Accidental::flat:
            symbol = _flat;
            break;
        case ScorePitchUtils::Accidental::natural:
            symbol = _natural;
            break;
        default:
            SQFATAL("unknown accidental");
            break;
    }
    SQINFO("drawAccidental (%d) at %f,%f", int(accidental), xPosition, yPosition);
    nvgText(args.vg, xPosition + _deltaXAccidental, yPosition, symbol.c_str(), NULL);
}

// returns true if accidental1 <= accidental2
inline bool compareAccidentals(ScorePitchUtils::Accidental accidental1, ScorePitchUtils::Accidental accidental2) {
    SQINFO("compare %d vs %d. flat=%d, sharp=%d, natural=%d, none=%d",
        int(accidental1), int(accidental2),
    int(ScorePitchUtils::Accidental::flat), int(ScorePitchUtils::Accidental::sharp), int(ScorePitchUtils::Accidental::natural), int(ScorePitchUtils::Accidental::none)); 

    if (accidental1 == accidental2) {
        return true;
    }

    if (accidental2 == ScorePitchUtils::Accidental::sharp) {
        // if the second is a sharp, it must be GE the other one.
        return true;                    
    }
    if (accidental1 == ScorePitchUtils::Accidental::sharp) {
        return false;
    }
    // More cases to implement.
    assert(false);
    return false;
} 

inline void ScoreChord::drawTwoNotes(
    const DrawArgs &args,
    const MidiNote &note,
    const std::tuple<ScaleNote, ScorePitchUtils::Accidental> &notationNote,
    const std::tuple<ScaleNote, ScorePitchUtils::Accidental> &notationNote2,
    const YInfo &yInfo,
    float xPosition) const {
    drawLedgerLinesForNotes(args, yInfo, xPosition);
    const char *notePtr = _wholeNote.c_str();

    const float noteXOffset = 5;
    SQINFO("drwing two notes at %f and %f", xPosition + noteXOffset, xPosition - noteXOffset);
    nvgText(args.vg, xPosition + noteXOffset, yInfo.position, notePtr, NULL);
    nvgText(args.vg, xPosition - noteXOffset, yInfo.position, notePtr, NULL);
    const auto accidental1 = std::get<1>(notationNote);
    const auto accidental2 = std::get<1>(notationNote2);

    bool showAccidental1 = false;
    bool showAccidental2 = false;
    if ((accidental1 == ScorePitchUtils::Accidental::none) && (accidental2 == ScorePitchUtils::Accidental::none)) {
        showAccidental1 = showAccidental2 = false;
    } else if ((accidental1 != ScorePitchUtils::Accidental::none) && (accidental2 == ScorePitchUtils::Accidental::none)) {
        showAccidental1 = true;
        showAccidental2 = false;
    } else if ((accidental1 == ScorePitchUtils::Accidental::none) && (accidental2 != ScorePitchUtils::Accidental::none)) {
        showAccidental1 = false;
        showAccidental2 = true;
    } else if ((accidental1 != ScorePitchUtils::Accidental::none) && (accidental2 != ScorePitchUtils::Accidental::none)) {
        showAccidental1 = true;
        showAccidental2 = true;
    }

    const bool showTwoAccidentals = showAccidental1 && showAccidental2;

    ScorePitchUtils::Accidental inOrderAccidendals[2] = {ScorePitchUtils::Accidental::none, ScorePitchUtils::Accidental::none };
    int accidentalIndex = 0;
    if (showAccidental1) {
        inOrderAccidendals[accidentalIndex++] = accidental1;
    }

    if (showAccidental2) {
        inOrderAccidendals[accidentalIndex++] = accidental2;
    }

    if (compareAccidentals(inOrderAccidendals[0], inOrderAccidendals[1])) {
        std::swap(inOrderAccidendals[0], inOrderAccidendals[1]);
    }


    float xPositionAccidental = xPosition + _deltaXAccidental;
    if (showTwoAccidentals) {
        xPositionAccidental += _deltaXAccidental;
    }
    //   SQINFO("will draw accid, show1=%d show 2 = %d", showAccidental1, showAccidental2);
    if (showAccidental1) {
        SQINFO("in two notes, drawing acciental1");
        _drawAccidental(args, xPositionAccidental, yInfo.position, inOrderAccidendals[0]);
    }
    if (showAccidental2) {
         SQINFO("in two notes, drawing acciental2");
        _drawAccidental(args, xPositionAccidental - _deltaXAccidental, yInfo.position, inOrderAccidendals[1]);
    }
}

inline void ScoreChord::drawOneNote(
    const DrawArgs &args,
    const MidiNote &note,
    const std::tuple<ScaleNote, ScorePitchUtils::Accidental> &notationNote,
    const YInfo &yInfo,
    float xPosition,
    bool offsetNote) const {
    SQINFO("  drawOneNote offset=%d dxAccid=%f", offsetNote, _deltaXAccidental);
    drawLedgerLinesForNotes(args, yInfo, xPosition);
    const char *notePtr = _wholeNote.c_str();

    const float noteXOffset = offsetNote ? 9 : 0;
    nvgText(args.vg, xPosition + noteXOffset, yInfo.position, notePtr, NULL);
    _drawAccidental(args, xPosition + _deltaXAccidental, yInfo.position, std::get<1>(notationNote));
}

inline void ScoreChord::_drawNotes(const DrawArgs &args, float xPosition) const {
    if (!_module) {
        return;
    }

    const auto pitchesAndChannels = _module->getQuantizedPitchesAndChannels();
    const unsigned channels = std::get<1>(pitchesAndChannels);
    const int *originalPitches = std::get<0>(pitchesAndChannels);
    if (channels <= 0) {
        //     SQINFO("On change 274, nothing");
        return;
    }
    //   SQINFO("change, channels=%d", channels);
  //  SQINFO("_drawNOtes, pos=%f", xPosition);

    ConstScalePtr scale = _module->getScale();
    int copyOfPitches[16];
    for (unsigned i = 0; i < channels; ++i) {
        copyOfPitches[i] = originalPitches[i];
        SQINFO("copied %d at index %d", copyOfPitches[i], i);
    }
    std::sort(copyOfPitches, copyOfPitches + channels);

    const int *firstBassNote = nullptr;
    const int *lastBassNote = nullptr;

    const int *firstTrebleNote = nullptr;
    const int *lastTrebleNote = nullptr;

    const int *firstMiddleC = nullptr;
    const int *lastMiddleC = nullptr;

    for (const int *pitchIterator = copyOfPitches; pitchIterator < (copyOfPitches + channels); ++pitchIterator) {
        const int pitch = *pitchIterator;
        if (pitch < MidiNote::MiddleC) {
            if (firstBassNote == nullptr) {
                firstBassNote = pitchIterator;
            }
            lastBassNote = pitchIterator;
        }
        if (pitch == MidiNote::MiddleC) {
            if (firstMiddleC == nullptr) {
                firstMiddleC = pitchIterator;
            }
            lastMiddleC = pitchIterator;
        }
        if (pitch > MidiNote::MiddleC) {
            if (firstTrebleNote == nullptr) {
                firstTrebleNote = pitchIterator;
            }
            lastTrebleNote = pitchIterator;
        }
    }
 //   SQINFO("after pass 1 %p, %p, %p, %p, %p", firstBassNote, lastBassNote, firstMiddleC, firstTrebleNote, lastTrebleNote);

    // Decide where middle C belongs
    if (firstMiddleC) {
        // If all notes are in bass, then put middle C there, too
        if (firstBassNote && !firstTrebleNote) {
            lastBassNote = copyOfPitches + channels;
        } else if (firstTrebleNote && !firstBassNote) {
            lastTrebleNote =  copyOfPitches + channels;
        } else {
             // otherwise it's mixed, to put in treble
             firstTrebleNote = firstMiddleC;
             if (!lastTrebleNote) {
                lastTrebleNote = lastMiddleC;
             }
             SQINFO("mixed");
        }
    }
   //  SQINFO("after pass 2 %p, %p, %p, %p, %p", firstBassNote, lastBassNote, firstMiddleC, firstTrebleNote, lastTrebleNote);
    if (firstBassNote) {
        _drawNotesOnStaff(args, scale, xPosition, true, firstBassNote, lastBassNote + 1);
    }

    if (firstTrebleNote) {
        _drawNotesOnStaff(args, scale, xPosition, false, firstTrebleNote, lastTrebleNote + 1);
    }

    //    _drawNotesOnStaff(args, scale, xPosition, false,  copyOfPitches, copyOfPitches + channels);
}

inline void ScoreChord::_drawNotesOnStaff(const DrawArgs &args, ConstScalePtr scale, float xPosition, bool bassStaff, const int *begin, const int *end) const {
  //  SQINFO("_drawNotesOnStaff %p, %p bassStuff=%d", begin, end, bassStaff);
    int lastYPos = 1000;
    bool lastNoteOffset = false;

    for (const int *pitchIterator = begin; pitchIterator < end; ++pitchIterator) {
        MidiNote mn(*pitchIterator);
        const auto notationNote = ScorePitchUtils::getNotationNote(*scale, mn);
        const auto yInfo = noteYInfo(mn, bassStaff);
        //    SQINFO("in draw loop i=%d pitch=%d y=%f", i, mn.get(), yInfo.position);
        const float distance = lastYPos - yInfo.position;
        lastYPos = yInfo.position;
        const bool noteOffsetByTwoLines = (distance / _ySpaceBetweenLines) > 1.2;

        bool twoNotesOnSameLine = false;
        std::tuple<ScaleNote, ScorePitchUtils::Accidental> notationNoteNext;
        // if there is another note after this one
        if ((pitchIterator + 1) < end) {
            MidiNote mnNext(*(pitchIterator + 1));
            const auto yInfoNext = noteYInfo(mnNext, bassStaff);
            if (yInfo.position == yInfoNext.position) {
                SQWARN("two notes at same location pitch = %d, %d, y=%f, %f", mn.get(), mnNext.get(), yInfo.position, yInfoNext.position);

                twoNotesOnSameLine = true;
                notationNoteNext = ScorePitchUtils::getNotationNote(*scale, mnNext);
                ++pitchIterator;
            }
        }

        if (!twoNotesOnSameLine) {
            const bool offsetThisNote = !noteOffsetByTwoLines && !lastNoteOffset;
            SQINFO("offset flag will be %d", offsetThisNote);
            lastNoteOffset = offsetThisNote;
            drawOneNote(args, mn, notationNote, yInfo, xPosition, offsetThisNote);
        } else {
            drawTwoNotes(args, mn, notationNote, notationNoteNext, yInfo, xPosition);
        }
    }
}
