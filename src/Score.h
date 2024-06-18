#pragma once

#include <assert.h>
#include <stdio.h>

#include <codecvt>
#include <locale>
#include <string>

#include "BufferingParent.h"
#include "Harmony1Module.h"
#include "plugin.hpp"

// #define _TESTCHORD
class Score : public app::LightWidget, public Dirty {
public:
    Score(Harmony1Module *);
    ~Score();
    static int _refCount;
    void step() override;
    void draw(const DrawArgs &args) override;
    //   void drawLayer(const DrawArgs &args, int layer) override;
    bool isDirty() const override {
        // SQINFO("Score::isDirty will ret %d", scoreIsDirty);
        return _scoreIsDirty;
    }

    void setWhiteOnBlack(bool b) {
        if (_whiteOnBlack != b) {
            _whiteOnBlack = b;
            _scoreIsDirty = true;
        }
    }

private:
    bool _scoreIsDirty = false;
    void filledRect(NVGcontext *vg, NVGcolor color, float x, float y, float w, float h, float rounding) const;
    void drawHLine(NVGcontext *vg, NVGcolor color, float x, float y, float length, float width) const;
    void drawVLine(NVGcontext *vg, NVGcolor color, float x, float y, float length, float width) const;

    void prepareFontMusic(const DrawArgs &args) const;
    void prepareFontText1(const DrawArgs &args) const;
    void prepareFontText2(const DrawArgs &args) const;



    class YInfo {
    public:
        float position = 200;
        float ledgerPos[3] = {};
    };
    /**
     * @brief get info about vertical placement
     *
     * @param note
     * @param bassStaff
     * @return std::pair<float, bool> first is the y position, second it flag if need ledger line
     */
    YInfo noteYInfo(const MidiNote &note, bool bassStaff) const;
    float noteY(const MidiNote &note, bool bassStaff) const;

    Harmony1Module *const module;
    std::list<Comp::Chord> chords;
    bool _whiteOnBlack = false;  // Default to black notes for better look in module browser.

    const std::string noteQuarterUp = u8"\ue1d5";
    const std::string noteQuarterDown = u8"\ue1d6";
    const std::string staffFiveLines = u8"\ue014";
    const std::string gClef = u8"\ue050";
    const std::string fClef = u8"\ue062";
    const std::string ledgerLine = u8"\ue022";
    const std::string flat = u8"\ue260";
    const std::string sharp = u8"\ue262";

    void drawStaff(const DrawArgs &args, float y) const;
    void drawBarLine(const DrawArgs &args, float x, float y) const;
    float drawChordRoot(const DrawArgs &args, float x, const Comp::Chord &chord) const;
    void drawChordInversion(const DrawArgs &args, float x, const Comp::Chord &chord) const;
    /**
     * @return std::pair, float width of key signature and float position of the end (which is all we really need)
     * if we end up just using second, then we can go back to float
     */
    std::pair<float, float> drawMusicNonNotes(const DrawArgs &args) const;
    void drawChordNumbers(const DrawArgs &args, std::pair<float, float> keysigLayout) const;
    void drawNotes(const DrawArgs &args, std::pair<float, float> keysigLayout) const;
    /**
     * @brief figure out the x position to draw a note
     *
     * @param noteNumber is the index of the note - 0..7
     * @param keysigWidth is the absolute of the keysig drawing - 0 for cmaj
     * @return float absolute x position
     */
    float noteXPos(int noteNumber, std::pair<float, float> keysigLayout) const;

    /**
     * @return float width of key signature
     */
    std::pair<float, float> drawKeysig(const DrawArgs &args, ConstScalePtr scale, bool trebleClef, float y) const;
    float ledgerLine2Pos(int ledgerLine, bool bassStaff) const;

    NVGcolor getForegroundColor() const;
    NVGcolor getBackgroundColor() const;

    // Y axis pos

    // topMargin = 36.5f;  is original
    const float topMargin = 27.5f;
    const float yTrebleStaff = topMargin + 0;
    const float yBassStaff = yTrebleStaff + 42;    // 28 way too close
    const float yTrebleClef = yTrebleStaff - 3.3;  // 3 a little low, 4 way high
    const float yBassClef = yBassStaff - 10;       // 11 too much
    const float yNoteInfo = yBassStaff + 17;       // 0 too high 12 for a long time...

    // X axis pos
    const float leftMargin = 4.5f;
    const float xStaff = leftMargin;
    const float xClef = xStaff + 2;
    const float xNote0 = xClef + 18;

