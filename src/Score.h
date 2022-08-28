#pragma once

#include <assert.h>
#include <stdio.h>

#include <codecvt>
#include <locale>
#include <string>

#include "Harmony1Module.h"
#include "plugin.hpp"

using FontPtr = std::shared_ptr<Font>;
class Harmony1Module;

// #define _TESTCHORD

class Dirty {
public:
    virtual bool isDirty() const = 0;
};

class BufferingParent : public Widget {
public:
    /**
     * @brief Construct a new Buffering Parent object
     *
     * @param childWidget is the widget that will do all the work
     * @param size the size at which to create/set
     * @param dd usually will be childWidget, and childWidget will implement Dirty
     */
    BufferingParent(Widget *childWidget, const Vec size, Dirty *dd) {
        this->box.size = size;
        childWidget->box.size = size;
        dbg = childWidget;
        fw = new FramebufferWidget();
        this->addChild(fw);
        fw->addChild(childWidget);
        dirtyDetector = dd;
    }

    void step() override {
        Widget::step();
        if (dirtyDetector && dirtyDetector->isDirty()) {
            fw->dirty = true;
            // SQINFO("set dirty true 46");
        }
    }

private:
    Widget *dbg = nullptr;
    FramebufferWidget *fw = nullptr;
    Dirty *dirtyDetector = nullptr;
};
//     return APP->engine->getParamValue(module, paramId) > .5;

class Score : public app::LightWidget, public Dirty {
public:
    Score(Harmony1Module *);
    void step() override;
    void draw(const DrawArgs &args) override;
    //   void drawLayer(const DrawArgs &args, int layer) override;
    bool isDirty() const override {
        return scoreIsDirty;
    }

    void setWhiteOnBlack(bool b) {
        if (whiteOnBlack != b) {
            whiteOnBlack = b;
            // INFO("set white on black %d", whiteOnBlack);
            scoreIsDirty = true;
        }
    }

private:
    bool scoreIsDirty = false;
    void filledRect(NVGcontext *vg, NVGcolor color, float x, float y, float w, float h, float rounding) const;
    void drawHLine(NVGcontext *vg, NVGcolor color, float x, float y, float length, float width) const;
    void drawVLine(NVGcontext *vg, NVGcolor color, float x, float y, float length, float width) const;

    // FontPtr loadFont() const;
    void prepareFontMusic(const DrawArgs &args) const;
    void prepareFontText(const DrawArgs &args) const;

    /**
     * @brief figure out the x position to draw a note
     *
     * @param noteNumber is the index of the note - 0..7
     * @param keysigWidth is the absolute of the keysig drawing - 0 for cmaj
     * @return float absolute x position
     */
    float noteXPos(int noteNumber, float keysigWidth) const;

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
    bool whiteOnBlack = false;  // default to black notes for better look in module browser

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
    void drawChordInfo(const DrawArgs &args, float x, const Comp::Chord &chord) const;
    /**
     * @return float width of key signature
     */
    float drawMusicNonNotes(const DrawArgs &args) const;
    void drawChordNumbers(const DrawArgs &args, float widthOfKeysig) const;
    void drawNotes(const DrawArgs &args, float widthOfKsig) const;

    /**
     * @return float width of key signature
     */
    float drawKeysig(const DrawArgs &args, ConstScalePtr scale, bool trebleClef, float y) const;
    float ledgerLine2Pos(int ledgerLine, bool bassStaff) const;

    NVGcolor getForegroundColor() const;
    NVGcolor getBackgroundColor() const;

    // float insetForKeysig(float keysigWidth) const;

    // Y axis pos

    // topMargin = 36.5f;  is original
    const float topMargin = 27.5f;
    const float yTrebleStaff = topMargin + 0;
    const float yBassStaff = yTrebleStaff + 42;    // 28 way too close
    const float yTrebleClef = yTrebleStaff - 3.3;  // 3 a little low, 4 way high
    const float yBassClef = yBassStaff - 10;       // 11 too much
    const float yNoteInfo = yBassStaff + 12;       // 0 too high

    // X axis pos
    const float leftMargin = 5.5f;
    const float xStaff = leftMargin;
    const float xClef = xStaff + 2;
    const float xNote0 = xClef + 18;

    const float deltaXNote = 10;            // 8 seemed close
    const float spaceBetweenLines = 1.67f;  // 1.7 slightly high
                                            // 1.65 low
    const float barlineHeight = 55.5;       // 55 low
                                            // 57 went  high

    //  const float barlineX0 = leftMargin;
    const float barlineX1 = xClef + 61;  //  small 63 big
    // const float barlineX2 = barlineX1 + 43;     // 40 too small 46 too big
};

