
#include "plugin.hpp"
#ifdef _H2

#include "BufferingParent.h"
#include "GfxUtils.h"
#include "Harmony2.h"
#include "KsigSharpFlatMonitor.h"
#include "NumberFormatter.h"
#include "PhasePatterns.h"
#include "PopupMenuParamWidget.h"
#include "Scale.h"
#include "SqLabel.h"
#include "SqLog.h"
#include "SqMenuItem.h"
#include "WidgetComposite.h"

//template <>
//int BufferingParent<SqLabel>::_refCount = 0;

#define _LAB

using Comp = Harmony2<WidgetComposite>;

class Harmony2Module : public rack::engine::Module {
public:
    std::shared_ptr<Comp> comp = std::make_shared<Comp>(this);
    Harmony2Module() {
        config(Comp::NUM_PARAMS, Comp::NUM_INPUTS, Comp::NUM_OUTPUTS, Comp::NUM_LIGHTS);
        addParams();
        addIO();
    }
    void process(const ProcessArgs& args) override {
        comp->process(args);
    }

    std::shared_ptr<Comp> getComp() const {
        return comp;
    }

private:
    void addParams() {
        const int numModes = getComp()->numCurrentModesMax();
        const int numDegrees = 12;
        for (int i = 0; i < NUM_TRANPOSERS; ++i) {
            this->configParam(Comp::XPOSE_DEGREE1_PARAM + i, 0, numDegrees - 1, 0, "Transpose Degrees");
            this->configParam(Comp::XPOSE_OCTAVE1_PARAM + i, 0, 5, 2, "Transpose Octaves", "", 0.f, 1.f, -2.f);
            this->configParam(Comp::XPOSE_ENABLE1_PARAM + i, 0, 10, 0, "hidden");
            this->configParam(Comp::XPOSE_TOTAL1_PARAM + i, 0, 10, 0, "hidden");
            this->configParam(Comp::XPOSE_ENABLEREQ1_PARAM + i, 0, 10, 0, "Enable Channel");
        }
        this->configParam(Comp::KEY_PARAM, 0, 11, 0, "Key signature root");
        this->configParam(Comp::MODE_PARAM, 0, numModes - 1, 0, "Key signature mode");
        this->configParam(Comp::SHARPS_FLATS_PARAM, 0, 3, 0, "hidden (sf)");
        this->configParam(Comp::ONLY_USE_DIATONIC_FOR_CV_PARAM, 0, 1, 0, "hidden (ud)");
    }

    void addIO() {
        this->configOutput(Comp::PITCH_OUTPUT, "Main chord CV");
        this->configOutput(Comp::PES_OUTPUT, "Scale (PES)");

        this->configInput(Comp::KEY_INPUT, "Scale root");
        this->configInput(Comp::XPOSE_INPUT, "Transpose steps");
        this->configInput(Comp::MODE_INPUT, "Scale mode/type");
        this->configInput(Comp::PITCH_INPUT, "Main CV");
        this->configInput(Comp::PES_INPUT, "Scale(PES)");
    }
};

class Harmony2Widget : public ModuleWidget {
public:
    Harmony2Widget(Harmony2Module* module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/harmony2-panel.svg")));
#if 1  // def _LAB
        addLabel(Vec(43, 3), "Harmony II", 20);
        addLabel(Vec(44, 355), "Squinktronix", 17);
#endif
        addTranposeControls(module);
        addKeysig(module);
        addMainCV();
        addModCV();
        addLeds();
        if (module) {
            const Comp* comp = module->getComp().get();
            _ksigMonitor = std::make_shared<KsigSharpFlatMonitor<Comp, PopupMenuParamWidget>>(comp, _keyRootWidget);
        }
    }

private:
    PopupMenuParamWidget* _keyRootWidget = nullptr;
    std::shared_ptr<KsigSharpFlatMonitor<Comp, PopupMenuParamWidget>> _ksigMonitor;
    BufferingParent<SqLabel>* _xposeDisplays[6] = {nullptr};

    void step() override {
        ModuleWidget::step();
        if (module && _ksigMonitor) {
            _ksigMonitor->poll();
        }
        stepForXpose();
    }

    Harmony2Module* _getModule() {
        return static_cast<Harmony2Module*>(getModule());
    }

