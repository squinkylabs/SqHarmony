/* Style.cpp
 */

#include "Style.h"

bool Style::allowVoiceCrossing() const {
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
    // new for 2023: make narrow not include C, it starts at D
   // return isNarrowRange() ? 62 : 60;
   // For this release, let's keep middle C.
   return 60;
}

int Style::maxSop() const {
    // New for 2023: wide goes up to C
    return isNarrowRange() ? 79 : 84;
}

int Style::minAlto() const {
    // if (specialTestMode) {
    //     return 55 + dx;
    // }
    // return 55;

    // New for 2023: wide goes down to E
    return isNarrowRange() ? 55 : 52;
}

int Style::maxAlto() const {
    //  return isNarrowRange() ? 74 - dx : 74;

    // New for 2023: wide goes up to F
    return isNarrowRange() ? 74 : 77;
}

int Style::minTenor() const {
    // 4 X 12, C3

    // New for 2023: both wide and narrow start at C
    return 48;
}

int Style::maxTenor() const {
    // if (specialTestMode) {
    //     return 69 - dx;
    // }
    // New for 2023: wide goes to C
    return isNarrowRange() ? 69 : 72;
}

int Style::minBass() const {
    // was 39
    // New for 2023: bass D on wide, F on narrow
    // well, back track from that 41 in vocal range back to 39
    return isNarrowRange() ? 39 : 38;
}

int Style::maxBass() const {
     // New for 2023: goes up to D on wide
    return isNarrowRange() ? 60 : 62;
}

