
#include "plugin.hpp"  // must be first include (for now).
#ifdef _MUTATOR

#include <string>

#include "BufferingParent.h"
#include "MutatorModule.h"
#include "PopupMenuParamWidget.h"
#include "SqLabel.h"
#include "SqLog.h"

#define _LAB

const float xCol2 = 80;
const float xCol1 = 12;

/**
 */
class MutatorWidget : public ModuleWidget {
public:
    MutatorWidget(class MutatorModule* module) {
        setModule(module);
        _module = module;

        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/mutator.svg")));

#if 1  // def _LAB
        addLabel(Vec(40 + 22, 6), "Mutator", 20);
        addLabel(Vec(38 + 22, 356), "Squinktronix", 17);
#endif
        const float yJax = 320;
        const float dx = 32;
        const float d0 = 10;
        addInputL(Vec(d0, yJax), Comp::MUTATE_INPUT, "Mut");
        addInputL(Vec(d0 + 1 * dx, yJax), Comp::REINIT_INPUT, "RST", 0);
        addInputL(Vec(d0 + 2 * dx, yJax), Comp::CENTER_VOLTAGE_INPUT, "Ctr", 2);
        addInputL(Vec(d0 + 3 * dx, yJax), Comp::INITIAL_VOLTAGE_INPUT, "Init", 6);
        addOutputL(Vec(d0 + 4 * dx + 10, yJax), Comp::NOTES_OUTPUT, "Notes", 2);

        addKeysig(module);

        const float yLen = 40;
        const auto p = createParam<RoundBlackSnapKnob>(Vec(120, yLen), module, Comp::ROW_LENGTH_PARAM);
        addParam(p);
        Vec vlabel(60, yLen);
        addLabel(vlabel, "Num notes:");

        addStyle(module);
        addStyle2(module);
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
        const float styleRow1 = 210;
        const float styleRow2 = styleRow1 + 50;

        const float dx = 40;
        const float x0 = 10;
        const float x1 = x0 + dx + 10;
        const float x2 = x1 + dx;
        const float x3 = x2 + dx;

        addParamL<RoundBlackKnob>(Vec(x0, styleRow1), module, Comp::DESIRED_CENTER_PARAM, "center", 8);
        addParamL<RoundBlackKnob>(Vec(x1, styleRow1), module, Comp::NON_CENTERED_WEIGHT_STYLE_PARAM, "ctr w", 8);
        addParamL<RoundBlackKnob>(Vec(x2, styleRow1), module, Comp::LEAPS_WEIGHT_STYLE_PARAM, "leaps", 8);
        addParamL<RoundBlackKnob>(Vec(x3, styleRow1), module, Comp::CONSONANT_WEIGHT_PARAM, "cnsnnt", 8);

        addParamL<RoundBlackKnob>(Vec(x0, styleRow2), module, Comp::PITCH_RANGE_STYLE_PARAM, "range", 8);
        addParamL<RoundBlackKnob>(Vec(x1, styleRow2), module, Comp::PITCH_RANGE_WEIGHT_STYLE_PARAM, "rng w", 8);
        addParamL<RoundBlackKnob>(Vec(x2, styleRow2), module, Comp::UNISON_WEIGHT_STYLE_PARAM, "unsn", 6);
    }

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

    void addKeysig(MutatorModule* xmodule) {
        const float yScale = 80;
        const float yMode = yScale;

        PopupMenuParamWidget* p = createParam<PopupMenuParamWidget>(
            Vec(xCol1, yScale),
            module,
            Comp::KEY_PARAM);
        p->setLabels(Scale::getRootLabels(false));
        p->box.size.x = 40;  // width
        p->box.size.y = 22;
        p->text = "C";
        addParam(p);

        p = createParam<PopupMenuParamWidget>(
            Vec(xCol2, yMode),
            module,
            Comp::MODE_PARAM);
        p->setShortLabels(Scale::getShortScaleLabels(false));
        p->setLabels(Scale::getScaleLabels(false));
        p->box.size.x = 70;  // width
        p->box.size.y = 22;
        p->text = "Maj";
        addParam(p);
    }

    void addStyle2(Module* module) {
        const float y = 140;
        PopupMenuParamWidget* p = createParam<PopupMenuParamWidget>(
            Vec(xCol1, y),
            module,
            Comp::SLOTS_TO_CHANGE_PARAM);
        p->setLabels({"all", "1", "2", "3", "4"});
        p->box.size.x = 40;  // width
        p->box.size.y = 22;
        p->text = "1";
        addParam(p);

        const float labelDY = -23;
        Vec vlabel(xCol1 + 4, y + labelDY);
        addLabel(vlabel, "# mutate");

         Vec vlabel2(xCol2 + 14,  y + labelDY);
        addLabel(vlabel2, "Selection mode");

        p = createParam<PopupMenuParamWidget>(
            Vec(xCol2, y),
            module,
            Comp::ADJACENCY_STYLE_PARAM);
        p->setShortLabels({"rr, adj",
                           "rr, dst",
                           "rnd, adj",
                           "rnd, dst",
                           "rnd, rnd"});

        p->setLabels({"round robin, adjacent",
                      "round robin, distributed",
                      "random, adjacent",
                      "random, distributed",
                      "random, random"});
        p->box.size.x = 70;
        p->box.size.y = 22;
        p->text = "Maj";
        addParam(p);
    }
};

Model* modelMutator = createModel<MutatorModule, MutatorWidget>("sqh-mutator");
#endif