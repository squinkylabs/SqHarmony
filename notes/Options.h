#pragma once

#include <memory>
class KeysigOld;
class Style;

using KeysigOldPtr = std::shared_ptr<KeysigOld>;
using StylePtr = std::shared_ptr<Style>;

class Options {
public:
    Options(KeysigOldPtr k, StylePtr s) : keysig(k), style(s) {
    }

    KeysigOldPtr keysig;
    StylePtr style;
};

using OptionsPtr = std::shared_ptr<Options>;
