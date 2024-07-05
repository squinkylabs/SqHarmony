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
 *      make leger lines look right (better)
 *
 * put in the natural and accidental if overlap. (done)
 * assign notes to correct staff (done)
 * put in 8va 8vb markings
 * don't draw notes off page
 * two accidentals on one line - spacing not right
 * get rid of debug stuff:     ScaleNote m2s(const MidiNote&, bool printDebug = false) const;
 *
 * enharmonic spelling:
 *      use flats in flat keys. (done)
 *      try to space identifiable chords.
 *      try to space all chords.
 *
 * Bugs:
 *      C and A below middle C - draws crazy leger lines(done)
 *      C and D flat in cmajor, draws two C (done)
 *      cmajor chord in C# Major - accidentals get on key sig. (done)
 *      C and E in C major - doesn't draw the C (fixed)
 *      In C Major one note, I don't see A natural, only sharp?? (user error)
 * 
 * How are sharps/flats handled now?
 * _drawNotes() doesn't care. it divides midi pitches between staves and calls
 * _drawNotesOnStaff, passing midi pitch and a Scale.
 * _drawNotesOnStaff makes NotationNote
 * NotationNote has ScorePitchUtils::Accidental, and int _legerLine;
 * ScorePitchUtils::getNotationNote uses the pref from the scale. Also it calls Scale::m2s,
 * and the resulting ScaleNote does have sharp/flat built in.
 * 
 * step 1: remove ScaleNote from NotationNote (done).
 * step 2: break out NotationNote into own class. (done)
 * step 3: make "getAlternateSpelling" method, with unit tests. (move the spelling stuff back into utils?)
 * step 4: add util to get vector of spellings. 
 * step 5: use it, re-do scoring (?), make new spelling stuff unit testable.
 */

// #define _LOG

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
    // const std::string _staffFiveLines = u8"\ue014";
    const std::string _gClef = u8"\ue050";
    const std::string _fClef = u8"\ue062";
    //  const std::string _legerLine = u8"\ue022";
    const std::string _legerLineWide = u8"\ue023";
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
    const float _xKeysig = 16 * _zoom;             // x position of the first accidental in the key signature.
    const float _deltaXAccidental = -2.2 * _zoom;  // accidental drawn this far from note, in x di
                                                   // for single note, -8 is way too much, -4 tolerable, -2 just about touching.
                                                   // 0 is on top, as it should be. -2.2 is pretty good for single notes.

    const float _noteXIndent = 6;  // Distance from the keysig to the first note, horizontally.

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
        float legerPos[3] = {};
    };
    YInfo _noteYInfo(const MidiNote &note, int legerLine, bool bassStaff) const;
    void drawLegerLinesForNotes(const DrawArgs &args, const YInfo &uInfo, float xPos) const;
    void drawOneNote(
        const DrawArgs &args,
        const MidiNote &note,
        const NotationNote &notationNote,
        const YInfo &yInfo,
        float xPosition,
        bool offsetNote) const;
    void drawTwoNotes(
        const DrawArgs &args,
        const MidiNote &note,
        const NotationNote &notationNote,
        const NotationNote &notationNote2,
        const YInfo &yInfo,
        float xPosition) const;

    void _drawStaff(const DrawArgs &args, float y) const;
    void _drawBarLine(const DrawArgs &args, float x, float y) const;
    void _drawAccidental(const DrawArgs &args, float x, float y, NotationNote::Accidental accidental) const;
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
        keysigWidth = std::max(keysigWidth, a.first);
        keysigWidth = std::max(keysigWidth, b.first);

        keysigEnd = std::max(keysigWidth, a.second);
        keysigEnd = std::max(keysigWidth, b.second);
    }

    _drawBarLine(args, _xBarlineEnd, _yBassStaff);
    _drawBarLine(args, _xBarlineFirst, _yBassStaff);
    return keysigEnd;
}

