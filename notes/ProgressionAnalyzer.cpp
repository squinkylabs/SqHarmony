

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
#if 0
if (show) 
    {
    TRACE("Const of analyzer, chords are:\n");
  	First.Dump(afxDump);
    Next.Dump(afxDump);
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
    }

    int totalPenalty = 0;
    int p = RuleForConsecInversions(options);
    totalPenalty += p;
    if (p && show) {
        str << "Penalty: RuleForConsecInversions " << p << std::endl;
    }

    if (p >= upperBound) {
        return p;
    }

    p = RuleForLeadingTone();
    totalPenalty += p;
    if (p && show) {
        str << "penalty: RuleForLeadingTone " << p << std::endl;
    }
    if (p >= upperBound) {
        return p;
    }

    p = RuleForPara();
    totalPenalty += p;

    if (p && show) {
        str << "penalty: RuleForPara " << p << std::endl;
    }
    if (p >= upperBound) {
        return p;
    }

    p = RuleForCross();
    totalPenalty += p;
    if (p && show) {
        str << "penalty: RuleForCross " << p << std::endl;
    }
    if (p >= upperBound) {
        return p;
    }

    p = Rule4Same();
    totalPenalty += p;
    if (p && show) {
        str << "penalty: Rule4Same " << p << std::endl;
    }
    if (p >= upperBound) {
        return p;
    }

    p = RuleForNoneInCommon(options);
    totalPenalty += p;
    if (p && show) {
        str << "penalty: RuleForNonInCommon " << p << std::endl;
    }
    if (p >= upperBound) {
        return p;
    }

    p = ruleForDoubling(options);
    totalPenalty += p;
    if (p && show) {
        str << "penalty: RuleForDoubling " << p << std::endl;
    }
    if (p >= upperBound) {
        return p;
    }

    p = ruleForSpreading(options);
    totalPenalty += p;
    if (p && show) {
        str << "penalty: RuleForSpreading " << p << std::endl;
    }
    if (p >= upperBound) {
        return p;
    }

    p = RuleForJumpSize();
    totalPenalty += p;
    if (p && show) {
        str << "penalty: RuleForJumpSize " << p << std::endl;
    }
    if (p >= upperBound) {
        return p;
    }

    if (show) {
        str << "-- leaving getPenalty with " << totalPenalty << std::endl;
        printf("%s", str.str().c_str());
    }

    return totalPenalty;
};

int ProgressionAnalyzer::RuleForJumpSize() const {
    for (int i = BASS; i <= SOP; i++) {
        int jump = first->fetchNotes()[i] - next->fetchNotes()[i];
        // TODO: is 12 right here? should this be 8 instead?
        if (abs(jump) > 8) {
            if (show) printf("BIG jump in voice %d\n", i);
            return AVG_PENALTY_PER_RULE;
        }
    }
    return 0;
}

