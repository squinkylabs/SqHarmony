

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

ProgressionAnalyzer::ProgressionAnalyzer(
    const Chord4* C1,
    const Chord4* C2,
    bool fs,
    PAStats* stats)
    : first(C1),
      next(C2),
      firstRoot(C1->fetchRoot()),
      nextRoot(C2->fetchRoot()),
      _show(fs || showAlways),
      _stats(stats) {
    figureMotion();  // init the motion guys
    _notesInCommon = _CalcInCommon();

#ifdef _DEBUG
    if (_show) {
        SQINFO("*** Const of analyzer, chords are:");
        first->dump();
        next->dump();
        SQINFO("_notesInCommon = %d", _notesInCommon);
    }
#endif
}

class DebugStringAccumulator {
public:
    std::stringstream& get() {
        return str;
    }
    ~DebugStringAccumulator() {
        if (_shouldShow) {
            const std::string s = str.str();
            SQINFO("::: Will now output penalty string len %lld", s.length());
            SQINFO("%s", s.c_str());
            SQINFO(" ::: end :::");
        }
    }
    DebugStringAccumulator(bool show) : _shouldShow(show) {
    }

private:
    std::stringstream str;
    const bool _shouldShow;
};

int ProgressionAnalyzer::getPenalty(const Options& options, int upperBound) const {
#if 0  // let's get rid of this fake rule
    if (!FakeRuleForDesc(options)) {
        return false;
    }
#endif
    assert(&options);
    assert(this);
    DebugStringAccumulator ds(_show);

    if (_show) {
        ds.get() << ".. enter getPenalty." << std::endl;
        ds.get() << "first: ";
        ds.get() << first->toString();
        ds.get() << " second: ";
        ds.get() << next->toString() << std::endl;
        ds.get() << "upper bound ";
        ds.get() << upperBound << std::endl;
    }

    int totalPenalty = 0;
    int p = 0;  // The penalty.

    // ---- RuleForPara #1
    p = ruleForPara();
    totalPenalty += p;
    if (p && _stats) {
        _stats->_ruleForPara++;
    }
    if (p && _show) {
        ds.get() << "penalty: RuleForPara " << p << std::endl;
    }
    if (totalPenalty >= upperBound) {
        if (_show) {
            ds.get() << "-- leaving getPenalty after para with " << totalPenalty << std::endl;
            // SQINFO("%s", str.str().c_str());
        }
        return totalPenalty;
    }

    // ---- RuleForCross #2
    p = ruleForCross();
    totalPenalty += p;
    if (p && _show) {
        ds.get() << "penalty: RuleForCross " << p << std::endl;
    }
    if (p && _stats) {
        _stats->_ruleForCross++;
    }
    if (totalPenalty >= upperBound) {
        if (_show) {
            ds.get() << "-- leaving getPenalty after cross with " << totalPenalty << std::endl;
        }
        return totalPenalty;
    }

    // ---- RuleForNoneInCommon  #4
    p = ruleForNoneInCommon(options);
    totalPenalty += p;
    if (p && _show) {
        ds.get() << "penalty: RuleForNoneInCommon " << p << std::endl;
    }
    if (p && _stats) {
        _stats->_ruleForNoneInCommon++;
    }
    if (totalPenalty >= upperBound) {
        if (_show) {
            ds.get() << "-- leaving getPenalty after NIC with " << totalPenalty << std::endl;
        }
        return totalPenalty;
    }

    // ----- RuleForSopranoJump #5
    p = ruleForSopranoJump(options);
    totalPenalty += p;
    if (p && _show) {
        ds.get() << "penalty: RuleForSopranoJump " << p << std::endl;
    }
    if (p && _stats) {
        _stats->_ruleForSopranoJump++;
    }
    if (totalPenalty >= upperBound) {
        if (_show) {
            ds.get() << "-- leaving getPenalty after soprano jump size with " << totalPenalty << std::endl;
        }
        return totalPenalty;
    }

    // ---- RuleForLeadingTone #6
    p = ruleForLeadingTone();
    totalPenalty += p;
    if (p && _show) {
        ds.get() << "penalty: RuleForLeadingTone " << p << std::endl;
    }
    if (p && _stats) {
        _stats->_ruleForLeadingTone++;
    }
    if (totalPenalty >= upperBound) {
        if (_show) {
            ds.get() << "-- leaving getPenalty after leading tone with " << totalPenalty << std::endl;
        }
        return totalPenalty;
    }

    // ----- RuleForJumpSize #6
    p = ruleForJumpSize();
    totalPenalty += p;
    if (p && _show) {
        ds.get() << "penalty: RuleForJumpSize " << p << std::endl;
    }
    if (p && _stats) {
        _stats->ruleForJumpSize++;
    }
    if (totalPenalty >= upperBound) {
        if (_show) {
            ds.get() << "-- leaving getPenalty after jump size with " << totalPenalty << std::endl;
        }
        return totalPenalty;
    }

    // ---- RuleForInversions
    p = ruleForInversions(options);
    totalPenalty += p;
    if (p && _show) {
        ds.get() << "Penalty: RuleForConsecInversions " << p << std::endl;
    }
    if (p && _stats) {
        _stats->_ruleForInversions++;
    }

    if (totalPenalty >= upperBound) {
        if (_show) {
            ds.get() << "-- leaving getPenalty after consec inv with " << totalPenalty << std::endl;
        }
        return totalPenalty;
    }

    // ---- Rule4Same
    p = rule4Same();
    if (p && _show) {
        ds.get() << "back from rule for same with " << p << std::endl;
    }
    totalPenalty += p;
    if (p && _show) {
        ds.get() << "penalty: Rule4Same " << p << " tot=" << totalPenalty << std::endl;
    }
    if (p && _stats) {
        _stats->_ruleForSame++;
    }
    if (totalPenalty >= upperBound) {
        if (_show) {
            ds.get() << "-- leaving getPenalty after 4sam with " << totalPenalty << std::endl;
        }
        return totalPenalty;
    }

    // ---- ruleForDoubling
    p = ruleForDoubling(options);
    totalPenalty += p;
    if (p && _show) {
        ds.get() << "penalty: RuleForDoubling " << p << std::endl;
    }
    if (p && _stats) {
        _stats->_ruleForDoubling++;
    }
    if (totalPenalty >= upperBound) {
        if (_show) {
            ds.get() << "-- leaving getPenalty after doubling with " << totalPenalty << std::endl;
            // SQINFO("%s", str.str().c_str());
        }
        return totalPenalty;
    }

    // ----- ruleForSpreading
    p = ruleForSpreading(options);
    totalPenalty += p;
    if (p && _show) {
        ds.get() << "penalty: RuleForSpreading " << p << std::endl;
    }
    if (p && _stats) {
        _stats->_ruleForSpreading++;
    }
    if (totalPenalty >= upperBound) {
        if (_show) {
            ds.get() << "-- leaving getPenalty after spreading with " << totalPenalty << std::endl;
            // SQINFO("%s", str.str().c_str());
        }
        return totalPenalty;
    }

    // ----- RuleForDistanceMoved
    p = ruleForDistanceMoved(options);
    totalPenalty += p;
    if (p && _show) {
        ds.get() << "penalty: RuleForDistanceMoved " << p << std::endl;
    }
    if (p && _stats) {
        _stats->_ruleForDistanceMoved++;
    }
    if (totalPenalty >= upperBound) {
        if (_show) {
            ds.get() << "-- leaving getPenalty after distance moved with " << totalPenalty << std::endl;
        }
        return totalPenalty;
    }

    //---------------------
    if (_show) {
        ds.get() << "-- leaving getPenalty 278 (bottom) with total " << totalPenalty << std::endl;
    }

    return totalPenalty;
};

