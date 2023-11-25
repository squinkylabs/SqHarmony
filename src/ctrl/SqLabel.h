
#pragma once

#include <algorithm>
#include <string>

// or should we just include rack.hpp?
#include <nanovg.h>

#include "BufferingParent.h"

class SqLabel : public widget::Widget, public Dirty {
public:
    std::string text;
    float fontSize = 12;
    float lineHeight = 12;
    NVGcolor color = color::BLACK_TRANSPARENT;
    std::string fontPath = asset::plugin(pluginInstance, "res/fonts/Roboto-Regular.ttf");

    void draw(const DrawArgs& args) override;

    // updates the text and forces draw
    void updateText(const std::string& s);

    // Implement dirty.
    virtual bool isDirty() const override {
        return _isDirty;
    };

private:
    bool _isDirty = true;
};

inline void SqLabel::updateText(const std::string& s) {
    text = s;
    _isDirty = true;
}

inline void SqLabel::draw(const DrawArgs& args) {
    std::shared_ptr<Font> font = APP->window->loadFont(fontPath);
    if (!font) {
        SQWARN("no font");
        return;
    }
    nvgFontFaceId(args.vg, font->handle);
    nvgFontSize(args.vg, fontSize);

    nvgTextLineHeight(args.vg, lineHeight);
    nvgFillColor(args.vg, color);  // is this needed? background or font?
                                   //	nvgText(args.vg, box.pos.x, box.pos.y, text.c_str(), NULL);
    // TODO: what are these arbitrary numbers?
    nvgText(args.vg, 8, 16, text.c_str(), NULL);
    ;
    _isDirty = false;
}
