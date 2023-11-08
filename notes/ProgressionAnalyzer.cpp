

#include "ProgressionAnalyzer.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <sstream>

#include "Chord4.h"
#include "SqLog.h"

static bool showAlways = false;
void ProgressionAnalyzer::showAnalysis() {
    showAlways = true;
}

/* ProgressionAnalyzer::ProgressionAnalyzer(const Chord4 * const C1, const Chord4 * const C2)
    : First (C1), Next(C2)
 */
ProgressionAnalyzer::ProgressionAnalyzer(const Chord4* C1, const Chord4* C2, bool fs)
    : first(C1), next(C2), firstRoot(C1->fetchRoot()), nextRoot(C2->fetchRoot()), show(fs || showAlways) {
    figureMotion();  // init the motion guys
    notesInCommon = InCommon();

#ifdef _DEBUG
    if (show) {
        SQINFO("*** Const of analyzer, chords are:");
        first->dump();
        next->dump();
    }
#endif
}

int ProgressionAnalyzer::getPenalty(const Options& options, int upperBound) const {
#if 0  // let's get rid of this fake rule
    if (!FakeRuleForDesc(options)) {
        return false;
    }
#endif
    assert(&options);
    assert(this);

    std::stringstream str;
    if (show) {
        str << ".. enter getPenalty." << std::endl;
        str << "first: ";
        str << first->toString();
        str << " second: ";
        str << next->toString() << std::endl;
        str << " upper bound ";
        str << upperBound;
        str << next->toString() << std::endl;
    }

    int totalPenalty = 0;
    int p = RuleForInversions(options);
    totalPenalty += p;
    if (p && show) {
        str << "Penalty: RuleForConsecInversions " << p << std::endl;
    }

    if (totalPenalty >= upperBound) {
        if (show) {
            str << "-- leaving getPenalty after consec inv with " << totalPenalty << std::endl;
            SQINFO("%s", str.str().c_str());
        }
        return totalPenalty;
    }

    p = RuleForLeadingTone();
    totalPenalty += p;
    if (p && show) {
        str << "penalty: RuleForLeadingTone " << p << std::endl;
    }
    if (totalPenalty >= upperBound) {
        if (show) {
            str << "-- leaving getPenalty after leading tone with " << totalPenalty << std::endl;
            SQINFO("%s", str.str().c_str());
        }
        return totalPenalty;
    }

    p = RuleForPara();
    totalPenalty += p;

    if (p && show) {
        str << "penalty: RuleForPara " << p << std::endl;
    }
    if (totalPenalty >= upperBound) {
        if (show) {
            str << "-- leaving getPenalty after para with " << totalPenalty << std::endl;
            SQINFO("%s", str.str().c_str());
        }
        return totalPenalty;
    }

    p = RuleForCross();
    totalPenalty += p;
    if (p && show) {
        str << "penalty: RuleForCross " << p << std::endl;
    }
    if (totalPenalty >= upperBound) {
        if (show) {
            str << "-- leaving getPenalty after cross with " << totalPenalty << std::endl;
            SQINFO("%s", str.str().c_str());
        }
        return totalPenalty;
    }

    p = Rule4Same();
    totalPenalty += p;
    if (p && show) {
        str << "penalty: Rule4Same " << p << " tot=" << totalPenalty <<  std::endl;
    }
    if (totalPenalty >= upperBound) {
        if (show) {
            str << "-- leaving getPenalty after 4sam with " << totalPenalty << std::endl;
            SQINFO("%s", str.str().c_str());
        }
        return totalPenalty;
    }

    p = RuleForNoneInCommon(options);
    totalPenalty += p;
    if (p && show) {
        str << "penalty: RuleForNonInCommon " << p << std::endl;
    }
    if (totalPenalty >= upperBound) {
        if (show) {
            str << "-- leaving getPenalty after NIC with " << totalPenalty << std::endl;
            SQINFO("%s", str.str().c_str());
        }
        return totalPenalty;
    }

    p = ruleForDoubling(options);
    totalPenalty += p;
    if (p && show) {
        str << "penalty: RuleForDoubling " << p << std::endl;
    }
    if (totalPenalty >= upperBound) {
        if (show) {
            str << "-- leaving getPenalty after doubling with " << totalPenalty << std::endl;
            SQINFO("%s", str.str().c_str());
        }
        return totalPenalty;
    }

    p = ruleForSpreading(options);
    totalPenalty += p;
    if (p && show) {
        str << "penalty: RuleForSpreading " << p << std::endl;
    }
    if (totalPenalty >= upperBound) {
        if (show) {
            str << "-- leaving getPenalty after spreading with " << totalPenalty << std::endl;
            SQINFO("%s", str.str().c_str());
        }
        return totalPenalty;
    }

    p = RuleForJumpSize();
    totalPenalty += p;
    if (p && show) {
        str << "penalty: RuleForJumpSize " << p << std::endl;
    }

    p = RuleForSopranoJump(options);
    totalPenalty += p;
    if (p && show) {
        str << "penalty: RuleForSopranoJump " << p << std::endl;
    }

    if (totalPenalty >= upperBound) {
        if (show) {
            str << "-- leaving getPenalty after jump size with " << totalPenalty << std::endl;
            SQINFO("%s", str.str().c_str());
        }
        return totalPenalty;
    }

    if (show) {
        str << "-- leaving getPenalty with " << totalPenalty << std::endl;
        SQINFO("%s", str.str().c_str());
    }

    return totalPenalty;
};

