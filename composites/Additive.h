
#pragma once
#include "sq_rack.h"

using Module = ::rack::engine::Module;

template <class TBase>
class Additive : public TBase {
public:
    // Additive();
    Additive(Module *);
};

template <class TBase>
Additive<TBase>::Additive(Module *module) : TBase(module) {
}