NVGcolor Score::getForegroundColor() const {
    return whiteOnBlack ? nvgRGB(0xff, 0xff, 0xff) : nvgRGB(0, 0, 0);
}

NVGcolor Score::getBackgroundColor() const {
    return whiteOnBlack ? nvgRGB(0, 0, 0) : nvgRGB(0xff, 0xff, 0xff);
}

inline Score::Score(Harmony1Module *m) : module(m) {
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
    if (!module) {
        Comp::Chord c;
        c.inversion = 0;
        c.root = 1;
        c.pitch[0] = 60;
        c.pitch[1] = 72;
        c.pitch[2] = 76;
        c.pitch[3] = 79;
        chords.push_back(c);

        c.inversion = 0;
        c.root = 3;
        c.pitch[0] = 64;
        c.pitch[1] = 71;
        c.pitch[2] = 76;
        c.pitch[3] = 79;
        chords.push_back(c);

        c.inversion = 1;
        c.root = 2;
        c.pitch[0] = 65;
        c.pitch[1] = 69;
        c.pitch[2] = 74;
        c.pitch[3] = 77;
        chords.push_back(c);

        c.inversion = 0;
        c.root = 7;
        c.pitch[0] = 59;
        c.pitch[1] = 71;
        c.pitch[2] = 74;
        c.pitch[3] = 77;
        chords.push_back(c);

        c.inversion = 1;
        c.root = 3;
        c.pitch[0] = 55;
        c.pitch[1] = 71;
        c.pitch[2] = 76;
        c.pitch[3] = 79;
        chords.push_back(c);

        c.inversion = 0;
        c.root = 4;
        c.pitch[0] = 53;
        c.pitch[1] = 72;
        c.pitch[2] = 77;
        c.pitch[3] = 81;
        chords.push_back(c);

        c.inversion = 0;
        c.root = 5;
        c.pitch[0] = 55;
        c.pitch[1] = 71;
        c.pitch[2] = 74;
        c.pitch[3] = 79;
        chords.push_back(c);

        c.inversion = 0;
        c.root = 1;
        c.pitch[0] = 60;
        c.pitch[1] = 67;
        c.pitch[2] = 72;
        c.pitch[3] = 76;
        chords.push_back(c);
    }

#endif
}

inline void Score::step() {
#ifndef _TESTCHORD
    if (module) {
        if (module->isChordAvailable()) {
            scoreIsDirty = true;
            auto ch = module->getChord();
            chords.push_back(ch);
            if (chords.size() > 8) {
                while (chords.size() > 1) {
                    chords.pop_front();
                }
                // SQINFO("pop chord now %d", chords.size());
            }
        }
    }
#endif
    Widget::step();
}

#if 0
inline float Score::insetForKeysig(float keysigWidth) const {
    if (keysigWidth < 2) {
        return 0;               // if no extra space, don't require any
    }
    return std::max(keysigWidth - 6, 2.f);
}
#endif

inline float Score::noteXPos(int noteNumber, float keysigWidth) const {
    // Squeeze notes together a bit to accomodate keysig.
    const float inset = (keysigWidth >= 2) ? std::max(keysigWidth - 4, 2.f) : 0;

    // 30 was too much squish. 50 a bit much
    const float delta = deltaXNote * (1.f - (inset / 70.f));

    // SQINFO("ksw=%f ins=%f delta=%f delx=%f", keysigWidth, inset, delta, deltaXNote);

    float x = xNote0 + inset + noteNumber * delta;
    if (noteNumber > 3) {
        // little bump into the next bar
        x += delta;
    }
    return x;
}

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

#if 0
    printf("notYInfo p=%d, bass=%d ll=%d lp0=%f 1=%f 2=%f\n",
        note.get(),
        bassStaff,
        ledgerLine,
        ret.ledgerPos[0],
        ret.ledgerPos[1],
        ret.ledgerPos[2]
        );
#endif

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
    // INFO("Score::draw clip=szx %f, szy %f, %f, %f", args.clipBox.size.x, args.clipBox.size.y, args.clipBox.pos.x, args.clipBox.pos.y);
    INFO("----- in draw %p, chords=%d", this, int(chords.size()));
    nvgScissor(args.vg, RECT_ARGS(args.clipBox));
    const float width = drawMusicNonNotes(args);
    drawNotes(args, width);
    drawChordNumbers(args, width);
    // SQINFO("clear dirty at end of draw was %d", scoreIsDirty);
    scoreIsDirty = false;

    Widget::draw(args);
}

inline void Score::drawChordNumbers(const DrawArgs &args, float widthOfKeysig) const {
    if (chords.empty()) {
        return;
    }
    prepareFontText(args);
    int i = 0;
    for (auto chord : chords) {
        // SQINFO("draw chord number %d size=%d", i, (int)chords.size());
        const float x = noteXPos(i, widthOfKeysig) + 1.5;
        drawChordInfo(args, x, chord);
        ++i;
    }
}

