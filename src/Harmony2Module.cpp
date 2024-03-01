
#include "plugin.hpp"
#ifdef _H2

#include "BufferingParent.h"
#include "Harmony2.h"
#include "NumberFormatter.h"
#include "PhasePatterns.h"
#include "PopupMenuParamWidget.h"
#include "SqLabel.h"
#include "SqLog.h"
#include "WidgetComposite.h"

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
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/blank-panel.svg")));
#if 1  // def _LAB
        addLabel(Vec(23, 6), "Harmony III", 20);
        // addLabel(Vec(28, 60), "Under Construction", 14);
        addLabel(Vec(30, 356), "Squinktronix", 16);
#endif
        //  addControls(module);
        //  addIO(module);
        //   addOutput(createOutput<PJ301MPort>(Vec(25, 200), module, Comp::OUT));
        addTranposeControls(module);
    }

private:
    void addTranposeControls(Harmony2Module* module) {
        const float y0= 50;
        const float deltaY = 30;
        for (int i = 0; i < 6; ++i) {
            const float y = y0 + i * deltaY;
            PopupMenuParamWidget* p = createParam<PopupMenuParamWidget>(
                Vec(8, y),
                module,
                Comp::XPOSE1_PARAM);
            p->setLabels(Comp::getTransposeLabels());  // just default to sharps, we will change. TODO: do it right.
            p->box.size.x = 40;                         // width
            p->box.size.y = 22;
           // p->text = "C";
            addParam(p);
        }
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
};

Model* modelHarmony2 = createModel<Harmony2Module, Harmony2Widget>("sqh-harmony2");
#endif