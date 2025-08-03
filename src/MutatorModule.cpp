
#include "plugin.hpp"  // must be first include (for now).
#ifdef _MUTATOR

#include <string>

#include "BufferingParent.h"
#include "MutatorModule.h"
#include "PopupMenuParamWidget.h"
#include "SqLabel.h"
#include "SqLog.h"

#define _LAB

struct RoundBigBlackSnapKnob : RoundBigBlackKnob {
    RoundBigBlackSnapKnob() {
        snap = true;
        smooth = false;
    }
};

/**
 */
class MutatorWidget : public ModuleWidget {
public:
    MutatorWidget(class MutatorModule* module) {
        setModule(module);
        _module = module;

        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/visualizer.svg")));

#if 1  // def _LAB
        addLabel(Vec(40, 6), "Mutator", 20);

        // 35 x was too far left
        addLabel(Vec(38, 356), "Squinktronix", 17);
#endif
        const float yJax = 320;
        const float dx = 30;
        const float d0 = 10;
        addInputL(Vec(d0, yJax), Comp::MUTATE_INPUT, "Mut", 1);
        addInputL(Vec(d0 + dx, yJax), Comp::CENTER_VOLTAGE_INPUT, "Ctr", 2);
        addOutputL(Vec(d0 + 2 * dx, yJax), Comp::NOTES_OUTPUT, "Notes", 2);
        addInputL(Vec(d0 + 3 * dx, yJax), Comp::INITIAL_VOLTAGE_INPUT, "Init", 6);

        addKeysig(module);

        addParamL<RoundBigBlackSnapKnob>(Vec(40, 50), module, Comp::ROW_LENGTH_PARAM, "Steps", 14);

        addStyle(module);
        addStyle2(module);
        addDebug(module);
    }

private:
    MutatorModule* _module = nullptr;
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

 
    void addStyle(Module* module) {
        const float styleRow1 = 160;
        const float styleRow2 = 210;
        const float x0 = 10;
        const float x1 = 50;
        const float x2 = 90;

        addParamL<RoundBlackKnob>(Vec(x0, styleRow1), module, Comp::NON_CENTERED_WEIGHT_STYLE_PARAM, "center", 8);
        addParamL<RoundBlackKnob>(Vec(x1, styleRow1), module, Comp::PITCH_RANGE_WEIGHT_STYLE_PARAM, "prng w", 8);
        addParamL<RoundBlackKnob>(Vec(x1, styleRow2), module, Comp::PITCH_RANGE_STYLE_PARAM, "p rng", 8);
        addParamL<RoundBlackKnob>(Vec(x2, styleRow1), module, Comp::LEAPS_WEIGHT_STYLE_PARAM, "leaps", 8);
        addParamL<RoundBlackKnob>(Vec(x0, styleRow2), module, Comp::UNISON_WEIGHT_STYLE_PARAM, "unsn", 6);
    }

    //  void addParam(ParamWidget* param);
    template <typename T>
    ParamWidget* addParamL(const Vec& vec, Module* module, int paramNum, const std::string& text, float label_dx = 0) {
#ifdef _LAB
        Vec vlabel(vec.x, vec.y);
        vlabel.y -= 20;
        vlabel.x += 4;
        vlabel.x += label_dx;
        const float xOffset = -2 + text.size() * 2.5;  // crude attempt to center text.
        vlabel.x -= xOffset;
        addLabel(vlabel, text);
#endif
        const auto foo = createParam<T>(vec, module, paramNum);
        addParam(foo);
        return foo;
    }

    void addOutputL(const Vec& vec, int outputNumber, const std::string& text, float label_dx = 0) {
        addOutput(createOutput<PJ301MPort>(vec, module, outputNumber));
#ifdef _LAB
        Vec vlabel(vec.x, vec.y);
        vlabel.y -= 20;
        vlabel.x += 4;
        vlabel.x += label_dx;
        const float xOffset = -2 + text.size() * 2.5;  // crude attempt to center text.
        vlabel.x -= xOffset;
        addLabel(vlabel, text);
#endif
    }

    void addInputL(const Vec& vec, int outputNumber, const std::string& text, float label_dx = 0) {
        addInput(createInput<PJ301MPort>(vec, module, outputNumber));
#ifdef _LAB
        Vec vlabel(vec.x, vec.y);
        vlabel.y -= 20;
        vlabel.x += 4;
        vlabel.x += label_dx;
        const float xOffset = -2 + text.size() * 2.5;  // crude attempt to center text.
        vlabel.x -= xOffset;
        addLabel(vlabel, text);
#endif
    }

    const float x0 = 11;
    const float x1 = 54.5;  // 50 too far left
    const float x2 = 100;
    const float xPes = x2;

    void addKeysig(MutatorModule* xmodule) {
        const float yScale = 270;
        const float yMode = yScale;

        PopupMenuParamWidget* p = createParam<PopupMenuParamWidget>(
            Vec(8, yScale),
            module,
            Comp::KEY_PARAM);
        p->setLabels(Scale::getRootLabels(false));
        p->box.size.x = 40;  // width
        p->box.size.y = 22;
        p->text = "C";
        addParam(p);
        //     _keyRootWidget = p;  // remember this so we can poll it.

        p = createParam<PopupMenuParamWidget>(
            Vec(60, yMode),
            module,
            Comp::MODE_PARAM);
        //   const bool diatonicOnly = xmodule ? xmodule->getComp()->diatonicOnly() : false;
        // Let user select whatever whey want
        p->setShortLabels(Scale::getShortScaleLabels(false));
        p->setLabels(Scale::getScaleLabels(false));
        p->box.size.x = 70;  // width
        p->box.size.y = 22;
        p->text = "Maj";
        addParam(p);
    }

    void addStyle2(Module* module) {
        const float y = 100;
        PopupMenuParamWidget* p = createParam<PopupMenuParamWidget>(
            Vec(8, y),
            module,
            Comp::SLOTS_TO_CHANGE_PARAM);
        p->setLabels({"all", "1", "2", "3", "4"});
        p->box.size.x = 40;  // width
        p->box.size.y = 22;
        p->text = "1";
        addParam(p);

        addParam(createParam<CKSSThree>(Vec(80, y), module, Comp::ADJACENCY_STYLE_PARAM));
    }

    void addDebug(Module* module) {
        const float y = 360;        // 350 too high
      //  void addInputL(const Vec& vec, int outputNumber, const std::string& text, float label_dx = 0) {
      addInputL( Vec(0, y), Comp::DEBUG_REINIT_INPUT, "re ini", 9);
      addInputL( Vec(110, y), Comp::DEBUG_EVAL_INPUT, "eval", 3);
    }
};

Model* modelMutator = createModel<MutatorModule, MutatorWidget>("sqh-mutator");
#endif