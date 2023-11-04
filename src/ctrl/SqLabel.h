
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

    // implement dirty
    virtual bool isDirty() const override {
        if (_isDirty) {
            SQINFO("isDirty ret %d this=%p", _isDirty, this);
        }
        // return false;
        return _isDirty;
    };

	Widget * _hackParent = nullptr;

private:
    bool _isDirty = true;
};

inline void SqLabel::draw(const DrawArgs& args) {
    //	float x = 0;		// text alignment stuff.

    SQINFO("sqlabx draw this=%p, text=%s", this, this->text.c_str());
 //   INFO("----- draw %p, chords=%d", this, int(chords.size()));
    INFO("----- pos=%f, %f size=%f, %f", box.pos.x, box.pos.y, box.size.x, box.size.y);
	if (_hackParent) {
    	INFO("----- hack p: pos=%f, %f size=%f, %f", _hackParent->box.pos.x,  _hackParent->box.pos.y,_hackParent->box.size.x,  _hackParent->box.size.y);
	}

	
	std::shared_ptr<Font> font = APP->window->loadFont(fontPath);
    if (!font) {
        SQWARN("no font");
        return;
    }
    nvgFontFaceId(args.vg, font->handle);

    nvgTextLineHeight(args.vg, lineHeight);

    // nvgDrawText();
    // NVGcolor colorActual = (color.a > 0.f) ? color : bndGetTheme()->regularTheme.textColor;
    // bndIconLabelValue(args.vg, x, 0.0, box.size.x, box.size.y, -1, colorActual, BND_LEFT, fontSize, text.c_str(), NULL);

    nvgFillColor(args.vg, color);  // is this needed? background or font?
                                   //	nvgText(args.vg, box.pos.x, box.pos.y, text.c_str(), NULL);
    nvgText(args.vg, 8, 16, text.c_str(), NULL);
    // SQINFO("draw, lineh=%f, x=%f, y=%f, text=>%s<", lineHeight, box.pos.x, box.pos.y, text.c_str());
    // SQINFO("size = %f, %f", box.size.x, box.size.y);
    _isDirty = false;
    SQINFO("leave draw, dirty = %d", _isDirty);
}

/*
  std::string fontPath("res/");
    fontPath += "Bravura.otf";
    // Get font
    std::shared_ptr<Font> font = APP->window->loadFont(asset::plugin(pluginInstance, fontPath.c_str()));

 */

/*
void draw(const DrawArgs& args) override {
                // Background
                nvgBeginPath(args.vg);
                nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 2);
                nvgFillColor(args.vg, nvgRGB(0x19, 0x19, 0x19));
                nvgFill(args.vg);

                prepareFont(args);

                // Background text
                nvgFillColor(args.vg, bgColor);
                nvgText(args.vg, textPos.x, textPos.y, bgText.c_str(), NULL);
        }
        */

template <class T>
inline void BufferingParent<T>::_setHackParent(Widget *childWidget) {
    INFO("_setHackParent");
    SqLabel* pscore = dynamic_cast<SqLabel*>(childWidget);
    if (pscore) {
        INFO("cast ok");
        pscore->_hackParent = this;
    }
}
