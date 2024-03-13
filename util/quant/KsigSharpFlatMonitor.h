#pragma once
template <typename TComp>
class KsigSharpFlatMonitor {
private:
    const TComp* const _comp;
    PopupMenuParamWidget* const _keyRootWidget;

public:
    KsigSharpFlatMonitor() = delete;
    KsigSharpFlatMonitor(const KsigSharpFlatMonitor&) = delete;

    KsigSharpFlatMonitor(const TComp* comp, PopupMenuParamWidget* rootWidget) : _comp(comp), _keyRootWidget(rootWidget) {
        
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