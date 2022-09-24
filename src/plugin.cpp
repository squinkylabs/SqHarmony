#include "plugin.hpp"

Plugin* pluginInstance;

void init(rack::Plugin* p) {
    pluginInstance = p;

    p->addModel(modelHarmony1);
    p->addModel(modelArpeggiator1);
    p->addModel(modelAdditive);
}