// Check for a very large jump in any voices (more than an octave).
int ProgressionAnalyzer::ruleForJumpSize() const {
    for (int i = BASS; i <= SOP; i++) {
        const int jump = first->fetchNotes()[i] - next->fetchNotes()[i];
        if (abs(jump) > 8) {
            // TODO: actually in this domain units are semitones.
            // if (_show) SQINFO("BIG jump in voice %d", i);
            return AVG_PENALTY_PER_RULE;
        }
    }
    return 0;
}

int ProgressionAnalyzer::ruleForSopranoJump(const Options& options) const {
    if (_show) SQINFO("enter rule for soprano jump");
    if (!options.style->limitSopranoJumps()) {
        return 0;
    }
    const int jump = first->fetchNotes()[SOP] - next->fetchNotes()[SOP];
    if (_show) SQINFO("jump is %d", jump);
    if (abs(jump) > 4) {  // was 4, try 3
        if (_show) SQINFO("moderately big jump in SOP voice");
        return AVG_PENALTY_PER_RULE;
    }
    return 0;
}

int ProgressionAnalyzer::ruleForInversions(const Options& options) const {
    const auto style = options.style;

    if (style->getInversionPreference() == Style::InversionPreference::DONT_CARE) {
        return 0;  // if we don't mind consecutive inversions, no penalty
    }

    const bool firstChordInverted = first->inversion(options) != ROOT_POS_INVERSION;
    const bool secondChordInverted = next->inversion(options) != ROOT_POS_INVERSION;

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

int ProgressionAnalyzer::fakeRuleForDesc(const Options& options) const {
    if (!options.style->forceDescSop()) return true;  // check if rule enabled
    assert(false);
    bool ret = (next->fetchNotes()[SOP] < first->fetchNotes()[SOP]);  // For a test, lets make molody descend
    if (_show && !ret) SQINFO("failed fake decrease melody");
    return ret ? 0 : AVG_PENALTY_PER_RULE;
}

/* bool ProgressionAnalyzer::Rule4Same()
 * check if all 4 vx in same direction
 */
int ProgressionAnalyzer::rule4Same() const {
    DIREC di;
    int i;

    for (i = 1, di = _direction[0]; i <= SOP; i++) {
        if (_direction[i] != di) return 0;  // if any direct dif, cool
                                            // is same ok?
    }
    if (_show) SQINFO("failing Rule4Same");
    return AVG_PENALTY_PER_RULE;
}

int ProgressionAnalyzer::ruleForCross() const {
    int i, j;

    for (i = BASS; i <= ALTO; i++) {
        for (j = i + 1; j <= SOP; j++)  // for all voice pairs
        {
            if (_direction[i] == _direction[j])  // if similar motion
            {
                if (first->fetchNotes()[i] > first->fetchNotes()[j])
                    if (_show) SQINFO("!! these voices reversed 1.  vx %d to %d!!", i, j);
                if (next->fetchNotes()[i] > next->fetchNotes()[j])
                    if (_show) SQINFO("!! these voices reversed 2.  vx %d to %d!!", i, j);

                if (_direction[i] == DIR_UP)  // if both ascend...
                {
                    if (next->fetchNotes()[i] > first->fetchNotes()[j]) {
                        if (_show) SQINFO("rules for cross, both asc first[%d] next[%d]", j, i);
                        return AVG_PENALTY_PER_RULE;
                    }
                }
                if (_direction[i] == DIR_DOWN)  // if both DESC...
                {
                    if (next->fetchNotes()[j] < first->fetchNotes()[i]) {
                        if (_show) SQINFO("rules for cross, both desc ! first[%d] next[%d]", i, j);
                        return AVG_PENALTY_PER_RULE;
                    }
                }
            }
        }
    }
    return 0;
}

int ProgressionAnalyzer::ruleForPara() const {
    int i, j;

    if (_show) {
        SQINFO("enter RuleForPara");
    }
    for (i = BASS; i <= ALTO; i++) {
        for (j = i + 1; j <= SOP; j++) {
            const int NextInterval = next->fetchSRNNotes()[i].interval(next->fetchSRNNotes()[j]);
            // figure the interval between these
            switch (NextInterval) {
                case 5:  // fifth
                case 1:  // octave?
                    if (_show) SQINFO("next interval=%d between vx %d and %d", NextInterval, i, j);

                    const int FirstInterval =
                        first->fetchSRNNotes()[i].interval(first->fetchSRNNotes()[j]);
                    if (FirstInterval == NextInterval)  // Parallel 5 or 12.
                    {
                        if (_show) {
                            SQINFO("found par 5th or oct");
                            SQINFO("-- Par motion to interval=%d vx%d/%d", FirstInterval, i, j);
                        SHOWZ:
                            if (_show && false) SQINFO("in Par, vx %d to %d. considering %d to %d, int = %d (first int = %d)",
                                                       i, j,
                                                       int(next->fetchSRNNotes()[i]),
                                                       int(next->fetchSRNNotes()[j]),
                                                       NextInterval,
                                                       FirstInterval);
                        }
                        return AVG_PENALTY_PER_RULE;
                    }
                    if (_direction[i] == _direction[j])  // direct 5 or 12 in similar motion
                    {
                        if (_show) {
                            SQINFO("-- RuleForPara found direct interval=%d->%d dir=%d, vx=%d,%d\n",
                                   FirstInterval, NextInterval,
                                   _direction[i],
                                   i, j);
                            SQINFO("  dir: 0=up, 1=same 2=down");
                        }

                        goto SHOWZ;
                    }
                    break;
            }
        }
    }
    if (_show) SQINFO("leaving RuleForPara");
    return 0;
}

int ProgressionAnalyzer::ruleForLeadingTone() const {
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
    if (_show && !fRet) SQINFO("failing rule4Lead");
    return fRet ? 0 : AVG_PENALTY_PER_RULE;
}

int ProgressionAnalyzer::ruleForNoneInCommon(const Options& options) const {
    if (_show) {
        SQINFO("enter RuleForNoneInCommon");
    }
    if (_notesInCommon != 0) {
        return 0;  // No penalty for this rule is there are notes in common.
    }

    if (!options.style->getNoNotesInCommon()) {
        return 0;  // If the style is ignoring this rule, then no penalty.
    }

    if (first->fetchRoot() == 5 && next->fetchRoot() == 6 && options.style->usePistonV_VI_exception()) {
        //  SQWARN("This is the exception case");
        return ruleForNoneInCommon56(options);
    } else {
        return ruleForNoneInCommonNorm(options);
    }
}

int ProgressionAnalyzer::ruleForNoneInCommon56(const Options& options) const {
    // The exception for 5-6 is:
    // leading-tone moves up one degree to the tonic.
    // the other two voices descend to the nearest position in the chord
    // the third is doubled in the VI chord
    if (_show) {
        SQINFO("enter RuleForNoneInCommon56");
    }

    int leadingToneVoice = -1;
    const auto firstNotes = first->fetchSRNNotes();
    for (int i = BASS; i <= SOP; ++i) {
        const int degree = int(firstNotes[i]);
        if (degree == 7) {
            assert(leadingToneVoice < 0);
            leadingToneVoice = i;
        }
    }

    assert(leadingToneVoice >= BASS);

    const auto nextSRNotes = next->fetchSRNNotes();

    // Leading-tone moves up one degree to the tonic.
    if ((int(nextSRNotes[leadingToneVoice]) != 1) ||
        (_motion[leadingToneVoice] > 2) ||
        (_motion[leadingToneVoice] < 1)) {
        return AVG_PENALTY_PER_RULE;
    }

    //  The third is doubled in the VI chord.
    int numThirdsInVI = 0;
    for (int i = BASS; i <= SOP; ++i) {
        const ChordRelativeNote crn = next->chordInterval(options, i);
        // only expect these degrees in a triad.
        assert(crn == 1 || crn == 3 || crn == 5);
        if (crn == 3) {
            ++numThirdsInVI;
        }
    }

    if (numThirdsInVI != 2) {
        return AVG_PENALTY_PER_RULE;
    }

    // The other two voices descend to the nearest position in the chord.
    // I think this means that one of the thirds descends.
    int thirdsAscending = 0;
    for (int i = BASS; i <= SOP; ++i) {
        if (i == leadingToneVoice) {
            // We have already taken care of this at the start. so we are ok here,
            // and can ignore that voice.
            continue;
        }
        const auto voiceDirection = _direction[i];

        // any voice going down is fine
        if (voiceDirection == DIREC::DIR_DOWN) {
            continue;
        }

        // here we are going up, and we are not the leading tone
        const bool isThird = nextSRNotes[i] == 3;
        if (isThird) {
            ++thirdsAscending;
        }
        if (thirdsAscending > 1) {
            return AVG_PENALTY_PER_RULE;
        }
    }
    return 0;
}

int ProgressionAnalyzer::ruleForNoneInCommonNorm(const Options& options) const {
    if (_show) {
        SQINFO("enter ruleForNoneInCommonNorm");
    }
    const DIREC di = _direction[TENOR];
    // No notes in common: upper 3 move opposite of bass.

    for (int i = ALTO; i <= SOP; i++) {
        if (di != _direction[i]) {
            if (_show) {
                SQINFO("violates notes in common rule 1a");
            }
            return SLIGHTLY_HIGHER_PENALTY_PER_RULE;
        }
    }
    if (di == _direction[BASS]) {
        if (_show) SQINFO("violates notes in common rule 1b");
        return AVG_PENALTY_PER_RULE;
    }

    // TODO: what about the V-VI exception?

    // to nearest
    for (int i = BASS; i <= SOP; i++) {
        if (!IsNearestNote(options, i)) {
            if (_show) {
                SQINFO("violates notes in common rule 1c");
            }
            return AVG_PENALTY_PER_RULE;
        }
    }

    return 0;
}

int ProgressionAnalyzer::ruleForDoubling(const Options& options) const {
    assert(first->isAcceptableDoubling(options));
    assert(next->isAcceptableDoubling(options));

    // Actually, all flavors of "no notes in common" carry their own doubling,
    // so there is never a reason to evaluate doubling in this case.
    if (options.style->getNoNotesInCommon() && (_notesInCommon == 0)) {
        return 0;
    }

    if ((first->fetchRoot() == 5) &&
        (next->fetchRoot() == 6) &&
        options.style->getNoNotesInCommon() &&
        options.style->usePistonV_VI_exception()) {
        // No notes in common rule for piston will already have its own doubling, which
        // is "non standard".
        // SQINFO("not evaluating doubling for piston V VI");
        return 0;
    }

    return next->isCorrectDoubling(options) ? 0 : SLIGHTLY_LOWER_PENALTY_PER_RULE;
}

int ProgressionAnalyzer::ruleForSpreading(const Options& options) const {
    int ret = 0;

    if (options.style->pullTogether()) {
        // Distance from bass to tenor 9 and 13 were ok for this.

        // Distance from bass to sop.
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

int ProgressionAnalyzer::ruleForDistanceMoved(const Options& options) const {
    if (_notesInCommon == 0) {
        return 0;  // no notes in common rules may require more movement.
    }

    int totalPenalty = 0;

    // Skip sop voice, since we already have a rule for it.
    for (int nVoice = BASS; nVoice < SOP; ++nVoice) {
        const int firstPitch = first->fetchNotes()[nVoice];
        const int nextPitch = next->fetchNotes()[nVoice];
        const int diffSquared = (firstPitch - nextPitch) * (firstPitch - nextPitch);
        totalPenalty += diffSquared;
    }
    // TODO: use defined values
    const int differenceThreshold = 30;                    // 50 worked fine, 30ok 10 too small
    return (totalPenalty > differenceThreshold) ? 50 : 0;  // 50 ok
}

/* bool ProgressionAnalyzer::IsNearestNote(int nVoice)
 */
bool ProgressionAnalyzer::IsNearestNote(const Options& options, int nVoice) const {
    bool done;
    HarmonyNote ntFirst(options);
    HarmonyNote ntNext(options);

    for (done = false, ntFirst = first->fetchNotes()[nVoice], ntNext = next->fetchNotes()[nVoice];
         !done;) {
        // In this look, we will be moving the first note towards the next note...
        if ((int)ntFirst == (int)ntNext) {
            return true;
        }
        if (next->isInChord(options, ntFirst))  // If while we are moving him, we hit a note
                                                // that is in the chord, first, then THAT note must be
                                                // the first one.
        {
            return false;
        }
        if (_direction[nVoice] == DIR_UP)
            ++ntFirst;
        else
            --ntFirst;
    }
    SQWARN("we shouldn't get here!!");
    return true;
}

void ProgressionAnalyzer::figureMotion() {
    for (int i = BASS; i <= SOP; i++)  // for each voice
    {
        _motion[i] = next->fetchNotes()[i] -
                     first->fetchNotes()[i];  // figure how much it changed

        if (_motion[i] > 0)
            _direction[i] = DIR_UP;
        else if (_motion[i] < 0)
            _direction[i] = DIR_DOWN;
        else
            _direction[i] = DIR_SAME;
    }
}

int ProgressionAnalyzer::_CalcInCommon() const {
    bool test[10];  // 8 degrees + 0 + 1 spare
    int matches;
    int i, nPitch;

    memset(test, 0, sizeof(test));  // Clear our test hit array.

    for (i = 0; i < CHORD_SIZE; i++) {
        nPitch = first->fetchSRNNotes()[i];  // Get the scale degree of this chord member.
        test[nPitch] = true;                 // Mark that we are here.
    }

    // We have now marked all the pitches in our chord.
    for (i = matches = 0; i < CHORD_SIZE; i++) {
        nPitch = next->fetchSRNNotes()[i];
        if (test[nPitch]) matches++;
    }
    return matches;
}

void PAStats::dump() const {
    SQINFO("ProgressionAnalyzer stats:");
    SQINFO("  _ruleForLeadingTone %d", _ruleForLeadingTone);
    SQINFO("  _ruleForPara %d", _ruleForPara);
    SQINFO("  _ruleForCross %d", _ruleForCross);
    SQINFO("  _ruleForSame %d", _ruleForSame);
    SQINFO("  _ruleForNoneInCommon %d", _ruleForNoneInCommon);
    SQINFO("  _ruleForDoubling %d", _ruleForDoubling);
    SQINFO("  _ruleForSpreading %d", _ruleForSpreading);
    SQINFO("  ruleForJumpSize %d", ruleForJumpSize);
    SQINFO("  _ruleForSopranoJump %d", _ruleForSopranoJump);
    SQINFO(" _ruleForDistanceMoved %d", _ruleForDistanceMoved);
}
