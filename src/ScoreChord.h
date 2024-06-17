#pragma once

//class VisualizerModule;

#include "VisualizerModule.h"

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

private:
    bool _scoreIsDirty = false;
    static int _refCount;
    bool _whiteOnBlack = false;

     VisualizerModule *const _module = nullptr;

     const std::string noteQuarterUp = u8"\ue1d5";
    const std::string noteQuarterDown = u8"\ue1d6";
    const std::string staffFiveLines = u8"\ue014";
    const std::string gClef = u8"\ue050";
    const std::string fClef = u8"\ue062";
    const std::string ledgerLine = u8"\ue022";
    const std::string flat = u8"\ue260";
    const std::string sharp = u8"\ue262";

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


    std::pair<float, float> drawMusicNonNotes(const DrawArgs &args) const;
    // void drawChordNumbers(const DrawArgs &args, std::pair<float, float> keysigLayout) const;
    void drawNotes(const DrawArgs &args, std::pair<float, float> keysigLayout) const;

    void drawStaff(const DrawArgs &args, float y) const;
    void drawBarLine(const DrawArgs &args, float x, float y) const;
   // float drawChordRoot(const DrawArgs &args, float x, const Comp::Chord &chord) const;
   // void drawChordInversion(const DrawArgs &args, float x, const Comp::Chord &chord) const;

   float Score::noteXPos(int noteNumber, std::pair<float, float> _keysigLayout) const;

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

void ScoreChord::filledRect(NVGcontext *vg, NVGcolor color, float x, float y, float w, float h, float rounding) const {
    w = std::min(w, 80.f);    //  clip with to 80, temp
    SQINFO("rect width=%f h=%f", w, h);
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
    nvgFontSize(args.vg, 54);
}

inline void ScoreChord::draw(const DrawArgs &args) {
    nvgScissor(args.vg, RECT_ARGS(args.clipBox));
    const auto keysigLayout = drawMusicNonNotes(args);
    drawNotes(args, keysigLayout);
    //  drawChordNumbers(args, keysigLayout);
    _scoreIsDirty = true;

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
    nvgText(args.vg, xClef, yTrebleClef, gClef.c_str(), NULL);

    drawStaff(args, yBassStaff);
    nvgText(args.vg, xClef, yBassClef, fClef.c_str(), NULL);

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

  // const float secondBarLineX = 3 + .5f * (noteXPos(3, keysigWidth) + noteXPos(4, keysigWidth));
    const float secondBarLineX = 2 + .5f * (noteXPos(3, ksStuff) + noteXPos(4, ksStuff));
    drawBarLine(args, secondBarLineX, yBassStaff);

    const float barlineX2 = args.clipBox.size.x - leftMargin;
    drawBarLine(args, barlineX2, yBassStaff);

    return std::make_pair(keysigWidth, keysigEnd);;
}

inline void ScoreChord::drawNotes(const DrawArgs &args, std::pair<float, float> keysigLayout) const {
}