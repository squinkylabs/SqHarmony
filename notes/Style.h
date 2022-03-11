#pragma once

#define CRAZY_STYLE 0

#include <iostream>
#include <memory>

#include "SqLog.h"

class Style {
public:

    /***** the following methods apply to lowest level: chords generated ****/

    int absMaxPitch();          // returns midi note number of max pitch our style allows
    int absMinPitch();          // returns midi note number of min pitch our style allows
    bool allowVoiceCrossing();  // True is we allow the alto to go above the sop in a given chord
    int maxUnison();            // may number of unisons allowed in a chord
    bool allow2ndInversion();
    bool allow1stInversion();

    bool requireStdDoubling();  // chords required to double root, etc???
                                // I think this means double root always!!
    bool forceDescSop();        // silly test to force melody to descend
                                //  bool allowConsecInversions();  // allow a first or second inversion to follow another?

    int minSop() const;
    int maxSop() const;
    int minAlto() const;
    int maxAlto() const;
    int minTenor() const;
    int maxTenor() const;
    int minBass() const;
    int maxBass() const;

    /** new: setters *****************/

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
    void setRangesPreference(Ranges);
    Ranges getRangesPreference() const;

    // void setAllowConsecInversions(bool allow);

    // no notes in common == true means use that rule
    void setNoNotesInCommon(bool);
    bool getNoNotesInCommon() const;

private:
    // bool _allowConsecInversions = false;
    Inversion inversionPreference = Inversion::DISCOURAGE_CONSECUTIVE;
    Ranges rangesPreference = Ranges::NORMAL_RANGE;
    bool enableNoNotesInCommonRule = true;

    bool isNarrowRange() const { return rangesPreference == Ranges::NARROW_RANGE; }

    // at 5 it's getting noticeably worse, but not awful
    // at 8 I get assertions. should look into that
    static const int dx{6};
};

using StylePtr = std::shared_ptr<Style>;

// inline void Style::setAllowConsecInversions(bool allow) {
//     _allowConsecInversions = allow;
// }

/* Recommended min/max for the four voices */

inline int Style::minSop() const {
    // Middle C, C4

    return isNarrowRange() ? 60 + dx : 60;
}

inline int Style::maxSop() const {
    // G5
    return isNarrowRange() ? 79 - dx : 79;
}

inline int Style::minAlto() const {
    // 4x12 + 7
    // G3
    return isNarrowRange() ? 55 + dx : 55;
}
inline int Style::maxAlto() const {
    // 6 * 12 + 2
    // D5
    return isNarrowRange() ? 74 - dx : 74;
}
inline int Style::minTenor() const {
    // 4 X 12, C3
    return isNarrowRange() ? 48 + dx : 48;
}
inline int Style::maxTenor() const {
    // 5 X 12 + 9
    // A4
    return isNarrowRange() ? 69 - dx : 69;
}
inline int Style::minBass() const {
    // 3 X 12 + 3
    // D# 2
    return isNarrowRange() ? 39 + dx : 39;
}
inline int Style::maxBass() const {
    // C4
    return isNarrowRange() ? 60 - dx : 60;
}

/* Absolute pitch ranges: these are enforced!
 */

inline int Style::absMaxPitch() {
    int ret = 0;
#if !CRAZY_STYLE
    ret = maxSop();     // used to be +5. seemed wrong
#else
    SQWARN("crazy style is on");
    ret = maxSop() + 10;
#endif
    return ret;
}

inline int Style::absMinPitch() {
    int ret = 0;
#if !CRAZY_STYLE
    ret = minBass();        // used to be =5, seemed wrong
#else
    ret = minBass() - 10;
#endif
    return ret;
}

#if 0
inline bool Style::OnlyRootPosition()
{
return true;
}
#endif

inline bool Style::allow2ndInversion() {
    // module version couldn't handle this being false - it would need backtracking.
    // but when true 1 - test fails?
    return true;
}

inline bool Style::allow1stInversion() {
    return true;
}

inline int Style::maxUnison() {
    return 0;  // bgf 1/4/93
}

inline bool Style::requireStdDoubling() {
    return true;
}

#if 0
inline bool Style::allowConsecInversions() {
    // allow a first inversion to follow another?
    return _allowConsecInversions;
}
#endif

inline bool Style::forceDescSop() {
#if !CRAZY_STYLE
    return false;
#else
    // std::cout << "force descend true\n";
    return true;
#endif
}