inline ScoreChord::YInfo ScoreChord::_noteYInfo(const MidiNote &note, int legerLine, bool bassStaff) const {
    YInfo ret;
    if (note.get() < 10) {
        return ret;
    }

    float yPosition = 0;

    //  const int legerLine = note.getLegerLine(prefs, bassStaff);
    const float staffBasePos = bassStaff ? _yBassStaff : _yTrebleStaff;

    if (legerLine < -1) {
        ret.legerPos[0] = staffBasePos + (2.f * _ySpaceBetweenLines);
    }
    if (legerLine < -3) {
        ret.legerPos[1] = staffBasePos + (4.f * _ySpaceBetweenLines);
    }
    if (legerLine < -5) {
        ret.legerPos[2] = staffBasePos + (6.f * _ySpaceBetweenLines);
    }
    if (legerLine > 9) {
        ret.legerPos[0] = staffBasePos + (-10.f * _ySpaceBetweenLines);
    }
    if (legerLine > 11) {
        ret.legerPos[1] = staffBasePos + (-12.f * _ySpaceBetweenLines);
    }
    if (legerLine > 13) {
        ret.legerPos[2] = staffBasePos + (-14.f * _ySpaceBetweenLines);
    }

    yPosition -= legerLine * _ySpaceBetweenLines;
    yPosition += staffBasePos;

    ret.position = yPosition;

#ifdef _LOG
    SQINFO("YY noteYInfo(note %d bassStaff=%d, leger line = %d) pos=%f", note.get(), bassStaff, legerLine, ret.position);
#endif
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
            p = std::max(p, x + widthOfSharpFlat);
        }
        width = std::max(width, w);
        pos = std::max(pos, p);
    } else {
        width = 0;
        pos = _xKeysig;
    }
    return std::make_pair(width, pos);
}

float ScoreChord::_noteY(const MidiNote &note, bool bassStaff) const {
    assert(!note.isBlackKey());
    float y = 0;
    const float staffBasePos = bassStaff ? _yBassStaff : _yTrebleStaff;
    const int legerLine = note.getLegerLine(bassStaff);
    y -= legerLine * _ySpaceBetweenLines;
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
            this->_scoreIsDirty = true;
        }

        const auto scale = _module->getScale();
        const auto scaleInfo = scale->get();
        if (scaleInfo.first.get() != _lastScaleBase.get() ||
            scaleInfo.second != _lastScaleMode) {
            _lastScaleBase = scaleInfo.first;
            _lastScaleMode = scaleInfo.second;
            _scoreIsDirty = true;
        }
    }
    Widget::step();
}

inline void ScoreChord::drawLegerLinesForNotes(const DrawArgs &args, const YInfo &yInfo, float xPosition) const {
    for (int i = 0; i < 3; ++i) {
        if (yInfo.legerPos[i] != 0) {
            nvgText(args.vg, xPosition, yInfo.legerPos[i], _legerLineWide.c_str(), NULL);
        }
    }
}

inline void ScoreChord::_drawAccidental(const DrawArgs &args, float xPosition, float yPosition, NotationNote::Accidental accidental) const {
#ifdef _LOG
    SQINFO("drawAccidental 424  x=%f, acciental=%d (for ref flat=%d )", xPosition, int(accidental), int(ScorePitchUtils::Accidental::flat));
#endif
    if (accidental == NotationNote::Accidental::none) {
#ifdef _LOG
        SQINFO("ret early from draw accidental = none");
#endif
        return;
    }
    std::string symbol = "";
    switch (accidental) {
        case NotationNote::Accidental::sharp:
            symbol = _sharp;
            break;
        case NotationNote::Accidental::flat:
            symbol = _flat;
            break;
        case NotationNote::Accidental::natural:
            symbol = _natural;
            break;
        default:
            SQFATAL("unknown accidental");
            break;
    }
#ifdef _LOG
    SQINFO("drawAccidental (%d) at %f,%f", int(accidental), xPosition, yPosition);
#endif
    nvgText(args.vg, xPosition + _deltaXAccidental, yPosition, symbol.c_str(), NULL);
}

