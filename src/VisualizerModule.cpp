
#include "plugin.hpp"  // must be first include (for now).
#ifdef _VISUALIZER

#include <string>

#include "BufferingParent.h"
#include "GfxUtils.h"
#include "KsigSharpFlatMonitor.h"
#include "NumberFormatter.h"
#include "PhasePatterns.h"
#include "PopupMenuParamWidget.h"
#include "ScoreChord.h"
#include "SqLabel.h"
#include "SqMenuItem.h"
#include "SqLog.h"

#define _LAB

//template <>
//int BufferingParent<ScoreChord>::_refCount = 0;

/**
 * TODO:
 * fix score drawing (details in ScoreChord.h)
 * fully test scoring
 * test recognizer
 * sharps/flats for main recognizer display?

 * recognize fourth inversion (or remove inversion from 9th chords?)
 * make docs
 * remove all SQINFO
 * 
 * make panel look decent (done first pass)
 * recognize all inversions of 7th (done)
 * implement 9th chords? C9, Cm9, CMaj9 (done)
 * look at text for chord type names. C7 CMaj7 CMin7, CMinMaj7 (done)
 * add tooltips for I/O, etc. (done)
 * build current sdk on linux (done)
 * ninth with a suspended 5th?
 * merge to main (done)
 */
class VisualizerWidget : public ModuleWidget {
public:
    VisualizerWidget(VisualizerModule* module) {
        setModule(module);
        _module = module;

        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/visualizer.svg")));
        addScore(module);
#if 1  // def _LAB
        addLabel(Vec(33.5, 6), "Visualizer", 20);
        addLabel(Vec(35, 356), "Squinktronix", 17);
#endif
        addRow2();
        addRow1();

        const float y = 144.5;
        const float x = 8.5;
        _displayString = addLabel(Vec(x, y), "");
        _displayString2 = addLabel(Vec(x, y+18.5), "");

        addKeysig();

        if (module) {
            const Comp* comp = module->getComp().get();
            _ksigMonitor = std::make_shared<KsigSharpFlatMonitor<Comp, PopupMenuParamWidget>>(comp, _keyRootWidget);
        }
    }

    unsigned getChangeParam() {
        if (!module) {
            return 0;
        }
        return unsigned(APP->engine->getParamValue(module, Comp::CHANGE_PARAM));
    }

     bool _stepForKeysig()  {
        ModuleWidget::step();
        bool ret = false;
        if (module && _ksigMonitor) {
            const bool bChange =_ksigMonitor->poll();
            if (bChange) {
                vu->setDirty();
                _displayString->setDirty();
                _displayString2->setDirty();
                ret = true;
            }
        }
        return ret;
    }

    void step() override {
        if (!module) {
            Widget::step();
            return;
        }
        const bool bChange = _stepForKeysig();
        _displayString->step();
        _displayString2->step();
        const float changeParam = getChangeParam();
        if ((changeParam == _changeParam) && !bChange) {
            Widget::step();
            return;
        }

        _changeParam = changeParam;
        const ChordRecognizer::Type type = ChordRecognizer::Type(APP->engine->getParamValue(module, Comp::TYPE_PARAM));
        const ChordRecognizer::Inversion inversion = ChordRecognizer::Inversion(APP->engine->getParamValue(module, Comp::INVERSION_PARAM));
        const int root = APP->engine->getParamValue(module, Comp::ROOT_PARAM);
        SqArray<ChordRecognizer::PitchAndIndex, 16> idp;
        ChordRecognizer::ChordInfo info = ChordRecognizer::ChordInfo(type, inversion, root, idp);
        const bool useSharps = resolveSharpPref(_module->getSharpsFlatsPref(), *(_module->getScale())); 
        const auto v = ChordRecognizer::toString(info, useSharps);
        _displayString->getChild()->text = v[0];
        _displayString->setDirty();
        _displayString2->getChild()->text = v[1];
        _displayString2->setDirty();

        Widget::step();
    }

private:
    unsigned _changeParam = 1000;
    VisualizerModule* _module = nullptr;

    BufferingParent<SqLabel>* _displayString;
    BufferingParent<SqLabel>* _displayString2;
    PopupMenuParamWidget* _keyRootWidget = nullptr;
    std::shared_ptr<KsigSharpFlatMonitor<Comp, PopupMenuParamWidget>> _ksigMonitor;

    ScoreChord* _scoreChord = nullptr;

    BufferingParent<ScoreChord>* vu = nullptr;
    void addScore(VisualizerModule* module) {
        _scoreChord = new ScoreChord(module);
        auto size = Vec(116, 100);
        vu = new BufferingParent<ScoreChord>(_scoreChord, size, _scoreChord);

        vu->box.pos = Vec(8, 40.0f);
        addChild(vu);
    }

    void addKeysig() {
        const float yMode = 216;
        const float yScale = yMode - 24;

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
        p->text = "Major";
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
    void addRow2() {
        const float y = 317;

        RoundedRect* r = new RoundedRect(Vec(x0 - 7, y - 18), Vec(124.5, 54));        // 120 too narrow
        addChild(r);
       // addInputL(Vec(x0, y), Comp::CV_INPUT, "V/Oct", 3);
        addOutputL(Vec(xPes, y), Comp::PES_OUTPUT, "PES", -1);
        addOutputL(Vec(x0, y), Comp::ROOT_OUTPUT, "Root", -1);
      
      //  addOutputL(Vec(x1, y), Comp::RECOGNIZED_OUTPUT, "VLD", -1);
       addOutputL(Vec(x1, y), Comp::RECOGNIZED_OUTPUT, "Valid", 4);

    }

    void addRow1() {
        float y = 270;
        addInputL(Vec(x1, y), Comp::GATE_INPUT, "[Gate]", 6.5f);
        addInputL(Vec(xPes, y), Comp::PES_INPUT, "PES", -1);
        addInputL(Vec(x0, y), Comp::CV_INPUT, "V/Oct", 3);

        y = 258;
        addChild(createLight<SmallLight<RedLight>>(
            Vec(xPes - 8, y),
            module,
            Comp::PES_INVALID_LIGHT));
    }

        void _setSharpFlat(int index) {
        APP->engine->setParamValue(module, Comp::SHARPS_FLATS_PARAM, float(index));
    }

    void appendContextMenu(ui::Menu* menu) override {
        if (!module) {
            return;
        }

        const float p = APP->engine->getParamValue(module, Comp::SHARPS_FLATS_PARAM);
        const int index = int(std::round(p));
        menu->addChild(createSubmenuItem("Sharps&Flats", "",
                                         [=](Menu* menu) {
                                             menu->addChild(createMenuItem("Default+sharps", CHECKMARK(index == 0), [=]() { _setSharpFlat(0); }));
                                             menu->addChild(createMenuItem("Default+flats", CHECKMARK(index == 1), [=]() { _setSharpFlat(1); }));
                                             menu->addChild(createMenuItem("Sharps", CHECKMARK(index == 2), [=]() { _setSharpFlat(2); }));
                                             menu->addChild(createMenuItem("Flats", CHECKMARK(index == 3), [=]() { _setSharpFlat(3); }));
                                         }));
    }
};

Model* modelVisualizer = createModel<VisualizerModule, VisualizerWidget>("sqh-visualizer");
#endif