    void stepForXpose() {
        // no optimization yet.
        for (int bank = 0; bank < NUM_TRANPOSERS; ++bank) {
            std::string s = "";
            auto mod = this->_getModule();
            if (!mod) {
                continue;
            }
            auto comp = mod->getComp();
            const bool enabled = comp->params[Comp::XPOSE_ENABLE1_PARAM + bank].value > 5;
            const bool cvConnected = comp->inputs[Comp::XPOSE_INPUT].isConnected();
            if (enabled && cvConnected) {
                const float f = comp->params[Comp::XPOSE_TOTAL1_PARAM + bank].value;
                const int steps = std::round(f * 12.f);
                std::stringstream str;
                str << steps;
                s = str.str().c_str();
            }
            _xposeDisplays[bank]->getChild()->updateText(s.c_str());
        }
    }

    void _setSharpFlat(int index) {
        APP->engine->setParamValue(module, Comp::SHARPS_FLATS_PARAM, float(index));
    }

    void appendContextMenu(ui::Menu* menu) override {
        if (!module) {
            return;
        }

        SqMenuItem_BooleanParam2* item = new SqMenuItem_BooleanParam2(module, Comp::ONLY_USE_DIATONIC_FOR_CV_PARAM);
        item->text = "Mode CV only diatonic";
        menu->addChild(item);

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

    const float x0 = 11;
    const float dx = 34;
    void addMainCV() {
        const float y = 317;

        RoundedRect* r = new RoundedRect(Vec(73, y - 18), Vec(70, 54));
        addChild(r);

        addInputL(Vec(x0, y), Comp::PITCH_INPUT, "V/Oct", 3);
        addOutputL(Vec(x0 + dx * 2, y), Comp::PITCH_OUTPUT, "V/Oct", 3);

        addOutputL(Vec(x0 + dx * 3, y), Comp::PES_OUTPUT, "PES", -1);
    }

    void addModCV() {
        const float y = 270;
        addInputL(Vec(x0, y), Comp::XPOSE_INPUT, "XP");

        addInputL(Vec(x0 + dx, y), Comp::KEY_INPUT, "Key", 0.f);
        addInputL(Vec(x0 + 2 * dx, y), Comp::MODE_INPUT, "Mode", -1);

        addInputL(Vec(x0 + 3 * dx, y), Comp::PES_INPUT, "PES", -1);
    }

    void addLeds() {
        const float y = 258;
        addChild(createLight<SmallLight<RedLight>>(
            Vec(26 + x0 + 3 * dx, y),
            module,
            Comp::PES_INVALID_LIGHT));
    }

    void addKeysig(Harmony2Module* xmodule) {
        const float yScale = 216;
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
        _keyRootWidget = p;  // remember this so we can poll it.

        p = createParam<PopupMenuParamWidget>(
            Vec(74, yMode),
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

    void addTranposeControls(Harmony2Module* module) {
        for (int i = 0; i < 6; ++i) {
            addTransposeControls(i, module != nullptr);
        }
    }

    static constexpr float y0 = 40;
    static constexpr float deltaY = 27;  // was 28
    static constexpr float xbutton = 5;
    static constexpr float xoctave = 28;
    static constexpr float xdegree = 75;
    static constexpr float xx = 120;

    void addTransposeControls(int index, bool haveModule) {
        const float y = y0 + index * deltaY;
        addParam(createLightParam<VCVLightButton<MediumSimpleLight<WhiteLight>>>(
            Vec(xbutton, y+1),
            module,
            Comp::XPOSE_ENABLEREQ1_PARAM + index,
            Comp::XPOSE_ENABLE1_LIGHT + index));
        PopupMenuParamWidget* p = createParam<PopupMenuParamWidget>(
            Vec(xoctave, y),
            module,
            Comp::XPOSE_OCTAVE1_PARAM + index);
        p->setLabels(Comp::getTransposeOctaveLabels());
        p->box.size.x = 38;  // width
        p->box.size.y = 22;
        addParam(p);
        if (!haveModule) {
            p->text = "0";
        }

        p = createParam<PopupMenuParamWidget>(
            Vec(xdegree, y),
            module,
            Comp::XPOSE_DEGREE1_PARAM + index);
        p->setLabels(Comp::getTransposeDegreeLabels());
        p->box.size.x = 44;  // width
        p->box.size.y = 22;
        if (!haveModule) {
            p->text = "0";
        }
        addParam(p);

        const auto x = addLabel(Vec(xx, y), "");
        _xposeDisplays[index] = x;
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
};

Model* modelHarmony2 = createModel<Harmony2Module, Harmony2Widget>("sqh-harmony2");
#endif