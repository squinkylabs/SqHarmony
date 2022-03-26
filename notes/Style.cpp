/* Style.cpp
 */

#include "Style.h"

bool Style::allowVoiceCrossing() {
    return false;
}

void Style::setInversionPreference(InversionPreference i) {
    inversionPreference = i;
}

Style::InversionPreference Style::getInversionPreference() const {
    return inversionPreference;
}

void Style::setRangesPreference(Ranges r) {
    rangesPreference = r;
}

Style::Ranges Style::getRangesPreference() const {
    return rangesPreference;
}

void Style::setNoNotesInCommon(bool b) {
    enableNoNotesInCommonRule = b;
}

bool Style::getNoNotesInCommon() const {
    return enableNoNotesInCommonRule;
}

int Style::minSop() const {
    if (specialTestMode) {
        return dx + 60;
    }
    return 60;
}

int Style::maxSop() const {
    return isNarrowRange() ? 79 - dx : 79;
}

int Style::minAlto() const {
    if (specialTestMode) {
        return 55 + dx;
    }
    return 55;
}

int Style::maxAlto() const {
    return isNarrowRange() ? 74 - dx : 74;
}

int Style::minTenor() const {
    // 4 X 12, C3
    return isNarrowRange() ? 48 + dx : 48;
}

int Style::maxTenor() const {
    if (specialTestMode) {
        return 69 - dx;
    }
    return 69;
}

int Style::minBass() const {
    return isNarrowRange() ? 39 + dx : 39;
}

int Style::maxBass() const {
    if (specialTestMode) {
        return 60 - dx;
    }
    return 60;
}

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
