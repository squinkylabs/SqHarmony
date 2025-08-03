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
#ifdef _LFN
    p->addModel(modelLFN2);
#endif
#ifdef _H2
    p->addModel(modelHarmony2);
#endif
#ifdef _VISUALIZER
    p->addModel(modelVisualizer);
#endif
#ifdef _MUTATOR
    p->addModel(modelMutator);
#endif
}