inline void ScoreChord::drawTwoNotes(
    const DrawArgs &args,
    const MidiNote &note,
    const NotationNote &notationNote,
    const NotationNote &notationNote2,
    const YInfo &yInfo,
    float xPosition) const {
    drawLegerLinesForNotes(args, yInfo, xPosition);
    const char *notePtr = _wholeNote.c_str();

    const float noteXOffset = 5;
#ifdef _LOG
    SQINFO("-- drawing two notes at xpos=%f and xpos=%f", xPosition + noteXOffset, xPosition - noteXOffset);
    SQINFO(" nn ll=%d accid=%d nn2 ll=%d accid=%d", notationNote._legerLine, int(notationNote._accidental), notationNote2._legerLine, int(notationNote2._accidental));
#endif
    nvgText(args.vg, xPosition + noteXOffset, yInfo.position, notePtr, NULL);
    nvgText(args.vg, xPosition - noteXOffset, yInfo.position, notePtr, NULL);
    const auto accidental1 = notationNote._accidental;
    const auto accidental2 = notationNote2._accidental;

    bool showAccidental1 = false;
    bool showAccidental2 = false;
    if ((accidental1 == NotationNote::Accidental::none) && (accidental2 == NotationNote::Accidental::none)) {
        showAccidental1 = showAccidental2 = false;
    } else if ((accidental1 != NotationNote::Accidental::none) && (accidental2 == NotationNote::Accidental::none)) {
        showAccidental1 = true;
        showAccidental2 = false;
    } else if ((accidental1 == NotationNote::Accidental::none) && (accidental2 != NotationNote::Accidental::none)) {
        showAccidental1 = false;
        showAccidental2 = true;
    } else if ((accidental1 != NotationNote::Accidental::none) && (accidental2 != NotationNote::Accidental::none)) {
        showAccidental1 = true;
        showAccidental2 = true;
    }

    const bool showTwoAccidentals = showAccidental1 && showAccidental2;
    float xAccidentalPostion;
    if (showTwoAccidentals) {
        xAccidentalPostion = xPosition + 2 * _deltaXAccidental - noteXOffset;
    } else {
        xAccidentalPostion = xPosition + _deltaXAccidental - noteXOffset;
    }

    if (showTwoAccidentals) {
        // SQWARN("can't show two atm");
        _drawAccidental(args, xAccidentalPostion, yInfo.position, accidental1);
        xAccidentalPostion -= _deltaXAccidental;
        _drawAccidental(args, xAccidentalPostion, yInfo.position, accidental2);

    } else if (showAccidental1) {
        _drawAccidental(args, xAccidentalPostion, yInfo.position, accidental1);
    } else if (showAccidental2) {
        _drawAccidental(args, xAccidentalPostion, yInfo.position, accidental2);
    }

#if 0

    ScorePitchUtils::Accidental inOrderAccidendals[2] = {ScorePitchUtils::Accidental::none, ScorePitchUtils::Accidental::none};
    int accidentalIndex = 0;
    if (showAccidental1) {
        inOrderAccidendals[accidentalIndex++] = accidental1;
    }

    if (showAccidental2) {
        inOrderAccidendals[accidentalIndex++] = accidental2;
    }

    // if (compareAccidentals(inOrderAccidendals[0], inOrderAccidendals[1])) {
    //     std::swap(inOrderAccidendals[0], inOrderAccidendals[1]);
    // }

    float xPositionAccidental = xPosition + _deltaXAccidental;
    if (showTwoAccidentals) {
        xPositionAccidental += _deltaXAccidental;
    }
    SQINFO("in drawTwoNotes will draw accid, show1=%d show 2 = %d", showAccidental1, showAccidental2);
    if (showAccidental1) {
        SQINFO("in drawTwoNotes, drawing acciental1");
        _drawAccidental(args, xPositionAccidental, yInfo.position, inOrderAccidendals[0]);
    }
    if (showAccidental2) {
        SQINFO("in drawTwoNotes, drawing acciental2");
        _drawAccidental(args, xPositionAccidental - _deltaXAccidental, yInfo.position, inOrderAccidendals[1]);
    }
#endif
}

inline void ScoreChord::drawOneNote(
    const DrawArgs &args,
    const MidiNote &note,
    const NotationNote &notationNote,
    const YInfo &yInfo,
    float xPosition,
    bool offsetNote) const {
#ifdef _LOG
    SQINFO("--- drawOneNote offset=%d dxAccid=%f Pos=%f,%f notationNoteaccid=%d",
           offsetNote, _deltaXAccidental, xPosition, yInfo.position, int(notationNote._accidental));
#endif
    drawLegerLinesForNotes(args, yInfo, xPosition);
    const char *notePtr = _wholeNote.c_str();

    const float noteXOffset = offsetNote ? 9 : 0;
    nvgText(args.vg, xPosition + noteXOffset, yInfo.position, notePtr, NULL);
    _drawAccidental(args, xPosition + _deltaXAccidental, yInfo.position, notationNote._accidental);
}

