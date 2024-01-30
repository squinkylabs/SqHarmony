
#include "plugin.hpp" // must be first include (for now).
#include "BufferingParent.h"
#include "NumberFormatter.h"
#include "PhasePatterns.h"
#include "PopupMenuParamWidget.h"
#include "SqLabel.h"
#include "SqLog.h"
#include "WidgetComposite.h"

#include "LFN2.h"

using Comp = LFN2<WidgetComposite>;

class LFN2Module : public rack::engine::Module {
public:
    std::shared_ptr<Comp> comp = std::make_shared<Comp>(this);
    LFN2Module() {
        config(Comp::NUM_PARAMS, Comp::NUM_INPUTS, Comp::NUM_OUTPUTS, Comp::NUM_LIGHTS);
        // addParams();
    }
    void process(const ProcessArgs& args) override {
        // comp->process(args);
       //SQINFO("xxx");
        const int x = gen32();
        float f = float(x) /  float(std::numeric_limits<int>::max());
        outputs[Comp::OUT].setVoltage(10 * f);
    }

private:
    std::mt19937 gen32;
};

class LFN2Widget : public ModuleWidget {
public:
    LFN2Widget(LFN2Module* module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/phase-patterns.svg")));
#if 1  // def _LAB
        addLabel(Vec(35, 6), "LFN II", 20);
        // addLabel(Vec(28, 60), "Under Construction", 14);
        addLabel(Vec(30, 356), "Squinktronix", 16);
#endif
        //  addControls(module);
        //  addIO(module);
        addOutput(createOutput<PJ301MPort>(Vec(85, 200), module, Comp::OUT));
        //addLabel(Vec(79, 1), "CkOut");
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
        // with 200, 20 it disappears at zoom > 250
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

Model* modelLFN2 = createModel<LFN2Module, LFN2Widget>("sqh-lfn2");