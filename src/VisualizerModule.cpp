
#include "plugin.hpp"  // must be first include (for now).
#ifdef _VISUALIZER

#include <string>

#include "BufferingParent.h"
#include "GfxUtils.h"
#include "NumberFormatter.h"
#include "PhasePatterns.h"
#include "PopupMenuParamWidget.h"
#include "ScoreChord.h"
#include "SqLabel.h"
#include "SqLog.h"

#define _LAB

template <>
int BufferingParent<ScoreChord>::_refCount = 0;

/**
 * TODO:
 * fix score drawing (details in ScoreChord.h)
 * make panel look decent
 * make docs
 * build current sdk on linux
 * fully test scoring
 * test recognizer
 * implement 9th chords?
 * look at text for chord type names.
 *
 *
 */
class VisualizerWidget : public ModuleWidget {
public:
    VisualizerWidget(VisualizerModule* module) {
        setModule(module);

        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/phase-patterns.svg")));
        addScore(module);
#if 1  // def _LAB
        addLabel(Vec(20, 6), "Visualizer", 20);
        addLabel(Vec(25, 356), "Squinktronix", 16);
#endif
        addRow2();
        addRow1();

        _displayString = addLabel(Vec(10, 160), "chord");
        _displayString2 = addLabel(Vec(10, 180), "chord");

        addKeysig();
    }

    unsigned getChangeParam() {
        if (!module) {
            return 0;
        }
        return unsigned(APP->engine->getParamValue(module, Comp::CHANGE_PARAM));
    }

    void step() override {
        if (!module) {
            Widget::step();
            return;
        }
        _displayString->step();
        _displayString2->step();
        const float changeParam = getChangeParam();
        if (changeParam == _changeParam) {
            Widget::step();
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

        Widget::step();
    }

private:
    unsigned _changeParam = 1000;

    BufferingParent<SqLabel>* _displayString;
    BufferingParent<SqLabel>* _displayString2;
    PopupMenuParamWidget* _keyRootWidget = nullptr;

    ScoreChord* _scoreChord = nullptr;
    void addScore(VisualizerModule* module) {
        _scoreChord = new ScoreChord(module);
        auto size = Vec(104, 100);
        auto vu = new BufferingParent<ScoreChord>(_scoreChord, size, _scoreChord);

        vu->box.pos = Vec(8, 28),
        addChild(vu);
    }

    void addKeysig() {
        const float yScale = 170;
        const float yMode = yScale + 28;

        PopupMenuParamWidget* p = createParam<PopupMenuParamWidget>(
            Vec(8, yScale),
            module,
            Comp::KEY_PARAM);
        p->setLabels(Scale::getRootLabels(false));
        p->box.size.x = 40;
        p->box.size.y = 22;
        p->text = "C";
        addParam(p);
        _keyRootWidget = p;  // Remember this so we can poll it.

        p = createParam<PopupMenuParamWidget>(
            Vec(8, yMode),
            module,
            Comp::MODE_PARAM);
        p->setShortLabels(Scale::getShortScaleLabels(true));
        p->setLabels(Scale::getScaleLabels(true));

        p->box.size.x = 70;  // width
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
    const float x1 = 66;
    void addRow2() {
        const float y = 317;

        RoundedRect* r = new RoundedRect(Vec(x1 - 7, y - 18), Vec(39.5, 54));
        addChild(r);
        addInputL(Vec(x0, y), Comp::CV_INPUT, "V/Oct", 3);
        addOutputL(Vec(x1, y), Comp::PES_OUTPUT, "PES", -1);
    }

    void addRow1() {
        float y = 270;
        addInputL(Vec(x0, y), Comp::GATE_INPUT, "[Gate]", 6.5f);
        addInputL(Vec(x1, y), Comp::PES_INPUT, "PES", -1);

        y = 258;
        addChild(createLight<SmallLight<RedLight>>(
            Vec(x1 - 8, y),
            module,
            Comp::PES_INVALID_LIGHT));
    }
};

Model* modelVisualizer = createModel<VisualizerModule, VisualizerWidget>("sqh-visualizer");
#endif