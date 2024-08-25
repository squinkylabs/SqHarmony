#pragma once

// #include "SharpsFlatsPref.h"
#include "NotationNote.h"
#include "ScoreDrawUtils.h"
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
 *      try to space identifiable chords correctly.
 *      try to space all chords correctly.
 *
 * Bugs:
 *      C and A below middle C - draws crazy leger lines(done)
 *      C and D flat in CvMajor, draws two C (done)
 *      C Major chord in C# Major - accidentals get on key sig. (done)
 *      C and E in C major - doesn't draw the C (fixed)
 *      In C Major one note, I don't see A natural, only sharp?? (user error)
 *
 * Misc TODO:
 *      Should NotationNote have a staff in it (yes)?
 *      Make SqArray take {} to initialize.
 *
 * How are sharps/flats handled now?
 * _drawNotes() doesn't care. it divides midi pitches between staves and calls
 * _drawNotesOnStaff, passing midi pitch and a Scale.
 * _drawNotesOnStaff makes NotationNote
 * NotationNote has ScorePitchUtils::Accidental, and int _legerLine;
 * ScorePitchUtils::getNotationNote uses the preference from the scale. Also it calls Scale::m2s,
 * and the resulting ScaleNote does have sharp/flat built in.
 *
 * step 1: remove ScaleNote from NotationNote (done).
 * step 2: break out NotationNote into own class. (done)
 * step 3: make "getAlternateSpelling" method, with unit tests. (done)
 *  (move the spelling stuff back into utils?) (done)
 *  make a util in MidiNote for leger line + accidental -> pitch. (done)
 *  consider breaking out MidiNote unit tests from testNotes.cpp (done)
 *  put clef bool into NotationNote (done)
 * step 4: add util to get vector of spellings. (done)
 * step 5: make validate take a scale, and fix none != natural. (done)
 *      a) make validate take a scale (done).
 *      a2) make a constructor for Scale that takes the arguments we usually pass to set. (done)
 *      b) make MidiNote pitchFromLeger take a scale and a NotationNote::Accidental (including all the unit tests). (done)
 *          may need to move pitchFromLeger into ScorePitchUtils.
 *      c) make validate pass c minor tests.    (done)
 * step 6: more unit tests for validate. natural where not needed, etc...
 *      a) make canonicalize(NotationNote)
 *      b) use it in validate
 *      c) make more unit tests like f# ok.
 * step 8: make ChordRecognizer::_make canonical re-usable:
 *      a) use SqArray
 *      b) templatize it, including sorter
 *      c) use it in speller.
 * step 9: make sure speller is good now., can spell C minor correctly.
 * ste0 10: hook new stuff up to the GUI.
 * step n: use it, re-do scoring (?), make new spelling stuff unit testable. (done)
 *
 * Open items:
 *      1) notes out of range draw funny. Maybe don't draw.
 *      2) accidentals can overlap. score drawing could be better, visually.
 *      3) need user choice for accidentals.
 *          a) In C major, do we use sharps or flats? (def+sharp, def+flat). (done)
 *          b) when drawing an accidental all by itself, use sharps or flats? (sharp, flat).
 *          c) make spelling pay attention to setting
 *      4) make key select UI respond to sharp/flats in key, like harmony2.
 *      5) should enharmonic spelling avoid mixing sharps and flats?
 *
 *
 * ksig name                    accidental gfx in cmaj          in cmin
 * def+sharp                    sharp                           flat
 * del+flat                     flat                            flat
 * sharp                        sharp                           sharp
 * flat                         flat                            flat
 *
 * To fix enh spelling of chord name.
 * make new API ChordRecognizer::toString(const ChordInfo& info, bool useSharps);
 * Make a new conversions function: f(scale, uiPref) -> useSharps)
 *      use it where we do this already.
 *      use below
 * add tests for new API.
 * call API from VisualizerModule. use new conversion API.
 *
 *
 * For new drawing:
 *      make single note draw in correct place (done)
 *      implement bass staff (done)
 *          bug: notes below middle C draw in crazy place.
 *              ypos for pitch 70 ll -4 bass 1 is 94.688004 (note right below middle c
 *              ypos for pitch 72 ll -2 bass 0 is 49.344002 (middle c)
 *              clearly the ll is whack for bass notes... Where does that come from
 *      draw single accidental in correct place (done)
 *      two note on one line (done)
 *      draw leger lines, when needed (done)
 *      C and C# draws as C and D# (done)
 *      F and F# draw both on F in G major (done)
 *      get rid of midi note param in pos callback, if not needed.
 *      in spelling, discourage to notes on same line. (done)
 *      make drawing of multiple accidentals look good.
 *      fix overlap of notes on adjacent leger lines.
 *      get rid of NotationNote::makeCanonical(), if not used. (done)
 *      get rid of reSpell, it not used.
 *
 *
 */

// #define _LOG

#define _NEWDRAW

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

    const std::string _gClef = u8"\ue050";
    const std::string _fClef = u8"\ue062";

    //  const std::string _legerLine = u8"\ue022";
    const std::string _legerLineWide = u8"\ue023";

    const std::string _sharp = u8"\ue262";
    const std::string _flat = u8"\ue260";

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
    const float _xBarlineEnd = 109.5;  // 105.5 a little small
    const float _xBarlineFirst = _leftMargin;
    const float _xKeysig = 16 * _zoom;             // x position of the first accidental in the key signature.
    const float _deltaXAccidental = -2.2 * _zoom;  // accidental drawn this far from note, in x di
                                                   // for single note, -8 is way too much, -4 tolerable, -2 just about touching.
                                                   // 0 is on top, as it should be. -2.2 is pretty good for single notes.
    const float _columnWidth = 6 * _zoom;

    const float _noteXIndent = 6;  // Distance from the keysig to the first note, horizontally.

    const float _ySpaceBetweenLines = 1.67f * _zoom;           // 1.7 slightly high
                                                               // 1.65 low
    const float _barlineHeight = _yBassStaff + (-14 * _zoom);  // 13.5 sticks up  15 gap

    float _drawMusicNonNotes(const DrawArgs &args) const;  // returns x pos at end of ksig
    void _drawNotes(const DrawArgs &args, float xPosition) const;

    // This is limited to white keys. Only used for keysig drawing.
    float _noteY(const MidiNote &note, bool bassStaff) const;

    class YInfo {
    public:
        float position = 200;
        float legerPos[3] = {0};
    };
    YInfo _noteYInfo(const MidiNote &note, int legerLine, bool bassStaff) const;
    void _drawLegerLinesForNotes(const DrawArgs &args, const YInfo &uInfo, float xPos) const;
    void _drawLegerLinesForNotes2(const DrawArgs &args, const LegerLinesLocInfo &uInfo, float xPos) const;
    void _drawStaff(const DrawArgs &args, float y) const;
    void _drawBarLine(const DrawArgs &args, float x, float y) const;
    void _drawAccidental(const DrawArgs &args, float x, float y, NotationNote::Accidental accidental) const;
    ;
    /**
     * @return float width of key signature
     */
    std::pair<float, float> _drawKeysig(const DrawArgs &args, ConstScalePtr scale, bool trebleClef, float y) const;

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
    // SQINFO("filled rect w = %f", w);
    //  w = std::min(w, 88.f);  //  clip with to 80, temp (TODO: do we need this?)

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

    const float xPosition = ((left + right) / 2.f);
   // SQINFO("draw. l=%f, r=%f col=%f xPos=%f", left, right, _columnWidth, xPosition);

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
        const auto a = _drawKeysig(args, scale, true, _yTrebleStaff);
        const auto b = _drawKeysig(args, scale, false, _yBassStaff);
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

inline std::pair<float, float> ScoreChord::_drawKeysig(const DrawArgs &args, ConstScalePtr scale, bool treble, float y) const {
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
    const int legerLine = note.getLegerLine(ResolvedSharpsFlatsPref::Sharps, bassStaff);
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

inline void ScoreChord::_drawLegerLinesForNotes(const DrawArgs &args, const YInfo &yInfo, float xPosition) const {
    for (int i = 0; i < 3; ++i) {
        if (yInfo.legerPos[i] != 0) {
            nvgText(args.vg, xPosition, yInfo.legerPos[i], _legerLineWide.c_str(), NULL);
        }
    }
}

inline void ScoreChord::_drawLegerLinesForNotes2(const DrawArgs &args, const LegerLinesLocInfo &llli, float xPosition) const {
    for (int i = 0; i < 3; ++i) {
        if (llli.legerPos[i] != 0) {
            // SQINFO("drawing a wide leger at %f,%f", xPosition, llli.legerPos[i]);
            nvgText(args.vg, xPosition, llli.legerPos[i], _legerLineWide.c_str(), NULL);
        }
    }
}

inline void ScoreChord::_drawNotes(const DrawArgs &args, float xPosition) const {
    if (!_module) {
        return;
    }

    const auto pitches = _module->getQuantizedPitches();
    const unsigned channels = pitches.numValid();
    const int *originalPitches = pitches.getDirectPtrAt(0);
    if (channels <= 0) {
        return;  // return is nothing to draw
    }

    ConstScalePtr scale = _module->getScale();
    SqArray<int, 16> inputNotes(originalPitches, originalPitches + channels);
    UIPrefSharpsFlats pref = _module->getSharpsFlatsPref();
    ScoreDrawUtilsPtr scoreDrawUtils = ScoreDrawUtils::make();
    DrawPositionParams drawPostion;
    drawPostion.noteXPosition = xPosition;
    drawPostion.noteColumnWidth = _columnWidth;
    drawPostion.accidentalColumnWidth = _columnWidth * .7;
    drawPostion.accidentalHeight = 15;
    drawPostion.noteYPosition = [this](const MidiNote &note, int legerLine, bool bassStaff) {
        YInfo yInfo = this->_noteYInfo(note, legerLine, bassStaff);
        const float ret = yInfo.position;
        // SQINFO("ypos for pitch %d ll %d bass %d is %f", note.get(), legerLine, bassStaff, ret);
        return ret;
    };

    drawPostion.llDrawInfo = [this](const MidiNote &note, int legerLine, bool bassStaff) {
        YInfo yInfo = this->_noteYInfo(note, legerLine, bassStaff);
        //  SQINFO("in ll callback, linfo 0 = %f, %f, %f", yInfo.legerPos[0], yInfo.legerPos[1], yInfo.legerPos[2]);
        LegerLinesLocInfo ret;
        for (int i = 0; i < 3; ++i) {
            ret.legerPos[i] = yInfo.legerPos[i];
            // SQINFO("copied %d to %f", i, )
        }
        // SQINFO("callback will ret %f, %f, %f", ret.legerPos[0], ret.legerPos[1], ret.legerPos[2]);
        return ret;
    };

    auto info = scoreDrawUtils->getDrawInfo(drawPostion, *scale, inputNotes, pref);

    for (auto mapIterator = info.begin(); mapIterator != info.end(); mapIterator++) {
        // SQINFO("something to draw : %s", mapIterator->second.toString().c_str());
        const auto llLocInfo = mapIterator->second.legerLinesLocInfo;
        // SQINFO("got info from func %f", llLocInfo.legerPos[0]);
        for (
            auto symbolIterator = mapIterator->second.notes.begin();
            symbolIterator != mapIterator->second.notes.end();
            ++symbolIterator) {
            const auto symbol = *symbolIterator;
            // SQINFO("drawing symbol %s", symbol.toString().c_str());
            nvgText(args.vg, symbol.xPosition, symbol.yPosition, symbol.glyph.c_str(), NULL);
            _drawLegerLinesForNotes2(args, llLocInfo, symbol.xPosition);
        }

        for (
            auto symbolIterator = mapIterator->second.accidentals.begin();
            symbolIterator != mapIterator->second.accidentals.end();
            ++symbolIterator) {
            const auto symbol = *symbolIterator;
            // SQINFO("drawing symbol %s", symbol.toString().c_str());
            nvgText(args.vg, symbol.xPosition, symbol.yPosition, symbol.glyph.c_str(), NULL);
            // _drawLegerLinesForNotes2(args, llLocInfo, symbol.xPosition);
        }
    }
}
