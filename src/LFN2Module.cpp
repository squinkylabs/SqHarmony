

#include "plugin.hpp"
#include "BufferingParent.h"
#include "NumberFormatter.h"
#include "PhasePatterns.h"
#include "PopupMenuParamWidget.h"
#include "SqLabel.h"
#include "SqLog.h"
#include "WidgetComposite.h"
#include "plugin.hpp"

class LFN2Module : public rack::engine::Module {
public:
     void process(const ProcessArgs& args) override {
       // comp->process(args);
       SQINFO("xxx");
    }
};

class LFN2Widget : public ModuleWidget {
public:
    LFN2Widget(LFN2Module* module){
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/phase-patterns.svg")));
#if 1 // def _LAB
        addLabel(Vec(35, 6), "LFN II", 20);
        // addLabel(Vec(28, 60), "Under Construction", 14);
        addLabel(Vec(30, 356), "Squinktronix", 16);
#endif
      //  addControls(module);
      //  addIO(module);
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