inline void Score::drawNotes(const DrawArgs &args, float keysigWidth) const {
    if (!chords.empty()) {
        // INFO("chords to score %d", int(chords.size()));
        int i = 0;
        for (auto chord : chords) {
            // note - we could add inset ourself in noteXPos.
            const float x = noteXPos(i, keysigWidth);

            // SQINFO("x = %f", noteXPos(i, keysigWidth));

            for (int i = 0; i < 4; ++i) {
                const bool stemUp = i % 2;
                auto yInfo = noteYInfo(chord.pitch[i], i < 2);

                for (int i = 0; i < 3; ++i) {
                    if (yInfo.ledgerPos[i] != 0) {
                        // printf("drawing ledger at %f\n", yInfo.ledgerPos[i]);
                        nvgText(args.vg, x, yInfo.ledgerPos[i], ledgerLine.c_str(), NULL);
                    }
                }
                const char *note = stemUp ? noteQuarterUp.c_str() : noteQuarterDown.c_str();
                // INFO("drawing note at %f, %f\n", x, yInfo.position);
                nvgText(args.vg, x, yInfo.position, note, NULL);
            }
            ++i;
        }
    }
}

inline float Score::drawKeysig(const DrawArgs &args, ConstScalePtr scale, bool treble, float y) const {
    const auto info = scale->getScoreInfo();
    float width = 0;
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
        for (int i = 0; i < num; ++i) {
            const float x = xClef + 11 + w;
            const int note = accidentals[i].get();
            const float yf = noteY(note, !treble);
            nvgText(args.vg, x, yf, character, NULL);
            w += 4;
        }
        width = std::max(width, w);
    }
    return width;
}

inline float Score::drawMusicNonNotes(const DrawArgs &args) const {
    NVGcolor color = getBackgroundColor();

    // TODO : USE SIZE
    // filledRect(args.vg, color, 0, 0, 200, 200, 5);
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
    if (module) {
        auto scale = module->getScale();
        const float a = drawKeysig(args, scale, true, yTrebleStaff);
        const float b = drawKeysig(args, scale, false, yBassStaff);
        keysigWidth = std::max(keysigWidth, a);
        keysigWidth = std::max(keysigWidth, b);
    }

    drawBarLine(args, xStaff, yBassStaff);

    const float secondBarLineX = 3 + .5f * (noteXPos(3, keysigWidth) + noteXPos(4, keysigWidth));
    // INFO("x was %f is %f", barlineX1, secondBarLineX);
    drawBarLine(args, secondBarLineX, yBassStaff);

    const float barlineX2 = args.clipBox.size.x - leftMargin;
    drawBarLine(args, barlineX2, yBassStaff);

    // bgf: non const - didn't compile. do we want this here?
    //  TransparentWidget::draw(args);
    return keysigWidth;
}

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
#if 0
    std::stringstream s;
    s << chord.root;
    s << ":";
    s << chord.inversion;
    INFO("draw into at %f, %f, %s", x, yNoteInfo, s.str().c_str());
    nvgText(args.vg, x, yNoteInfo, s.str().c_str(), NULL);
#endif
}

inline void Score::drawStaff(const DrawArgs &args, float yBase) const {
    const float x = xStaff;
    const float length = args.clipBox.size.x - 2 * leftMargin;
    // auto color = nvgRGB(0xff, 0xff, 0xff);
    const auto color = getForegroundColor();
    for (int i = 0; i < 5; ++i) {
        float y = yBase - 2.f * float(i) * spaceBetweenLines;
        drawHLine(args.vg, color, x, y, length, .5f);
    }
}

inline void Score::drawBarLine(const DrawArgs &args, float x, float y) const {
    auto color = getForegroundColor();
    drawVLine(args.vg, color, x, y, barlineHeight, .5f);
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
    //   nvgRect(vg, x, y, w, h);
    nvgRoundedRect(vg, x, y, w, h, rounding);
    nvgFill(vg);
}

void Score::prepareFontMusic(const DrawArgs &args) const {
    std::string fontPath("res/");
    fontPath += "Bravura.otf";
    // Get font
    std::shared_ptr<Font> font = APP->window->loadFont(asset::plugin(pluginInstance, fontPath.c_str()));
    if (!font) {
        printf("font didn't load\n");
        return;
    }
    nvgFontFaceId(args.vg, font->handle);
    nvgFontSize(args.vg, 54);
}

void Score::prepareFontText(const DrawArgs &args) const {
    // int f = APP->window->uiFont->handle;
    nvgFontFaceId(args.vg, APP->window->uiFont->handle);
    nvgFontSize(args.vg, 7);  // TODO: use our own font?
}