    const float deltaXNote = 13;            // 8 seemed close. was 10 for a long time. 12 is good now, try 13
    const float spaceBetweenLines = 1.67f;  // 1.7 slightly high
                                            // 1.65 low
    const float barlineHeight = 55.5;       // 55 low
                                            // 57 went high
    const float paddingAfterKeysig = 9;     // space between the clef or clef + keysig and the first note

    //  const float barlineX0 = leftMargin;
    const float barlineX1 = xClef + 61;  //  small 63 big
    // const float barlineX2 = barlineX1 + 43;     // 40 too small 46 too big
};

NVGcolor Score::getForegroundColor() const {
    return _whiteOnBlack ? nvgRGB(0xff, 0xff, 0xff) : nvgRGB(0, 0, 0);
}

NVGcolor Score::getBackgroundColor() const {
    return _whiteOnBlack ? nvgRGB(0, 0, 0) : nvgRGB(0xff, 0xff, 0xff);
}

inline Score::~Score() {
    --_refCount;
}

int Score::_refCount = 0;

inline Score::Score(Harmony1Module *m) : module(m) {
    ++_refCount;
#ifdef _TESTCHORD
    Comp::Chord ch;

    ch.pitch[0] = 0;
    ch.pitch[1] = 0;
    ch.pitch[2] = 0;
    ch.pitch[3] = 0;

    // bass
    ch.pitch[0] = MidiNote::MiddleC - (12);
    ch.pitch[1] = MidiNote::MiddleC;
    ch.pitch[2] = MidiNote::MiddleC + 4;
    ch.pitch[3] = MidiNote::MiddleC + 8;
    chords.push_back(ch);
    chords.push_back(ch);
    chords.push_back(ch);
    chords.push_back(ch);
    chords.push_back(ch);
    chords.push_back(ch);
    chords.push_back(ch);
    chords.push_back(ch);
    chords.push_back(ch);
#else
    //   if (!module) {
    Comp::Chord c;
    c.inversion = 0;
    c.root = 1;
    c.pitch[0] = MidiNote(60);
    c.pitch[1] = MidiNote(72);
    c.pitch[2] = MidiNote(76);
    c.pitch[3] = MidiNote(79);
    chords.push_back(c);

    c.inversion = 0;
    c.root = 3;
    c.pitch[0] = MidiNote(64);
    c.pitch[1] = MidiNote(71);
    c.pitch[2] = MidiNote(76);
    c.pitch[3] = MidiNote(79);
    chords.push_back(c);

    c.inversion = 1;
    c.root = 2;
    c.pitch[0] = MidiNote(65);
    c.pitch[1] = MidiNote(69);
    c.pitch[2] = MidiNote(74);
    c.pitch[3] = MidiNote(77);
    chords.push_back(c);

    c.inversion = 0;
    c.root = 7;
    c.pitch[0] = MidiNote(59);
    c.pitch[1] = MidiNote(71);
    c.pitch[2] = MidiNote(74);
    c.pitch[3] = MidiNote(77);
    chords.push_back(c);

    c.inversion = 1;
    c.root = 3;
    c.pitch[0] = MidiNote(55);
    c.pitch[1] = MidiNote(71);
    c.pitch[2] = MidiNote(76);
    c.pitch[3] = MidiNote(79);
    chords.push_back(c);

    c.inversion = 0;
    c.root = 4;
    c.pitch[0] = MidiNote(53);
    c.pitch[1] = MidiNote(72);
    c.pitch[2] = MidiNote(77);
    c.pitch[3] = MidiNote(81);
    chords.push_back(c);

    c.inversion = 0;
    c.root = 5;
    c.pitch[0] = MidiNote(55);
    c.pitch[1] = MidiNote(71);
    c.pitch[2] = MidiNote(74);
    c.pitch[3] = MidiNote(79);
    chords.push_back(c);

    c.inversion = 0;
    c.root = 1;
    c.pitch[0] = MidiNote(60);
    c.pitch[1] = MidiNote(67);
    c.pitch[2] = MidiNote(72);
    c.pitch[3] = MidiNote(76);
    chords.push_back(c);
#endif
}

inline void Score::step() {
#ifndef _TESTCHORD
    if (module) {
        if (module->isChordAvailable()) {
            // SQINFO("saw a new chord in UI, will dirty push");
            _scoreIsDirty = true;
            auto ch = module->getChord();
            chords.push_back(ch);
            // SQINFO("Now ui has %u", unsigned(chords.size()));
            if (chords.size() > 8) {
                while (chords.size() > 1) {
                    // SQINFO("getting rid of stale input more than 8 chords");
                    chords.pop_front();
                }
            }
        }
    }
#endif
    Widget::step();
}