inline void ScoreChord::_drawNotes(const DrawArgs &args, float xPosition) const {
    if (!_module) {
        return;
    }

    const auto pitchesAndChannels = _module->getQuantizedPitchesAndChannels();
    const unsigned channels = std::get<1>(pitchesAndChannels);
    const int *originalPitches = std::get<0>(pitchesAndChannels);
    if (channels <= 0) {
        return;
    }

#ifdef _LOG
    SQINFO("++++ _drawNOtes, pos=%f channels=%d", xPosition, channels);
#endif
    ConstScalePtr scale = _module->getScale();
    int copyOfPitches[16];
    for (unsigned i = 0; i < channels; ++i) {
        copyOfPitches[i] = originalPitches[i];
    #ifdef _LOG
        SQINFO("copied %d at index %d", copyOfPitches[i], i);
        #endif
    }
    std::sort(copyOfPitches, copyOfPitches + channels);

#ifdef _LOG
    SQINFO("after sort %d %d  copyof=%p", copyOfPitches[0], copyOfPitches[1], copyOfPitches);
#endif

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
#ifdef _LOG
    SQINFO("after pass 1 %p, %p, %p, %p, %p", firstBassNote, lastBassNote, firstMiddleC, firstTrebleNote, lastTrebleNote);
    SQINFO("base #=%d", unsigned(lastBassNote - firstBassNote));
#endif
    // Decide where middle C belongs
    if (firstMiddleC) {
        // If all notes are in bass, then put middle C there, too
        if (firstBassNote && !firstTrebleNote) {
            lastBassNote = firstMiddleC;
        } else if (firstTrebleNote && !firstBassNote) {
            firstTrebleNote = firstMiddleC;
        } else {
            // otherwise it's mixed, to put in treble
            firstTrebleNote = firstMiddleC;
            if (!lastTrebleNote) {
                lastTrebleNote = lastMiddleC;
            }
        }
    }
#ifdef _LOG
    SQINFO("after pass 2 %p, %p, %p, %p, %p", firstBassNote, lastBassNote, firstMiddleC, firstTrebleNote, lastTrebleNote);
#endif
    if (firstBassNote) {
        //SQINFO("drawing bass, there are %d", unsigned((lastBassNote + 1) - firstBassNote));
        const unsigned num = unsigned((lastBassNote + 1) - firstBassNote);
        assert(num <= channels);
        _drawNotesOnStaff(args, scale, xPosition, true, firstBassNote, lastBassNote + 1);
    }

    if (firstTrebleNote) {
        //SQINFO("about to call draw notes on staff tn0=%d, tn1=%d", firstTrebleNote[0], firstTrebleNote[1]);

        const unsigned num = unsigned((lastTrebleNote + 1) - firstTrebleNote);
        assert(num <= channels);
        _drawNotesOnStaff(args, scale, xPosition, false, firstTrebleNote, lastTrebleNote + 1);
    }
}

inline void ScoreChord::_drawNotesOnStaff(const DrawArgs &args, ConstScalePtr scale, float xPosition, bool bassStaff, const int *begin, const int *end) const {
    #ifdef _LOG
    SQINFO("\n!!!! _drawNotesOnStaff %p, %p bassStuff=%d", begin, end, bassStaff);
    SQINFO("scale=%p", scale.get());
    SQINFO("scale base = %d", scale->base().get());
    #endif

    assert(scale.get() != nullptr);
    int lastYPos = 1000;
    bool lastNoteOffset = false;

    for (const int *pitchIterator = begin; pitchIterator < end; ++pitchIterator) {
        MidiNote mn(*pitchIterator);
    #ifdef _LOG
        {
            const auto x = *pitchIterator;
            assert(x < 1000);
            assert(x > -1000);
            SQINFO("in loop, x=%d mn.get() = %d", x, mn.get());
        }
        #endif
        const auto notationNote = ScorePitchUtils::getNotationNote(*scale, mn, bassStaff);
        //SQINFO("drawNotesOnStaff just got notation note with accid=%d", int(notationNote._accidental));
        const auto yInfo = _noteYInfo(mn, notationNote._legerLine, bassStaff);
        //SQINFO("in draw loop iter=%p pitch=%d y=%f", pitchIterator, mn.get(), yInfo.position);
        const float distance = lastYPos - yInfo.position;
        lastYPos = yInfo.position;
        const bool noteOffsetByTwoLines = (distance / _ySpaceBetweenLines) > 1.2;

        bool twoNotesOnSameLine = false;
        // if there is another note after this one
        NotationNote notationNoteNext;
        if ((pitchIterator + 1) < end) {
            MidiNote mnNext(*(pitchIterator + 1));
            // SQINFO("will get nn for next, pitch = %d", mnNext.get());
            notationNoteNext = ScorePitchUtils::getNotationNote(*scale, mnNext, bassStaff);
            const auto yInfoNext = _noteYInfo(mnNext, notationNoteNext._legerLine, bassStaff);
            if (yInfo.position == yInfoNext.position) {
                // SQWARN("two notes at same location pitch = %d, %d, y=%f, %f", mn.get(), mnNext.get(), yInfo.position, yInfoNext.position);

                twoNotesOnSameLine = true;
                ++pitchIterator;
            }
        }

        if (!twoNotesOnSameLine) {
            const bool offsetThisNote = !noteOffsetByTwoLines && !lastNoteOffset;
            //SQINFO("offset flag will be %d y=%f", offsetThisNote, yInfo.position);
            lastNoteOffset = offsetThisNote;
            drawOneNote(args, mn, notationNote, yInfo, xPosition, offsetThisNote);
        } else {
            drawTwoNotes(args, mn, notationNote, notationNoteNext, yInfo, xPosition);
        }
    }
}