int ProgressionAnalyzer::RuleForJumpSize() const {
    for (int i = BASS; i <= SOP; i++) {
        const int jump = first->fetchNotes()[i] - next->fetchNotes()[i];
        // This was 12 - I changed to 8. I think it was a typo.
        if (abs(jump) > 8) {
            if (show) SQINFO("BIG jump in voice %d", i);
            return AVG_PENALTY_PER_RULE;
        }
    }
    return 0;
}

int ProgressionAnalyzer::RuleForSopranoJump(const Options& options) const {
    if (!options.style->limitSopranoJumps()) {
        return 0;
    }
    const int jump = first->fetchNotes()[SOP] - next->fetchNotes()[SOP];
    if (abs(jump) > 4) {
        if (show) SQINFO("BIG jump in SOP voice");
         return AVG_PENALTY_PER_RULE;
    }
    return 0;
}

int ProgressionAnalyzer::RuleForInversions(const Options& options) const {
    const auto style = options.style;

    if (style->getInversionPreference() == Style::InversionPreference::DONT_CARE) {
        return 0;  // if we don't mind consecutive inversions, no penalty
    }

    const bool firstChordInverted = first->inversion(options) != ROOT_POS_INVERSION;
    const bool secondChordInverted = next->inversion(options) != ROOT_POS_INVERSION;

    // bool fRet = true;
    bool twoInversionInARow = false;
    if (firstChordInverted && secondChordInverted) {
        twoInversionInARow = true;
    }

    int penalty = 0;

    if (style->getInversionPreference() == Style::InversionPreference::DISCOURAGE) {
        if (secondChordInverted) {
            penalty += AVG_PENALTY_PER_RULE;
        }
    }

    if (twoInversionInARow) {
        penalty += AVG_PENALTY_PER_RULE;
    }
    return penalty;
}

int ProgressionAnalyzer::FakeRuleForDesc(const Options& options) const {
    if (!options.style->forceDescSop()) return true;                  // check if rule enabled
    assert(false);
    bool ret = (next->fetchNotes()[SOP] < first->fetchNotes()[SOP]);  // For a test, lets make molody descend
    if (show && !ret) SQINFO("failed fake decrease melody");
    return ret ? 0 : AVG_PENALTY_PER_RULE;
}

/* bool ProgressionAnalyzer::Rule4Same()
 * check if all 4 vx in same direction
 */
int ProgressionAnalyzer::Rule4Same() const {
    DIREC di;
    int i;

    for (i = 1, di = direction[0]; i <= SOP; i++) {
        if (direction[i] != di) return 0;  // if any direct dif, cool
                                           // is same ok?
    }
    if (show) SQINFO("failing Rule4Same");
    return AVG_PENALTY_PER_RULE;
}

