
#include "plugin.hpp"
#ifdef _H3

#include "BufferingParent.h"
#include "NumberFormatter.h"
#include "PhasePatterns.h"
#include "PopupMenuParamWidget.h"
#include "SqLabel.h"
#include "SqLog.h"
#include "WidgetComposite.h"

#include "Harmony3.h"

using Comp = Harmony3<WidgetComposite>;

class Harmony3Module : public rack::engine::Module {
public:
    std::shared_ptr<Comp> comp = std::make_shared<Comp>(this);
    Harmony3Module() {
        config(Comp::NUM_PARAMS, Comp::NUM_INPUTS, Comp::NUM_OUTPUTS, Comp::NUM_LIGHTS);
        // addParams();
    }
};

class Harmony3Widget : public ModuleWidget {
public:
    Harmony3Widget(Harmony3Module* module) {
    }
};

Model* modelHarmony3 = createModel<Harmony3Module, Harmony3Widget>("sqh-harmony3");
#endif