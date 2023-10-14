
#include "PhasePatterns.h"

#include "WidgetComposite.h"
#include "plugin.hpp"

using Comp = PhasePatterns<WidgetComposite>;

class PhasePatternsModule : public rack::engine::Module {
public:
    PhasePatternsModule() {
        // config(Comp::NUM_PARAMS, Comp::NUM_INPUTS, Comp::NUM_OUTPUTS, Comp::NUM_LIGHTS);
    }
};

class PhasePatternsWidget : public ModuleWidget {
public:
    PhasePatternsWidget(PhasePatternsModule* module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/blank_panel2.svg")));
#ifdef _LAB
        addLabel(Vec(20, 12), "Additive", 24);
#endif
       // addControls();
       // addIO();
    }

private:
};

Model* modelPhasePatterns = createModel<PhasePatternsModule, PhasePatternsWidget>("sqh-phasepatterns");