inline float Score::noteXPos(int noteNumber, std::pair<float, float> _keysigLayout) const {
    const float keysigXEnds = _keysigLayout.second;
    const float totalWidth = box.size.x - 2 * leftMargin;
   // const float totalWidthForNotes = totalWidth - keysigWidth;
    const float totalWidthForNotes = totalWidth - keysigXEnds;
    const float delta = totalWidthForNotes / 8.5;
    const float firstNotePosition = leftMargin + keysigXEnds;

//    const float firstNotePosition = leftMargin + keysigWidth;
    float x = firstNotePosition + noteNumber * delta;
    if (noteNumber > 3) {
        // little bump into the next bar. Used to be a while delta, the /2 is new.
        x += (delta / 2.f);
    }
    // SQINFO("n=%d totalW=%f for notes = %f", noteNumber, totalWidth, totalWidthForNotes);
    // SQINFO(" first note pos = %f, ret x=%f keysig end = %f", firstNotePosition, x, keysigXEnds);
    return x;
}

#if 0  // original version
inline float Score::noteXPos(int noteNumber, float keysigWidth) const {
    // Squeeze notes together a bit to accommodate keysig.
    const float inset = (keysigWidth >= 2) ? std::max(keysigWidth - 4, 2.f) : 0;
    SQINFO("ksig w = %f, inset = %f, delta mod=%f", keysigWidth, inset, (1.f - (inset / 70.f)));

    // What is this inset / 70? a heuristic?
    const float delta = deltaXNote * (1.f - (inset / 70.f));
  //  const float delta = deltaXNote;
    float x = xNote0 + inset + noteNumber * delta;
    if (noteNumber > 3) {
        // little bump into the next bar. Used to be a while delta, the /2 is new.
        x += (delta / 2.f);
    }
    return x;
}
#endif

float Score::noteY(const MidiNote &note, bool bassStaff) const {
    float y = 0;
    const float staffBasePos = bassStaff ? yBassStaff : yTrebleStaff;
    const int ledgerLine = note.getLedgerLine(bassStaff);
    y -= ledgerLine * spaceBetweenLines;
    y += staffBasePos;
    return y;
}

inline Score::YInfo Score::noteYInfo(const MidiNote &note, bool bassStaff) const {
    YInfo ret;
    if (note.get() < 10) {
        return ret;
    }

    float y = 0;

    const int ledgerLine = note.getLedgerLine(bassStaff);
    const float staffBasePos = bassStaff ? yBassStaff : yTrebleStaff;

    if (ledgerLine < -1) {
        ret.ledgerPos[0] = staffBasePos + (2.f * spaceBetweenLines);
    }
    if (ledgerLine < -3) {
        ret.ledgerPos[1] = staffBasePos + (4.f * spaceBetweenLines);
    }
    if (ledgerLine < -5) {
        ret.ledgerPos[2] = staffBasePos + (6.f * spaceBetweenLines);
    }
    if (ledgerLine > 9) {
        ret.ledgerPos[0] = staffBasePos + (-10.f * spaceBetweenLines);
    }
    if (ledgerLine > 11) {
        ret.ledgerPos[1] = staffBasePos + (-12.f * spaceBetweenLines);
    }
    if (ledgerLine > 13) {
        ret.ledgerPos[2] = staffBasePos + (-14.f * spaceBetweenLines);
    }

    y -= ledgerLine * spaceBetweenLines;
    y += staffBasePos;

    ret.position = y;
    return ret;
}

#if 0
// experiment to make notes glow
inline void Score::drawLayer(const DrawArgs &args, int layer) {
    if (layer == 1) {
        prepareFontMusic(args);
        drawNotes(args);
        INFO("draw layer 1");
    }
    Widget::drawLayer(args, layer);
}
#endif

inline void Score::draw(const DrawArgs &args) {
    nvgScissor(args.vg, RECT_ARGS(args.clipBox));
    const auto keysigLayout = drawMusicNonNotes(args);
    drawNotes(args, keysigLayout);
    drawChordNumbers(args, keysigLayout);
    _scoreIsDirty = false;

    Widget::draw(args);
}

inline void Score::drawChordNumbers(const DrawArgs &args, std::pair<float, float> keysigLayout) const {
    if (chords.empty()) {
        return;
    }
    float endOfChordRoots[8];
    prepareFontText1(args);
    int i = 0;
    for (auto chord : chords) {
        const float x = noteXPos(i, keysigLayout) - 2;
        const float xPos = drawChordRoot(args, x, chord);
        endOfChordRoots[i] = xPos;
        ++i;
    }
    prepareFontText2(args);
    i = 0;
    for (auto chord : chords) {
        //   const float x = noteXPos(i, widthOfKeysig) + 1.5;
        drawChordInversion(args, endOfChordRoots[i], chord);
        ++i;
    }
}

