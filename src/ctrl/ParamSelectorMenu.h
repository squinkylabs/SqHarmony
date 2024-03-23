#pragma once

#include "SqMenuItem.h"
#include "rack.hpp"

class ParamSelectorMenuItem {
public:
    ParamSelectorMenuItem() = delete;
    static ::rack::ui::MenuItem* make(int value,
                                      const std::string& text,
                                      bool selected,
                                      std::function<void(int)> setter) {
        std::function<bool()> isCheckedFn = [selected]() {
            return selected;
            return false;
        };

        std::function<void()> clickFn = [value, setter]() {
            setter(value);
        };

        auto ret = new SqMenuItem(isCheckedFn, clickFn);
        ret->text = text;
        return ret;
    }
};

using Module = ::rack::engine::Module;
/**
 * @brief a menu item that opens up more options
 *
 */
class ParamSelectorMenu : public ::rack::ui::MenuItem {
public:
    ParamSelectorMenu(
        const std::string& name,
        const std::vector<std::string> items,
        Module* module,
        int paramterId);
    // This is a lower level ctor - not hard-coded to any param stuff
    ParamSelectorMenu(
        const std::string& name,
        const std::vector<std::string> items,
        int initialSelection,
        std::function<void(int)> valueSetter);

    ::rack::ui::Menu* createChildMenu() override;

private:
    const std::string name;
    const std::vector<std::string> itemNames;
    std::function<void(int)> valueSetter;
    int initialValue = 0;

    // to support param version
    Module* module = nullptr;
    int paramId = -1;
};

inline ParamSelectorMenu::ParamSelectorMenu(
    const std::string& name,
    const std::vector<std::string> items,
    int initialSelection,
    std::function<void(int)> valueSetter) : name(name), itemNames(items), valueSetter(valueSetter) {
    this->text = name;
    initialValue = initialSelection;
}

inline ParamSelectorMenu::ParamSelectorMenu(
    const std::string& name,
    const std::vector<std::string> items,
    Module* module,
    int parameterId) : name(name), itemNames(items), module(module), paramId(parameterId) {
    this->text = name;
    float v = 0;
    if (module) {
        v = module->paramQuantities[paramId]->getValue();
    }
    initialValue = int(std::round(v));
}

inline ::rack::ui::Menu* ParamSelectorMenu::createChildMenu() {
    ::rack::ui::Menu* menu = new ::rack::ui::Menu();
    auto label = ::rack::construct<::rack::ui::MenuLabel>(
        &rack::ui::MenuLabel::text,
        name);
    menu->addChild(label);

    auto setter = this->valueSetter;
    if (module) {
        // for the "high level" case we need to make a setter function
        auto mod = module;
        auto pid = paramId;
        assert(paramId >= 0);
        setter = [mod, pid](int value) {
            mod->paramQuantities[pid]->setValue(float(value));
        };
    }
    int value = 0;
    for (auto it : itemNames) {
        const bool bSelected = value == initialValue;
        auto item = ParamSelectorMenuItem::make(value, it, bSelected, setter);
        value += 1;
        menu->addChild(item);
    }
    return menu;
}