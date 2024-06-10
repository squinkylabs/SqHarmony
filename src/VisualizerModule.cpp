
#include "plugin.hpp" // must be first include (for now).
#ifdef _VISUALIZER

#include "BufferingParent.h"
#include "NumberFormatter.h"
#include "PhasePatterns.h"
#include "PopupMenuParamWidget.h"
#include "SqLabel.h"
#include "SqLog.h"
#include "WidgetComposite.h"

#include "Visualizer.h"
using Comp = Visualizer<WidgetComposite>;

class VisualizerModule : public rack::engine::Module {
public:
    std::shared_ptr<Comp> comp = std::make_shared<Comp>(this);
    VisualizerModule() {
        config(Comp::NUM_PARAMS, Comp::NUM_INPUTS, Comp::NUM_OUTPUTS, Comp::NUM_LIGHTS);
        // addParams();
    }
};

class VisualizerWidget : public ModuleWidget {
public:
    VisualizerWidget(VisualizerModule* module) {
    }
};

Model* modelVisualizer = createModel<VisualizerModule, VisualizerWidget>("sqh-visualizer");
#endif