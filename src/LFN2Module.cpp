

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
};

class LFN2Widget : public ModuleWidget {
public:
    LFN2Widget(LFN2Module* module){}
};

Model* modelLFN2 = createModel<LFN2Module, LFN2Widget>("sqh-lfn2");