int ProgressionAnalyzer::RuleForCross() const {
    int i, j;

    for (i = BASS; i <= ALTO; i++) {
        for (j = i + 1; j <= SOP; j++)  // for all voice pairs
        {
            if (direction[i] == direction[j])  // if similar motion
            {
                if (first->fetchNotes()[i] > first->fetchNotes()[j])
                    if (show) SQINFO("!! these voices reversed 1.  vx %d to %d!!", i, j);
                if (next->fetchNotes()[i] > next->fetchNotes()[j])
                    if (show) SQINFO("!! these voices reversed 2.  vx %d to %d!!", i, j);

                if (direction[i] == DIR_UP)  // if both ascend...
                {
                    if (next->fetchNotes()[i] > first->fetchNotes()[j]) {
                        if (show) SQINFO("rules for cross, both asc first[%d] next[%d]", j, i);
                        return AVG_PENALTY_PER_RULE;
                    }
                }
                if (direction[i] == DIR_DOWN)  // if both DESC...
                {
                    if (next->fetchNotes()[j] < first->fetchNotes()[i]) {
                        if (show) SQINFO("rules for cross, both desc ! first[%d] next[%d]", i, j);
                        return AVG_PENALTY_PER_RULE;
                    }
                }
            }
        }
    }
    return 0;
}

int ProgressionAnalyzer::RuleForPara() const {
    int i, j;

    if (show) SQINFO("enter RuleForPara");
    for (i = BASS; i <= ALTO; i++) {
        for (j = i + 1; j <= SOP; j++) {
            const int NextInterval = next->fetchSRNNotes()[i].interval(next->fetchSRNNotes()[j]);
            // figure the interval between these
#if 0
        if (show) printf("in PAR, interval = %d, vx = %d to %d\n",
            NextInterval,
            i, j
            );
#endif

            switch (NextInterval) {
                case 5:  // fifth
                case 1:  // octave?

                    if (show) SQINFO("next interval=%d between vx %d and %d", NextInterval, i, j);
                   
                    const int FirstInterval =
                        first->fetchSRNNotes()[i].interval(first->fetchSRNNotes()[j]);
                    if (FirstInterval == NextInterval)  // paralel 5 or 12
                    {
                        if (show) {
                            SQINFO("found par 5th or oct");
                            SQINFO("-- Par motion to interval=%d vx%d/%d", FirstInterval, i, j);
                        SHOWZ:
                            if (show && false) SQINFO("in Par, vx %d to %d. considering %d to %d, int = %d (first int = %d)",
                                                      i, j,
                                                      int(next->fetchSRNNotes()[i]),
                                                      int(next->fetchSRNNotes()[j]),
                                                      NextInterval,
                                                      FirstInterval);
                        }
                        return AVG_PENALTY_PER_RULE;
                    }
                    if (direction[i] == direction[j])  // direct 5 or 12 in similar motion
                    {
                        if (show) {
                            SQINFO("-- RuleForPara found direct interval=%d->%d dir=%d, vx=%d,%d\n",
                                   FirstInterval, NextInterval,
                                   direction[i],
                                   i, j);
                            SQINFO("  dir: 0=up, 1=same 2=down");
                        }

                        goto SHOWZ;
                    }
                    break;
            }
        }
    }
    if (show) SQINFO("leaving RuleForPara\n");
    return 0;
}

int ProgressionAnalyzer::RuleForLeadingTone() const {
    int i, nPitch;
    bool fRet = true;

    for (i = BASS; i <= SOP; i++) {
        nPitch = first->fetchSRNNotes()[i];                               // get the scale degree of this voice of chord
        if (nPitch == 7) {                                                // if it is leading tone
            if (next->fetchNotes()[i] != (first->fetchNotes()[i] + 1)) {  // if it doesn't ascend to tonic
                if (next->fetchNotes()[i] > first->fetchNotes()[i]) {     // and it is ascend..
                                                                          // over simplification: force all lead to asc to tonic or desc
                                                                          // in some cases must be stricter
                    fRet = false;
                } else if (i == SOP) {     // if it is descending in soprano voice.
                    if (firstRoot == 5) {  // and progression from V ...
                        switch (nextRoot) {
                            case 1:  // V-I must asc
                            case 6:  // V-VI "  "
                                fRet = false;
                                break;
                        }
                    }
                }
            }
        }
    }
    if (show && !fRet) SQINFO("failing rule4Lead");
    return fRet ? 0 : AVG_PENALTY_PER_RULE;
}