int ProgressionAnalyzer::RuleForConsecInversions(const Options& options) const {
    const auto style = options.style;

    if (style->getInversionPreference() == Style::Inversion::DONT_CARE) {
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

    if (style->getInversionPreference() == Style::Inversion::DISCOURAGE) {
        if (secondChordInverted) {
            penalty += AVG_PENALTY_PER_RULE / 2;  // TODO:
        }
    }

    if (twoInversionInARow) {
        penalty += AVG_PENALTY_PER_RULE;
    }
    return penalty;
}

int ProgressionAnalyzer::FakeRuleForDesc(const Options& options) const {
    if (!options.style->forceDescSop()) return true;                  // check if rule enabled
    bool ret = (next->fetchNotes()[SOP] < first->fetchNotes()[SOP]);  // For a test, lets make molody descend
    if (show && !ret) printf("failed decrese melody\n");
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
    if (show) printf("failing Rule4Same\n");
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
                    if (show) printf("!! these voices reversed 1.  vx %d to %d!!\n", i, j);
                if (next->fetchNotes()[i] > next->fetchNotes()[j])
                    if (show) printf("!! these voices reversed 2.  vx %d to %d!!\n", i, j);

                if (direction[i] == DIR_UP)  // if both ascend...
                {
                    if (next->fetchNotes()[i] > first->fetchNotes()[j]) {
                        if (show) printf("rules for cross, both asc first[%d] next[%d]\n", j, i);
                        return AVG_PENALTY_PER_RULE;
                    }
                }
                if (direction[i] == DIR_DOWN)  // if both DESC...
                {
                    if (next->fetchNotes()[j] < first->fetchNotes()[i]) {
                        if (show) printf("rules for cross, both desc ! first[%d] next[%d]\n", i, j);
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

    if (show) printf("enter RuleForPara\n");
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

                    const int FirstInterval =
                        first->fetchSRNNotes()[i].interval(first->fetchSRNNotes()[j]);
                    if (FirstInterval == NextInterval)  // paralel 5 or 12
                    {
                        if (show) {
                            printf("found par 5th or oct");
                            printf("-- Par motion to interval=%d vx%d/%d\n", FirstInterval, i, j);
                        SHOWZ:
                            if (show && false) printf("in Par, vx %d to %d. considering %d to %d, int = %d (first int = %d)\n",
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
                            printf("-- RuleForPara found direct int=%d->%d dir=%d, vx=%d,%d\n",
                                   FirstInterval, NextInterval,
                                   direction[i],
                                   i, j);
                            printf("  dir: 0=up, 1=same 2=down\n");
                        }

                        goto SHOWZ;
                    }
                    break;
            }
        }
    }
    if (show) printf("leaving RuleForPara\n");
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
    if (show && !fRet) printf("failing rule4Lead\n");
    return fRet ? 0 : AVG_PENALTY_PER_RULE;
}

int ProgressionAnalyzer::RuleForNoneInCommon(const Options& options) const {
    DIREC di;
    int i;

    if (notesInCommon != 0) {
        return 0;
    }

    if (!options.style->getNoNotesInCommon()) {
        return 0;
    }
    // assert(notesInCommon == 0);

    // No notes in common: upper 3 move opposite of bass

    for (di = direction[TENOR], i = ALTO; i <= SOP; i++) {
        if (di != direction[i]) {
            if (show) printf("violates notes in common rule 1a\n");
            return SLIGHTLY_HIGHER_PENALTY_PER_RULE;
        }
    }
    if (di == direction[BASS]) {
        if (show) printf("violates notes in common rule 1b\n");
        return AVG_PENALTY_PER_RULE;
    }

    // to nearest
    for (i = BASS; i <= SOP; i++) {
        if (!IsNearestNote(options, i)) {
            if (show) printf("violates notes in common rule 1c\n");
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
        const int distance = next->fetchNotes()[1] - next->fetchNotes()[0];
        if (distance > 7) {
            ret = PENALTY_FOR_FAR_APART;
        }
        if (distance > 11) {
            ret = PENALTY_FOR_VERY_FAR_APART;
        }
    }

    return ret;
}

#if 0
int ProgressionAnalyzer::ruleForSpreading(const Options& options) const {
    int ret = 0;

    if (options.style->pullTogether()) {
        const int distance = next->fetchNotes()[3] - next->fetchNotes()[0];
        if (distance > 15) {
            ret = PENALTY_FOR_FAR_APART;
        }
        if (distance > 20) {
            ret = PENALTY_FOR_VERY_FAR_APART;
        }
    }

    return ret;
}
#endif

/* bool ProgressionAnalyzer::RuleForNoneInCommon()
 */
#if 0
bool ProgressionAnalyzer::RuleForNoneInCommon(const Options& options) const {
    DIREC di;
    int i;

    if (notesInCommon != 0) return true;

    // No notes in common: upper 3 move opposite of bass

    for (di = direction[TENOR], i = ALTO; i <= SOP; i++) {
        if (di != direction[i]) {
            if (show) printf("violates notes in common rule 1a\n");
            return false;
        }
    }
    if (di == direction[BASS]) {
        if (show) printf("violates notes in common rule 1b\n");
        return false;
    }

    // to nearest
    for (i = BASS; i <= SOP; i++) {
        if (!IsNearestNote(options, i)) {
            if (show) printf("violates notes in common rule 1c\n");
            return false;
        }
    }
    return true;
}
#endif

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
    printf("we shouldn't get here!!\n");
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
