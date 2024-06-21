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
 * make note not overlap with last in stacked close chords.
 * put in the natural and accidental if overlap.
 * make it look decent.
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
    const std::string _flat = u8"\ue260";
    const std::string _natural = u8"\ue261";
    const std::string _sharp = u8"\ue262";

    const float _zoom = 1.6;

    const float topMargin = 27.5f * _zoom;
    const float yTrebleStaff = topMargin + 0;
    const float yBassStaff = yTrebleStaff + (34);
    const float yTrebleClef = yTrebleStaff - (3.3 * _zoom);  // 3 a little low, 4 way high
    const float yBassClef = yBassStaff - (10 * _zoom);       // 11 too much
    const float yNoteInfo = yBassStaff + (17 * _zoom);       // 0 too high 12 for a long time...

    // X axis pos
    const float leftMargin = 4.5f;
    const float xStaff = leftMargin;
    const float _xClef = xStaff + 2;
    const float _xClefWidth = 8 * _zoom;
    const float xNote0 = _xClef + 18 * _zoom;
    const float _deltaXAccidental = -6;  // accidental drawn this far from note, in x di
    const float deltaXNote = 13;         // 8 seemed close. was 10 for a long time. 12 is good now, try 13
    const float _noteXIndent = 6;        // Distance from the keysig to the first note, horizontally.

    const float _ySpaceBetweenLines = 1.67f * _zoom;  // 1.7 slightly high
                                                      // 1.65 low
    const float barlineHeight = 55.5;                 // 55 low
                                                      // 57 went high
    const float _paddingAfterKeysig = 9 * _zoom;      // space between the clef or clef + keysig and the first note

    std::pair<float, float> drawMusicNonNotes(const DrawArgs &args) const;
    void drawNotes(const DrawArgs &args, std::pair<float, float> keysigLayout) const;
    float noteY(const MidiNote &note, bool bassStaff) const;

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

    void drawStaff(const DrawArgs &args, float y) const;
    void drawBarLine(const DrawArgs &args, float x, float y) const;
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
    w = std::min(w, 80.f);  //  clip with to 80, temp (TODO: do we need this?)
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
    const auto keysigLayout = drawMusicNonNotes(args);
    drawNotes(args, keysigLayout);
    _scoreIsDirty = false;

    Widget::draw(args);
}

inline std::pair<float, float> ScoreChord::drawMusicNonNotes(const DrawArgs &args) const {
    NVGcolor color = getBackgroundColor();

    filledRect(args.vg, color, 0, 0, box.size.x, box.size.y, 5);

    prepareFontMusic(args);

    color = getForegroundColor();
    // Background text
    nvgFillColor(args.vg, color);

    drawStaff(args, yTrebleStaff);
    nvgText(args.vg, _xClef, yTrebleClef, _gClef.c_str(), NULL);

    drawStaff(args, yBassStaff);
    nvgText(args.vg, _xClef, yBassClef, _fClef.c_str(), NULL);

    float keysigWidth = 0;
    float keysigEnd = 0;

    if (_module) {
        auto scale = _module->getScale();
        const auto a = drawKeysig(args, scale, true, yTrebleStaff);
        const auto b = drawKeysig(args, scale, false, yBassStaff);
        keysigWidth = std::max(keysigWidth, a.first);
        keysigWidth = std::max(keysigWidth, b.first);

        keysigEnd = std::max(keysigWidth, a.second);
        keysigEnd = std::max(keysigWidth, b.second);
    }
    const std::pair<float, float> ksStuff = std::make_pair(keysigWidth, keysigEnd);

    drawBarLine(args, xStaff, yBassStaff);

    const float secondBarLineX = 2 + .5f * (noteXPos(3, ksStuff) + noteXPos(4, ksStuff));
    drawBarLine(args, secondBarLineX, yBassStaff);

    const float barlineX2 = args.clipBox.size.x - leftMargin;
    drawBarLine(args, barlineX2, yBassStaff);

    return std::make_pair(keysigWidth, keysigEnd);
}

