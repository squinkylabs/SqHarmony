#include "rack.hpp"
#include "plugin.hpp"

class AdditiveModule : public Module {
};

class AdditiveWidget : public ModuleWidget {
public:
    AdditiveWidget(AdditiveModule*& module) {
         setModule(module);
    }

};



rack::Model* modelAdditive = createModel<AdditiveModule, AdditiveWidget>("sqh-additive");