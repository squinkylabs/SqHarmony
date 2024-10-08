#pragma once

#include "Scale.h"
#include "ScaleQuantizer.h"
#include "SharpsFlatsPref.h"

class PopupMenuParamWidget;

enum class SharpFlatUserOptions {
    DefaultPlusSharps,
    DefaultPlusFlats,
    SharpsAlways,
    FlatsAlways
};

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
        assert(rootWidget);
    }
    bool poll() {
        assert(_comp);
        const int basePitch = int(std::round(_comp->params[TComp::KEY_PARAM].value));
        assert(basePitch < 12 && basePitch >= 0);
        const auto mode = Scale::Scales(int(std::round(_comp->params[TComp::MODE_PARAM].value)));
        auto _quantizerOptions = std::make_shared<ScaleQuantizer::Options>();
        _quantizerOptions->scale = std::make_shared<Scale>();
        _quantizerOptions->scale->set(MidiNote(basePitch), mode);

        const auto scale = _quantizerOptions->scale;
        const auto scaleSharpFlatPref = scale->getSharpsFlatsPref();
        const std::string sRoot = _keyRootWidget->getShortLabel(1);
        bool isSharps = sRoot.find('#') != std::string::npos;

        const bool scalePrefersSharps = (scaleSharpFlatPref == SharpsFlatsPref::Sharps);
        const float compositePref = int(std::round(_comp->params[TComp::SHARPS_FLATS_PARAM].value));
        const bool defaultIsOkWithComposite = (compositePref < 2);

        const bool compositeTieBreakerIsSharps = compositePref == 0 || compositePref == 2;

        // If we are cool with defaults, and ksig has a default, and we are set to it, leave
        // we don't really need this case...
        if (defaultIsOkWithComposite && (scaleSharpFlatPref != SharpsFlatsPref::DontCare) &&
            (scalePrefersSharps == isSharps)) {
            return false;
        }
        // If we are cool with defaults, and ksig has a default, and we are NOT set to it, set to it.
        if (defaultIsOkWithComposite && (scaleSharpFlatPref != SharpsFlatsPref::DontCare) &&
            (scalePrefersSharps != isSharps)) {
            // then set to default, and return.
            _keyRootWidget->setLabels(Scale::getRootLabels(!scalePrefersSharps));
            return true;
        }
        // If it's a don't care scale, but we aren't set to pref then set to user pref.
        if (defaultIsOkWithComposite && (scaleSharpFlatPref == SharpsFlatsPref::DontCare) &&
            (compositeTieBreakerIsSharps != isSharps)) {
            _keyRootWidget->setLabels(Scale::getRootLabels(!compositeTieBreakerIsSharps));
            return true;
        }
        // if use has a hard pref for sharps or flats, and we are not on it, set it
        if (!defaultIsOkWithComposite && (compositeTieBreakerIsSharps != isSharps)) {
            _keyRootWidget->setLabels(Scale::getRootLabels(!compositeTieBreakerIsSharps));
            return true;
        }

        return false;
    }
};
