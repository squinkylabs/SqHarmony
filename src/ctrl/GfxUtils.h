
#pragma once

#include "SqLog.h"

class GfxUtils {
public:
    static void filledRect(NVGcontext *vg, NVGcolor color, float x, float y, float w, float h) {
        nvgFillColor(vg, color);
        nvgBeginPath(vg);
        nvgRect(vg, x, y, w, h);
        nvgFill(vg);
    }

    // void nvgRoundedRect(NVGcontext* ctx, float x, float y, float w, float h, float r);
    static void filledRoundedRect(NVGcontext *vg, NVGcolor color, float x, float y, float w, float h, float r) {
      //  SQINFO("fill x=%f y=%f w=%f h=%f", x, y, w, h);
       // filledRect(vg, color, x, y, w, h);
        nvgFillColor(vg, color);
        nvgBeginPath(vg);
        nvgRoundedRect(vg, x, y, w, h, r);
        nvgFill(vg);
    }
};


class RoundedRect : public Widget {
public:
    RoundedRect(const Vec &position, const Vec &size) {
        math::Rect xbox;
        xbox.pos = position;
        xbox.size = size;
        Widget::setBox(xbox);
    }
    void draw(const DrawArgs &args) override {
        NVGcolor black = nvgRGB(0, 0, 0);
        GfxUtils::filledRoundedRect(args.vg, black, 
            0,
            0,
            box.size.x,
            box.size.y,
            7.f);
    }

private:
};