inline ScoreChord::YInfo ScoreChord::noteYInfo(const MidiNote &note, bool bassStaff) const {
    YInfo ret;
    if (note.get() < 10) {
        return ret;
    }

    float y = 0;

    const int ledgerLine = note.getLedgerLine(bassStaff);
    const float staffBasePos = bassStaff ? yBassStaff : yTrebleStaff;

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

inline void ScoreChord::drawBarLine(const DrawArgs &args, float x, float y) const {
    auto color = getForegroundColor();
    drawVLine(args.vg, color, x, y, barlineHeight, .75f);
}

inline float ScoreChord::noteXPos(int noteNumber, std::pair<float, float> _keysigLayout) const {
    const float keysigXEnds = _keysigLayout.second;
    const float totalWidth = box.size.x - 2 * leftMargin;
    const float totalWidthForNotes = totalWidth - keysigXEnds;
    const float delta = totalWidthForNotes / 8.5;
    const float firstNotePosition = leftMargin + keysigXEnds + _noteXIndent;

    SQINFO("noteXPos called with noteNumber = %d, keysig layout = %f, %f", noteNumber, _keysigLayout.first, _keysigLayout.second);
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
    float pos = _xClef + _paddingAfterKeysig + _xClefWidth;
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

    const float widthOfSharpFlat = 4 * _zoom;
    const char *character = (areFlats) ? _flat.c_str() : _sharp.c_str();
    if (num) {
        float w = 0;
        float p = 0;
        for (int i = 0; i < num; ++i) {
            const float x = _xClef + +_xClefWidth + _paddingAfterKeysig + w;
            const int note = accidentals[i].get();
            const float yf = noteY(MidiNote(note), !treble);
            nvgText(args.vg, x, yf, character, NULL);
            w += widthOfSharpFlat;
            p = std::max(p, x + widthOfSharpFlat);
        }
        width = std::max(width, w);
        pos = std::max(pos, p);
    }
    return std::make_pair(width, pos);
}

float ScoreChord::noteY(const MidiNote &note, bool bassStaff) const {
    float y = 0;
    const float staffBasePos = bassStaff ? yBassStaff : yTrebleStaff;
    const int ledgerLine = note.getLedgerLine(bassStaff);
    y -= ledgerLine * _ySpaceBetweenLines;
    y += staffBasePos;
    return y;
}

inline void ScoreChord::drawStaff(const DrawArgs &args, float yBase) const {
    const float x = xStaff;
    const float length = args.clipBox.size.x - 2 * leftMargin;
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
            nvgText(args.vg, xPosition, yInfo.ledgerPos[i], _ledgerLine.c_str(), NULL);
        }
    }
}

inline void ScoreChord::drawTwoNotes(
    const DrawArgs &args,
    const MidiNote &note,
    const std::tuple<ScaleNote, ScorePitchUtils::Accidental> &notationNote,
    const std::tuple<ScaleNote, ScorePitchUtils::Accidental> &notationNote2,
    const YInfo &yInfo,
    float xPosition) const {
    SQINFO("!! two notes nimp");
}

inline void ScoreChord::drawOneNote(
    const DrawArgs &args,
    const MidiNote &note,
    const std::tuple<ScaleNote, ScorePitchUtils::Accidental> &notationNote,
    const YInfo &yInfo,
    float xPosition,
    bool offsetNote) const {
    SQINFO("  drawOneNote offset=%d", offsetNote);
    drawLedgerLinesForNotes(args, yInfo, xPosition);
    const char *notePtr = _wholeNote.c_str();
    nvgText(args.vg, xPosition, yInfo.position, notePtr, NULL);
    if (std::get<1>(notationNote) != ScorePitchUtils::Accidental::none) {
        std::string symbol = "";
        switch (std::get<1>(notationNote)) {
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
        nvgText(args.vg, xPosition + _deltaXAccidental, yInfo.position, symbol.c_str(), NULL);
    }
}

inline void ScoreChord::drawNotes(const DrawArgs &args, std::pair<float, float> keysigLayout) const {
    if (!_module) {
        return;
    }

    const auto pitchesAndChannels = _module->getQuantizedPitchesAndChannels();
    const unsigned channels = std::get<1>(pitchesAndChannels);
    const int *originalPitches = std::get<0>(pitchesAndChannels);
    if (channels <= 0) {
        SQINFO("On change 274, nothing");
        return;
    }
    SQINFO("change, channels=%d", channels);

    ConstScalePtr scale = _module->getScale();
    int copyOfPitches[16];
    for (unsigned i = 0; i < channels; ++i) {
        copyOfPitches[i] = originalPitches[i];
        SQINFO("copied %d at index %d", copyOfPitches[i], i);
    }
    std::sort(copyOfPitches, copyOfPitches + channels);

    const float xPosition = noteXPos(0, keysigLayout);
    int lastYPos = 1000;
    bool lastNoteOffset = false;

    // for (unsigned i = 0; i < channels; ++i) {
    //     MidiNote mn(copyOfPitches[i]);
    //     SQINFO("before loop, pitch[%d] = %d", i, copyOfPitches[i]);
    // }
    for (unsigned i = 0; i < channels; ++i) {
        MidiNote mn(copyOfPitches[i]);
        const auto notationNote = ScorePitchUtils::getNotationNote(*scale, mn);
        const auto yInfo = noteYInfo(mn, false);
        SQINFO("in draw loop i=%d pitch=%d y=%f", i, mn.get(), yInfo.position);
        const float distance = lastYPos - yInfo.position;
        lastYPos = yInfo.position;
        const bool noteOffsetByTwoLines = (distance / _ySpaceBetweenLines) > 1.2;

        bool twoNotesOnSameLine = false;
        std::tuple<ScaleNote, ScorePitchUtils::Accidental> notationNoteNext;
        // if there is a line after us
        if (i < channels - 1) {
            MidiNote mnNext(copyOfPitches[i + 1]);
            const auto yInfoNext = noteYInfo(mnNext, false);
            if (yInfo.position == yInfoNext.position) {
                SQWARN("two notes at same location pitch = %d, %d, y=%f, %f", mn.get(), mnNext.get(), yInfo.position, yInfoNext.position);

                twoNotesOnSameLine = true;
                notationNoteNext = ScorePitchUtils::getNotationNote(*scale, mnNext);
                ++i;
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

        //  distance = 2.161999 between lines=2.338000 next=0

        //  SQINFO("distance = %f between lines=%f next=%d", distance, _ySpaceBetweenLines, nextLine);

        // drawOneNote(args, mn, notationNote, yInfo, xPosition);
    }
}
