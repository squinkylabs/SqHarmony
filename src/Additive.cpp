#include "plugin.hpp"
#include "rack.hpp"

#define _LAB

class AdditiveModule : public Module {
};

class AdditiveWidget : public ModuleWidget {
public:
    AdditiveWidget(AdditiveModule*& module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/blank-panel.svg")));
#ifdef _LAB
        addLabel(Vec(20, 12), "Additive", 24);
#endif
    }

private:
#ifdef _LAB
    Label* addLabel(const Vec& v, const std::string& str, float fontSize = 14) {
        NVGcolor white = nvgRGB(0xe0, 0xe0, 0xe0);
        Label* label = new Label();
        label->box.pos = v;
        label->text = str;
        label->color = white;
        label->fontSize = fontSize;
        addChild(label);
        return label;
    }
#endif
};

rack::Model* modelAdditive = createModel<AdditiveModule, AdditiveWidget>("sqh-additive");