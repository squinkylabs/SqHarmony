#pragma once 

#include <memory>
class Keysig;
class Style;

using KeysigPtr = std::shared_ptr<Keysig>;
using StylePtr = std::shared_ptr<Style>;

class Options {
public:
    Options(KeysigPtr k, StylePtr s) : keysig(k), style(s) {
    }
    
    KeysigPtr keysig;
    StylePtr style;
};

using OptionsPtr = std::shared_ptr<Options>;
