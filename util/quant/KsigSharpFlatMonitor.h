#pragma once


class PopupMenuParamWidget;

// TComp is the composite type
// TWidget is the type of wieget we are monitoring. TWidget is PopupMenuParamWidget in the real case.
template <typename TComp, typename TWidget>
class KsigSharpFlatMonitor {
private:
    const TComp* const _comp;
    TWidget* const _keyRootWidget;

public:
    KsigSharpFlatMonitor() = delete;
    KsigSharpFlatMonitor(const KsigSharpFlatMonitor&) = delete;
    KsigSharpFlatMonitor(const TComp* comp, TWidget* rootWidget) : _comp(comp), _keyRootWidget(rootWidget) {
        
    }
    void poll() {
        assert(_comp);
        const int basePitch = int(std::round(_comp->params[TComp::KEY_PARAM].value));
        const auto mode = Scale::Scales(int(std::round(_comp->params[TComp::MODE_PARAM].value)));
        auto _quantizerOptions = std::make_shared<ScaleQuantizer::Options>();
        _quantizerOptions->scale = std::make_shared<Scale>();
        _quantizerOptions->scale->set(basePitch, mode);

        const auto scale = _quantizerOptions->scale;
        const auto scaleSharpFlatPref = scale->getSharpsFlatsPref();
        const std::string sRoot = _keyRootWidget->getShortLabel(1);
        bool isSharps = sRoot.find('#') != std::string::npos;

        const bool shouldUseSharps = (scaleSharpFlatPref == Scale::SharpsFlatsPref::Sharps);
        if ((scaleSharpFlatPref != Scale::SharpsFlatsPref::DontCare) &&
            (shouldUseSharps != isSharps)) {
            SQINFO("need to change should use sharps =%d!!", shouldUseSharps);
            _keyRootWidget->setLabels(Scale::getRootLabels(!shouldUseSharps));
        }
    }
};