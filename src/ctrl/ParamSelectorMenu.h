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

/**
 * @brief a menu item that opens up more options
 *
 */
class ParamSelectorMenu : public ::rack::ui::MenuItem {
public:
    ParamSelectorMenu(
        const std::string& name,
        const std::vector<std::string> items,
        int initialSelection,
        std::function<void(float)> valueSetter);

    ::rack::ui::Menu* createChildMenu() override;

private:
    const std::string name;
    const std::vector<std::string> itemNames;
    std::function<void(float)> valueSetter;
    std::string initName;
};

inline ParamSelectorMenu::ParamSelectorMenu(
    const std::string& name,
    const std::vector<std::string> items,
    int initialSelection,
    std::function<void(float)> valueSetter) : name(name), itemNames(items), valueSetter(valueSetter) {
    this->text = name;
    this->initName = itemNames[initialSelection];
}

::rack::ui::Menu* ParamSelectorMenu::createChildMenu() {
    ::rack::ui::Menu* menu = new ::rack::ui::Menu();
    auto label = ::rack::construct<::rack::ui::MenuLabel>(
        &rack::ui::MenuLabel::text,
        name);
    menu->addChild(label);
    float value = 0;
    for (auto it : itemNames) {
        const bool bSelected = it == initName;
        auto item = ParamSelectorMenuItem::make(value, it, bSelected, valueSetter);
        value += 1;
        menu->addChild(item);
    }
    return menu;
}