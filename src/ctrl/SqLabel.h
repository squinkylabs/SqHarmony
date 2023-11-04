
#pragma once

#include <algorithm>
#include <string>

// or should we just include rack.hpp?
#include <nanovg.h>

#include "BufferingParent.h"

/*
struct Label : widget::Widget {
        enum Alignment {
                LEFT_ALIGNMENT,
                CENTER_ALIGNMENT,
                RIGHT_ALIGNMENT,
        };

        std::string text;
        float fontSize;
        float lineHeight;
        NVGcolor color;
        Alignment alignment = LEFT_ALIGNMENT;

        Label();
        void draw(const DrawArgs& args) override;
};
*/

class SqLabel : public widget::Widget, public Dirty {
public:
    ~SqLabel() {
        SQINFO("dtor of label %p", this);
    }
    std::string text;
    float fontSize = 12;
    float lineHeight = 12;
    NVGcolor color = color::BLACK_TRANSPARENT;
    std::string fontPath = asset::plugin(pluginInstance, "res/fonts/Roboto-Regular.ttf");

    void draw(const DrawArgs& args) override;
    SqLabel() {
        box.size.y = BND_WIDGET_HEIGHT;
        box.size.x = 40;  // hax
        SQINFO("SqLabel ctor %p", this);
    }

    // updates the text and forces draw
    void updateText(const std::string& s);

    // implement dirty
    virtual bool isDirty() const override {
        if (_isDirty) {
            SQINFO("isDirty ret %d this=%p", _isDirty, this);
        }
        // return false;
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

    nvgTextLineHeight(args.vg, lineHeight);
    nvgFillColor(args.vg, color);  // is this needed? background or font?
                                   //	nvgText(args.vg, box.pos.x, box.pos.y, text.c_str(), NULL);
    nvgText(args.vg, 8, 16, text.c_str(), NULL);;
    _isDirty = false;
}

