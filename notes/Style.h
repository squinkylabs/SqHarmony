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

    enum class InversionPreference {
        DONT_CARE,
        DISCOURAGE_CONSECUTIVE,
        DISCOURAGE
    };

    void setInversionPreference(InversionPreference);
    InversionPreference getInversionPreference() const;

    enum class Ranges {
        NORMAL_RANGE,
        ENCOURAGE_CENTER,  // weight rule
        NARROW_RANGE       // limit extremes
    };
    void setRangesPreference(Ranges);
    Ranges getRangesPreference() const;

    // void setAllowConsecInversions(bool allow);

    // no notes in common == true means use that rule
    void setNoNotesInCommon(bool);
    bool getNoNotesInCommon() const;

    bool pullTogether() const { return rangesPreference == Ranges::ENCOURAGE_CENTER; }

    bool limitSopranoJumps() const {
        return true;
    }

    bool forbidLeadingToneDoubling() const {
        return true;
    }

    bool forbidLeadingToneChordInRootPosition() const {
        return true;
    }

    bool usePistonV_VI_exception() const {
        return true;
        // return false;
    }

private:
    InversionPreference inversionPreference = InversionPreference::DISCOURAGE_CONSECUTIVE;
    Ranges rangesPreference = Ranges::NORMAL_RANGE;
    bool enableNoNotesInCommonRule = true;

    bool isNarrowRange() const {
        return (rangesPreference == Ranges::NARROW_RANGE);
    }

    // at 5 it's getting noticeably worse, but not awful
    // at 8 I get assertions. should look into that (ranges don't overlap at all)
    // old way 7 crapped out, 6 ok
    // new way 12 crap, 10 asserts 9 ok : 9 best for new way

    // 2023: let's get more constrained, and get rid of special test mode/
    int dx = 8;
    // bool specialTestMode = false;
};

using StylePtr = std::shared_ptr<Style>;

/* Absolute pitch ranges: these are enforced!
 */

inline int Style::absMaxPitch() {
    int ret = 0;
#if !CRAZY_STYLE
    ret = maxSop();  // used to be +5. seemed wrong
#else
    a b c  // we won't support this any longer
        SQWARN("crazy style is on");
    ret = maxSop() + 10;
#endif
    return ret;
}

inline int Style::absMinPitch() {
    int ret = 0;
#if !CRAZY_STYLE
    ret = minBass();  // used to be =5, seemed wrong
#else
    ret = minBass() - 10;
#endif
    return ret;
}

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

inline bool Style::forceDescSop() {
#if !CRAZY_STYLE
    return false;
#else
    // std::cout << "force descend true\n";
    return true;
#endif
}
