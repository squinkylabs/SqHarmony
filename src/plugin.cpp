#include "plugin.hpp"

Plugin* pluginInstance;

void init(rack::Plugin* p) {
    pluginInstance = p;

    p->addModel(modelHarmony1);
    p->addModel(modelArpeggiator1);
#ifdef _ADD
    p->addModel(modelAdditive);
#endif
#ifdef _PM
    p->addModel(modelPhasePatterns);
#endif
#ifdef _MULT
    p->addModel(modelMult);
#endif
    p->addModel(modelLFN2);
}
