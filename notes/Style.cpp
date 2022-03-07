/* Style.cpp
 */

#include "Style.h"

bool Style::allowVoiceCrossing() {
    return false;
}

void Style::setInversionPreference(Inversion i) {
    inversionPreference = i;
}

Style::Inversion Style::getInversionPreference() const {
    return inversionPreference;
}

void Style::setRangesPreference(Ranges r) {
    rangesPreference = r;
}

Style::Ranges Style::getRangesPreference() const {
    return rangesPreference;
}

/*
    enum class Inversion {
        DISCOURAGE,
        DISCOURAGE_CONSECUTIVE,
        DONT_CARE
    };
    void setInversionPreference(Inversion);
    Inversion getInversionPreference() const;

    enum class Ranges {
        NORMAL_RANGE,
        ENCOURAGE_CENTER,
        NARROW_RANGE
    };
    void setRangetPreference(Ranges);
    Ranges getRangesPreference() const;
    */

#if 0
void Style::print() {
    CString Str;
    dc.TextOut(r.left, r.top, ">> here are the current style settings:");
    r.top += dy;

    Str.Format(_T("Force descend melody = %d"), ForceDescSop());
    dc.TextOut(r.left, r.top, Str);
    r.top += dy;

    Str.Format(_T("Max pitch = %d, min = %d"),
               AbsMaxPitch(),
               AbsMinPitch());
    dc.TextOut(r.left, r.top, Str);
    r.top += dy;

    Str.Format(_T("allow voice crossing = %d max unison = %d"),
               AllowVoiceCrossing(),
               MaxUnison());
    dc.TextOut(r.left, r.top, Str);
    r.top += dy;

    Str.Format(_T("allow 1st inv = %d, allow 2nd = %d"),
               Allow1stInversion(),
               Allow2ndInversion());
    dc.TextOut(r.left, r.top, Str);
    r.top += dy;

    Str.Format(_T("Require std doubling = %d"),
               RequireStdDoubling());
    dc.TextOut(r.left, r.top, Str);
    r.top += dy;
}
#endif

// Style TheStyle;
