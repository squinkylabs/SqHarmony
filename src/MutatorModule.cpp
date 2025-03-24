
#include "plugin.hpp"  // must be first include (for now).
#ifdef _MUTATOR

#include <string>

#include "BufferingParent.h"
#include "SqLabel.h"
#include "SqLog.h"

#include "MutatorModule.h"

#define _LAB

/**
 */
class MutatorWidget : public ModuleWidget {
public:
    MutatorWidget(class MutatorModule* module) {
        setModule(module);
        _module = module;

        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/visualizer.svg")));

#if 1  // def _LAB
        addLabel(Vec(38, 6), "Mutator", 20);
        addLabel(Vec(35, 356), "Squinktronix", 17);
#endif
        addInputL(Vec(40, 40), Comp::MUTATE_INPUT, "Mut");
        addOutputL(Vec(40, 150), Comp::NOTES_OUTPUT, "Notes");
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
    const float x1 = 54.5;            // 50 too far left
    const float x2 = 100;
    const float xPes = x2;
 };

Model* modelMutator = createModel<MutatorModule, MutatorWidget>("sqh-mutator");
#endif