#pragma once

#include <functional>
// #include "SqHelper.h"
// #include "SqUI.h"
#include "SqUI.h"
#include "rack.hpp"

/**
 * This menu item takes generic lambdas,
 * so can be used for anything
 **/
struct SqMenuItem : ::rack::MenuItem {
    void onAction(const ::rack::event::Action& e) override {
        _onActionFn();
    }

    void step() override {
        ::rack::MenuItem::step();
        rightText = CHECKMARK(_isCheckedFn());
    }

    SqMenuItem(std::function<bool()> isCheckedFn,
               std::function<void()> clickFn) : _isCheckedFn(isCheckedFn),
                                                _onActionFn(clickFn) {
    }

    SqMenuItem(const char* label,
               std::function<bool()> isCheckedFn,
               std::function<void()> clickFn) : _isCheckedFn(isCheckedFn),
                                                _onActionFn(clickFn) {
        text = label;
    }

private:
    std::function<bool()> _isCheckedFn;
    std::function<void()> _onActionFn;
};

struct SqMenuItemAccel : ::rack::MenuItem {
    void onAction(const ::rack::event::Action& e) override {
        _onActionFn();
    }

    SqMenuItemAccel(const char* accelLabel, std::function<void()> clickFn) : _onActionFn(clickFn) {
        rightText = accelLabel;
    }

private:
    std::function<void()> _onActionFn;
};

#if 0  // don't need this any more
struct ManualMenuItem : SqMenuItem {
    ManualMenuItem(const char* menuText, const char* url) : SqMenuItem(
                                                                []() { return false; },
                                                                [url]() { SqHelper::openBrowser(url); }) {
        this->text = menuText;
    }
};
#endif

/**
 * menu item that toggles a boolean param.
 */
struct SqMenuItem_BooleanParam2 : ::rack::MenuItem {
    SqMenuItem_BooleanParam2(::rack::engine::Module* mod, int id) : paramId(id),
                                                                    module(mod) {
    }

    void onAction(const sq::EventAction& e) override {
        const float newValue = isOn() ? 0 : 1;
        APP->engine->setParamValue(module, paramId, newValue);
        e.consume(this);
    }

    void step() override {
        rightText = CHECKMARK(isOn());
    }

private:
    bool isOn() {
        return APP->engine->getParamValue(module, paramId) > .5;
    }
    const int paramId;
    ::rack::engine::Module* const module;
};

struct SqMenuItem_BooleanParam : ::rack::MenuItem {
    SqMenuItem_BooleanParam(::rack::ParamWidget* widget) : widget(widget) {
    }

    void onAction(const sq::EventAction& e) override {
        const float newValue = isOn() ? 0 : 1;
        if (widget->getParamQuantity()) {
            widget->getParamQuantity()->setValue(newValue);
        }

        sq::EventChange ec;
        widget->onChange(ec);
        e.consume(this);
    }

    void step() override {
        rightText = CHECKMARK(isOn());
    }

private:
    bool isOn() {
        bool ret = false;
        if (widget->getParamQuantity()) {
            ret = widget->getParamQuantity()->getValue() > .5f;
        }

        return ret;
    }
    ::rack::ParamWidget* const widget;
};