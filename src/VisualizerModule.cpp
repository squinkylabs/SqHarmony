
#include "plugin.hpp"  // must be first include (for now).
#ifdef _VISUALIZER

#include <string>

#include "BufferingParent.h"
#include "NumberFormatter.h"
#include "PhasePatterns.h"
#include "PopupMenuParamWidget.h"
#include "ScoreChord.h"
#include "SqLabel.h"
#include "SqLog.h"
#include "Visualizer.h"
#include "WidgetComposite.h"

using Comp = Visualizer<WidgetComposite>;

template <>
int BufferingParent<ScoreChord>::_refCount = 0;

class VisualizerModule : public rack::engine::Module {
public:
    std::shared_ptr<Comp> comp = std::make_shared<Comp>(this);
    VisualizerModule() {
        config(Comp::NUM_PARAMS, Comp::NUM_INPUTS, Comp::NUM_OUTPUTS, Comp::NUM_LIGHTS);
        // addParams();
    }
    void process(const ProcessArgs& args) override {
        comp->process(args);
    }
};

class VisualizerWidget : public ModuleWidget {
public:
    VisualizerWidget(VisualizerModule* module) {
        setModule(module);
        addScore(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/phase-patterns.svg")));
#if 1  // def _LAB
        addLabel(Vec(20, 6), "Visualizer", 20);
        // addLabel(Vec(28, 60), "Under Construction", 14);
        addLabel(Vec(25, 356), "Squinktronix", 16);
#endif
        addInput(createInput<PJ301MPort>(Vec(42, 300), module, Comp::CV_INPUT));

        _displayString = addLabel(Vec(10, 100), "chord");
        _displayString2 = addLabel(Vec(10, 120), "chord");
    }

    void step() override {
        _displayString->step();
        _displayString2->step();
        const float changeParam = APP->engine->getParamValue(module, Comp::CHANGE_PARAM);
        if (changeParam == _changeParam) {
            return;
        }

        _changeParam = changeParam;
        const ChordRecognizer::Type type = ChordRecognizer::Type(APP->engine->getParamValue(module, Comp::TYPE_PARAM));
        const ChordRecognizer::Inversion inversion = ChordRecognizer::Inversion(APP->engine->getParamValue(module, Comp::INVERSION_PARAM));
        const int root = APP->engine->getParamValue(module, Comp::ROOT_PARAM);
        ChordRecognizer::ChordInfo info = std::make_tuple(type, inversion, root);
        const auto v = ChordRecognizer::toString(info);
        _displayString->getChild()->text = v[0];
        _displayString->setDirty();
        _displayString2->getChild()->text = v[1];
        _displayString2->setDirty();
    }

private:
    float _changeParam = -1;

    BufferingParent<SqLabel>* _displayString;
    BufferingParent<SqLabel>* _displayString2;

    ScoreChord* _scoreChord = nullptr;
    void addScore(VisualizerModule* module) {
        _scoreChord = new ScoreChord(module);
        auto size = Vec(134, 100);
        auto vu = new BufferingParent<ScoreChord>(_scoreChord, size, _scoreChord);

        vu->box.pos = Vec(8, 28),
        addChild(vu);
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

Model* modelVisualizer = createModel<VisualizerModule, VisualizerWidget>("sqh-visualizer");
#endif