inline void Score::drawNotes(const DrawArgs &args,  std::pair<float, float> keysigLayout) const {
    if (!chords.empty()) {
        int i = 0;
        for (auto chord : chords) {
            const float x = noteXPos(i, keysigLayout);

            for (int i = 0; i < 4; ++i) {
                const bool stemUp = i % 2;
                auto yInfo = noteYInfo(chord.pitch[i], i < 2);

                for (int i = 0; i < 3; ++i) {
                    if (yInfo.ledgerPos[i] != 0) {
                        nvgText(args.vg, x, yInfo.ledgerPos[i], ledgerLine.c_str(), NULL);
                    }
                }
                const char *note = stemUp ? noteQuarterUp.c_str() : noteQuarterDown.c_str();
                nvgText(args.vg, x, yInfo.position, note, NULL);
            }
            ++i;
        }
    }
}

inline std::pair<float, float> Score::drawKeysig(const DrawArgs &args, ConstScalePtr scale, bool treble, float y) const {
    const auto info = scale->getScoreInfo();
    float width = 0;
    float pos = xClef + paddingAfterKeysig;
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

    const char *character = (areFlats) ? flat.c_str() : sharp.c_str();
    if (num) {
        float w = 0;
        float p = 0;
        for (int i = 0; i < num; ++i) {
            const float x = xClef + paddingAfterKeysig + w;
            const int note = accidentals[i].get();
            const float yf = noteY(MidiNote(note), !treble);
            nvgText(args.vg, x, yf, character, NULL);
            w += 4;
            p = std::max(p, x + 4);
            //p += 4;
        }
        width = std::max(width, w);
        pos = std::max (pos, p);
    }
    // SQINFO("drawKeysig returning %f,%f", width, pos);
    return std::make_pair(width, pos);
}

inline std::pair<float, float> Score::drawMusicNonNotes(const DrawArgs &args) const {
    NVGcolor color = getBackgroundColor();

    filledRect(args.vg, color, 0, 0, box.size.x, box.size.y, 5);

    prepareFontMusic(args);

    color = getForegroundColor();
    // Background text
    nvgFillColor(args.vg, color);

    drawStaff(args, yTrebleStaff);
    nvgText(args.vg, xClef, yTrebleClef, gClef.c_str(), NULL);

    drawStaff(args, yBassStaff);
    nvgText(args.vg, xClef, yBassClef, fClef.c_str(), NULL);

    float keysigWidth = 0;
    float keysigEnd = 0;

    if (module) {
        auto scale = module->getScale();
        const auto a = drawKeysig(args, scale, true, yTrebleStaff);
        const auto b = drawKeysig(args, scale, false, yBassStaff);
        keysigWidth = std::max(keysigWidth, a.first);
        keysigWidth = std::max(keysigWidth, b.first);

        keysigEnd = std::max(keysigWidth, a.second);
        keysigEnd = std::max(keysigWidth, b.second);
    }
    const std::pair<float, float> ksStuff = std::make_pair(keysigWidth, keysigEnd);

    drawBarLine(args, xStaff, yBassStaff);

  // const float secondBarLineX = 3 + .5f * (noteXPos(3, keysigWidth) + noteXPos(4, keysigWidth));
    const float secondBarLineX = 2 + .5f * (noteXPos(3, ksStuff) + noteXPos(4, ksStuff));
    drawBarLine(args, secondBarLineX, yBassStaff);

    const float barlineX2 = args.clipBox.size.x - leftMargin;
    drawBarLine(args, barlineX2, yBassStaff);

    return std::make_pair(keysigWidth, keysigEnd);;
}

inline std::string intToRoman(int number) {
    std::string output;
    switch (number) {
        case 1:
            output = "I";
            break;
        case 2:
            output = "II";
            break;
        case 3:
            output = "III";
            break;
        case 4:
            output = "IV";
            break;
        case 5:
            output = "V";
            break;
        case 6:
            output = "VI";
            break;
        case 7:
            output = "VII";
            break;
        default:
            assert(false);
    }
    return output;
}

// This is the new roman numeral way
inline float Score::drawChordRoot(const DrawArgs &args, float x, const Comp::Chord &chord) const {
    const std::string rootRoman = intToRoman(chord.root);

    nvgText(args.vg, x, yNoteInfo, rootRoman.c_str(), NULL);
    const float rootWidth = nvgTextBounds(args.vg, x, yNoteInfo, rootRoman.c_str(), NULL, nullptr);
    return x + rootWidth;
}

