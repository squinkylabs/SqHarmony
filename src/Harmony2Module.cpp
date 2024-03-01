
#include "plugin.hpp"
#ifdef _H2

#include "BufferingParent.h"
#include "Harmony2.h"
#include "NumberFormatter.h"
#include "PhasePatterns.h"
#include "PopupMenuParamWidget.h"
#include "Scale.h"
#include "SqLabel.h"
#include "SqLog.h"
#include "WidgetComposite.h"

#define _LAB

using Comp = Harmony2<WidgetComposite>;

class Harmony2Module : public rack::engine::Module {
public:
    std::shared_ptr<Comp> comp = std::make_shared<Comp>(this);
    Harmony2Module() {
        config(Comp::NUM_PARAMS, Comp::NUM_INPUTS, Comp::NUM_OUTPUTS, Comp::NUM_LIGHTS);
        // addParams();
    }
};


class Harmony2Widget : public ModuleWidget {
public:
    Harmony2Widget(Harmony2Module* module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/harmony2-panel.svg")));
#if 1  // def _LAB
        addLabel(Vec(43, 6), "Harmony II", 20);
        // addLabel(Vec(28, 60), "Under Construction", 14);
        addLabel(Vec(44, 353), "Squinktronix", 17);
#endif
        //  addControls(module);
        //  addIO(module);
        //   addOutput(createOutput<PJ301MPort>(Vec(25, 200), module, Comp::OUT));
        addTranposeControls(module);
        addKeysig();
        addMainCV();
        addModCV();
    }

private:
    void addMainCV() {
        const float y = 317;
        addInputL(Vec(19, y), Comp::PITCH_INPUT, "CVI"); 
      //   addInputL(const Vec& vec, int outputNumber, const std::string& text) {
        addOutputL(Vec(60, y), Comp::PITCH_OUTPUT, "CVO");
    }

    void addModCV() {
        const float y = 270;
        addInputL(Vec(19, y), Comp::XPOSE_INPUT, "XP"); 

        addInputL(Vec(60, y), Comp::KEY_INPUT, "Key"); 
        addInputL(Vec(100, y), Comp::MODE_INPUT, "Mode"); 
    }

    void addKeysig() {
        const float yScale = 220;
        const float yMode = yScale;

        PopupMenuParamWidget* p = createParam<PopupMenuParamWidget>(
            Vec(8, yScale),
            module,
            Comp::KEY_PARAM);
        p->setLabels(Scale::getRootLabels(false));  // just default to sharps, we will change. TODO: do it right.
        p->box.size.x = 40;                         // width
        p->box.size.y = 22;
        p->text = "C";
        addParam(p);
    // don't know yet if we need this...
     //   _keyRootWidget = p;  // remember this so we can poll it.

        p = createParam<PopupMenuParamWidget>(
            Vec(74, yMode),
            module,
            Comp::MODE_PARAM);
        p->setShortLabels(Scale::getShortScaleLabels(true));
        p->setLabels(Scale::getScaleLabels(true));

        p->box.size.x = 70;  // width
        p->box.size.y = 22;
        p->text = "Maj";
        addParam(p);
    }

    void addTranposeControls(Harmony2Module* module) {  
        for (int i = 0; i < 6; ++i) {
            addTransposeControls(i);
        }
    }

    static constexpr float y0 = 40;
    static constexpr float deltaY = 28;
    static constexpr float xbutton = 5;
    static constexpr float xoctave = 28;
    static constexpr float xdegree = 75;

    void addTransposeControls(int index) {
        const float y = y0 + index * deltaY;

                // RIB trigger button
        addParam(createLightParam<VCVLightButton<MediumSimpleLight<WhiteLight>>>(
            Vec(xbutton, y),
            module,
            Comp::XPOSE_ENABLE1_PARAM + index,
            Comp::XPOSE_ENABLE1_LIGHT + index));
        PopupMenuParamWidget* p = createParam<PopupMenuParamWidget>(
            Vec(xoctave, y),
            module,
            Comp::XPOSE_OCTAVE1_PARAM + index);
        p->setLabels(Comp::getTransposeOctaveLabels());  // just default to sharps, we will change. TODO: do it right.
        p->box.size.x = 40;                        // width
        p->box.size.y = 22;
        addParam(p);

         p = createParam<PopupMenuParamWidget>(
            Vec(xdegree, y),
            module,
            Comp::XPOSE_DEGREE1_PARAM + index);
        p->setLabels(Comp::getTransposeDegreeLabels());  // just default to sharps, we will change. TODO: do it right.
        p->box.size.x = 40;                        // width
        p->box.size.y = 22;
        addParam(p);
    }

    /**
     * @brief
     *
     * @param v is the position, panel relative
     * @param str text to display
     * @param fontSize
     * @return BufferingParent<SqLabel>*
     */
    BufferingParent<SqLabel>* addLabel(const Vec& v, const std::string& str, float fontSize = 14) {
        // TODO: what are these arbitrary numbers?
        // This "max size" is lame - do something better; was 200, 20
        // with 20, 2 is totally invisible.
        // with 200, 20 it disappears at zoom > 250clichéd
        // 400, 40 is no better
        // at 100, 10 it's truncated, but it still zooms up to about 240
        const Vec size(200, 20);
        SqLabel* lp = new SqLabel();
        BufferingParent<SqLabel>* parent = new BufferingParent<SqLabel>(lp, size, lp);

        NVGcolor white = nvgRGB(0xff, 0xff, 0xff);
        auto adjustedPos = v;

        adjustedPos.x -= 1.5f * str.size();
        parent->box.pos = adjustedPos;

        lp->text = str;
        lp->color = white;
        lp->fontSize = fontSize;

        addChild(parent);
        return parent;
    }

    void addOutputL(const Vec& vec, int outputNumber, const std::string& text) {
        addOutput(createOutput<PJ301MPort>(vec, module, outputNumber));
#ifdef _LAB
        Vec vlabel(vec.x, vec.y);
        vlabel.y -= 20;
        vlabel.x += 4;
        const float xOffset = -2 + text.size() * 2.5;  // crude attempt to center text.
        vlabel.x -= xOffset;
        addLabel(vlabel, text);
#endif
    }

    void addInputL(const Vec& vec, int outputNumber, const std::string& text) {
        addInput(createInput<PJ301MPort>(vec, module, outputNumber));
#ifdef _LAB
        Vec vlabel(vec.x, vec.y);
        vlabel.y -= 20;
         vlabel.x += 4;
        const float xOffset = -2 + text.size() * 2.5;  // crude attempt to center text.
        vlabel.x -= xOffset;
        addLabel(vlabel, text);
#endif
    }
};

Model* modelHarmony2 = createModel<Harmony2Module, Harmony2Widget>("sqh-harmony2");
#endif