int ProgressionAnalyzer::RuleForNoneInCommon(const Options& options) const {
    DIREC di;
    int i;

    if (notesInCommon != 0) {
        return 0;       // No penalty for this rule is there are notes in common.
    }

    if (!options.style->getNoNotesInCommon()) {
        return 0;       // If the style is ignoring this rule, then no penalty.
    }

    // No notes in common: upper 3 move opposite of bass.

    for (di = direction[TENOR], i = ALTO; i <= SOP; i++) {
        if (di != direction[i]) {
            if (show) SQINFO("violates notes in common rule 1a");
            return SLIGHTLY_HIGHER_PENALTY_PER_RULE;
        }
    }
    if (di == direction[BASS]) {
        if (show) SQINFO("violates notes in common rule 1b");
        return AVG_PENALTY_PER_RULE;
    }

    // TODO: what about the V-VI exception?

    // to nearest
    for (i = BASS; i <= SOP; i++) {
        if (!IsNearestNote(options, i)) {
            if (show) SQINFO("violates notes in common rule 1c");
            return AVG_PENALTY_PER_RULE;
        }
    }

    return 0;
}

int ProgressionAnalyzer::ruleForDoubling(const Options& options) const {
    assert(first->isAcceptableDoubling(options));
    assert(next->isAcceptableDoubling(options));

    return next->isCorrectDoubling(options) ? 0 : SLIGHTLY_LOWER_PENALTY_PER_RULE;
}

int ProgressionAnalyzer::ruleForSpreading(const Options& options) const {
    int ret = 0;

    if (options.style->pullTogether()) {
        // Distance from bass to tenor 9 and 13 were ok for this.

        // distance from bass to sop
        const int distance = next->fetchNotes()[3] - next->fetchNotes()[0];
        if (distance > 12) {
            ret = PENALTY_FOR_FAR_APART;
        }
        if (distance > 16) {
            ret = PENALTY_FOR_VERY_FAR_APART;
        }
    }

    return ret;
}

/* bool ProgressionAnalyzer::IsNearestNote(int nVoice)
 */
bool ProgressionAnalyzer::IsNearestNote(const Options& options, int nVoice) const {
    bool done;
    HarmonyNote ntFirst(options);
    HarmonyNote ntNext(options);

    for (done = false, ntFirst = first->fetchNotes()[nVoice], ntNext = next->fetchNotes()[nVoice];
         !done;) {
        // In this look, we will be movint the first note twords the next note...

        if ((int)ntFirst == (int)ntNext) {
            return true;
        }
        if (next->isInChord(options, ntFirst))  // if while we are moving him, we hit a note
                                                // that is in the chord, first, then THAT note must be
                                                // the first one
        {
            return false;
        }
        if (direction[nVoice] == DIR_UP)
            ++ntFirst;
        else
            --ntFirst;
    }
    SQWARN("we shouldn't get here!!");
    return true;
}


/* void ProgressionAnalyzer::FigureMotion()
 */
void ProgressionAnalyzer::figureMotion() {
    for (int i = BASS; i <= SOP; i++)  // for each voice
    {
        magMotion[i] = next->fetchNotes()[i] -
                       first->fetchNotes()[i];  // figure how much it changed

        if (magMotion[i] > 0)
            direction[i] = DIR_UP;
        else if (magMotion[i] < 0)
            direction[i] = DIR_DOWN;
        else
            direction[i] = DIR_SAME;
    }
}

/*  int Chord4::InCommon(const Chord4 * ThisGuy) const
 *
 * find how many voices are in common (by degree, not abs pitch)
 */

int ProgressionAnalyzer::InCommon() const {
    bool test[10];  // 8 degrees + 0 + 1 spare
    int matches;
    int i, nPitch;

    memset(test, 0, sizeof(test));  // clear our test hit array

    for (i = 0; i < CHORD_SIZE; i++) {
        //    nPitch = First.FetchNotes()[i];	// get the pitch of this chord member
        nPitch = first->fetchSRNNotes()[i];  // get the scale degree of this chord member
        test[nPitch] = true;                 // mark that we are here
    }

    // We have now marked all the pitches in our chord

    for (i = matches = 0; i < CHORD_SIZE; i++) {
        nPitch = next->fetchSRNNotes()[i];
        if (test[nPitch]) matches++;
    }
    return matches;
}