inline void Score::drawChordInversion(const DrawArgs &args, float x, const Comp::Chord &chord) const {
    // const std::string rootRoman = intToRoman(chord.root);
    switch (chord.inversion) {
        case 1:
            nvgText(args.vg, x, yNoteInfo - 5, "6", NULL);
            break;
        case 2:
            nvgText(args.vg, x, yNoteInfo - 5, "6", NULL);
            nvgText(args.vg, x, yNoteInfo - 1, "4", NULL);
            break;
    }
}

#if 0  // this is the old way
inline void Score::drawChordInfo(const DrawArgs &args, float x, const Comp::Chord &chord) const {
    {
        std::stringstream s;
        s << chord.root;
        nvgText(args.vg, x, yNoteInfo, s.str().c_str(), NULL);
        // SQINFO("draw %s at x=%f y=%f", s.str().c_str(), x, yNoteInfo);
    }
    {
        std::stringstream s;
        s << chord.inversion;
        nvgText(args.vg, x, yNoteInfo + 8, s.str().c_str(), NULL);
    }
}
#endif

inline void Score::drawStaff(const DrawArgs &args, float yBase) const {
    const float x = xStaff;
    const float length = args.clipBox.size.x - 2 * leftMargin;
    const auto color = getForegroundColor();
    for (int i = 0; i < 5; ++i) {
        float y = yBase - 2.f * float(i) * spaceBetweenLines;
        drawHLine(args.vg, color, x, y, length, .5f);
    }
}

inline void Score::drawBarLine(const DrawArgs &args, float x, float y) const {
    auto color = getForegroundColor();
  //  drawVLine(args.vg, color, x, y, barlineHeight, .5f);
   drawVLine(args.vg, color, x, y, barlineHeight, .75f);
}

void Score::drawVLine(NVGcontext *vg, NVGcolor color, float x, float y, float length, float width) const {
    nvgBeginPath(vg);
    nvgMoveTo(vg, x, y);
    nvgLineTo(vg, x, y - length);
    nvgStrokeColor(vg, color);
    nvgStrokeWidth(vg, width);
    nvgStroke(vg);
    nvgClosePath(vg);
}

void Score::drawHLine(NVGcontext *vg, NVGcolor color, float x, float y, float length, float width) const {
    nvgBeginPath(vg);
    nvgMoveTo(vg, x, y);
    nvgLineTo(vg, x + length, y);
    nvgStrokeColor(vg, color);
    nvgStrokeWidth(vg, width);
    nvgStroke(vg);
    nvgClosePath(vg);
}

void Score::filledRect(NVGcontext *vg, NVGcolor color, float x, float y, float w, float h, float rounding) const {
    nvgFillColor(vg, color);
    nvgBeginPath(vg);
    nvgRoundedRect(vg, x, y, w, h, rounding);
    nvgFill(vg);
}

void Score::prepareFontMusic(const DrawArgs &args) const {
    std::string fontPath("res/");
    fontPath += "Bravura.otf";
    // Get font
    std::shared_ptr<Font> font = APP->window->loadFont(asset::plugin(pluginInstance, fontPath.c_str()));
    if (!font) {
        WARN("Score font didn't load\n");
        return;
    }
    nvgFontFaceId(args.vg, font->handle);
    nvgFontSize(args.vg, 54);
}

void Score::prepareFontText1(const DrawArgs &args) const {
    // std::string fontPath("res/fonts/Roboto-Light.ttf");
    std::string fontPath("res/fonts/Roboto-Regular.ttf");
    std::shared_ptr<Font> font = APP->window->loadFont(asset::plugin(pluginInstance, fontPath.c_str()));
    if (!font) {
        WARN("Score font for text didn't load\n");
        return;
    }
    nvgFontFaceId(args.vg, font->handle);
    nvgFontSize(args.vg, 7);
}

void Score::prepareFontText2(const DrawArgs &args) const {
    //   std::string fontPath("res/fonts/Roboto-Light.ttf");
    std::string fontPath("res/fonts/Roboto-Regular.ttf");
    std::shared_ptr<Font> font = APP->window->loadFont(asset::plugin(pluginInstance, fontPath.c_str()));
    if (!font) {
        WARN("Score font for text didn't load\n");
        return;
    }
    nvgFontFaceId(args.vg, font->handle);
    nvgFontSize(args.vg, 5);
}

#if 0  // old version
void Score::prepareFontText(const DrawArgs &args) const {
   nvgFontFaceId(args.vg, APP->window->uiFont->handle);
    nvgFontSize(args.vg, 7);
}
#endif
