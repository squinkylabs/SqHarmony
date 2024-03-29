#pragma once

#define CRAZY_STYLE 0

#include <assert.h>
#include <iostream>
#include <memory>

#include "SqLog.h"

class Style {
public:
    /***** the following methods apply to lowest level: chords generated ****/

    int absMaxPitch() const;          // returns midi note number of max pitch our style allows
    int absMinPitch() const;          // returns midi note number of min pitch our style allows
    bool allowVoiceCrossing() const;  // True is we allow the alto to go above the sop in a given chord
    int maxUnison() const;            // may number of unisons allowed in a chord
    bool allow2ndInversion() const;
    bool allow1stInversion() const;

    bool requireStdDoubling() const;  // chords required to double root, etc???
                                      // I think this means double root always!!
    bool forceDescSop();              // silly test to force melody to descend
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
        ENCOURAGE_CENTER,  // weight rule
        VOCAL_RANGE,       // limit extremes to vocal range.
        WIDE_RANGE,
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

    bool isNarrowRange() const;

    static bool doubleBass() {
        return true;        // For now the new rule is not trustworthy.
    }

private:
    InversionPreference inversionPreference = InversionPreference::DISCOURAGE_CONSECUTIVE;
    Ranges rangesPreference = Ranges::VOCAL_RANGE;
    bool enableNoNotesInCommonRule = true;
};

using StylePtr = std::shared_ptr<Style>;

inline bool Style::isNarrowRange() const {
    bool narrow = false;
    switch (rangesPreference) {
        case Ranges::VOCAL_RANGE:
        case Ranges::ENCOURAGE_CENTER:
            narrow = true;
            break;
        case Ranges::WIDE_RANGE:
            narrow = false;
            break;
        default:
            assert(false);
    }

    return narrow;
}

/* Absolute pitch ranges: these are enforced!
 */
inline int Style::absMaxPitch() const {
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

inline int Style::absMinPitch() const {
    int ret = 0;
#if !CRAZY_STYLE
    ret = minBass();  // used to be =5, seemed wrong
#else
    ret = minBass() - 10;
#endif
    return ret;
}

inline bool Style::allow2ndInversion() const {
    return true;
}

inline bool Style::allow1stInversion() const {
    return true;
}

inline int Style::maxUnison() const {
    return 0;  // bgf 1/4/93
}

inline bool Style::requireStdDoubling() const {
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
