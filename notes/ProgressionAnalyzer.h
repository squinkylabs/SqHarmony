
#pragma once

class Chord4;
class Options;

enum DIREC {
    DIR_UP,
    DIR_SAME,
    DIR_DOWN
};

class PAStats {
public:
    void dump() const;

    int _ruleForInversions = 0;
    int _ruleForLeadingTone = 0;
    int _ruleForPara = 0;
    int _ruleForCross = 0;
    int _ruleForSame = 0;
    int _ruleForNoneInCommon = 0;
    int _ruleForDoubling = 0;
    int _ruleForSpreading = 0;
    int ruleForJumpSize = 0;
    int _ruleForSopranoJump = 0;
    int _ruleForDistanceMoved = 0;

private:
};

class ProgressionAnalyzer {
public:
    ProgressionAnalyzer(const Chord4* C1, const Chord4* C2, bool fShow, PAStats* collectStats = nullptr);
    // bool isLegal(const Options&) const;

    static const int PENALTY_FOR_REPEATED_CHORDS = {50};
    static const int PENALTY_FOR_FAR_APART = {80};
    static const int PENALTY_FOR_VERY_FAR_APART = {100};
    static const int AVG_PENALTY_PER_RULE = {100};
    static const int PENALTY_FOR_WEAK_RULE = {AVG_PENALTY_PER_RULE / 2};
    static const int SLIGHTLY_HIGHER_PENALTY_PER_RULE = {110};
    static const int SLIGHTLY_LOWER_PENALTY_PER_RULE = {90};
    static const int MAX_PENALTY = {AVG_PENALTY_PER_RULE * 100};

    // 0 means perfect, negative numbers not allowed
    int getPenalty(const Options&, int upperBound) const;

    // Sets a PA to show analysis every time it runs.
    static void showAnalysis();

private:
    const Chord4* const first;
    const Chord4* const next;
    const int firstRoot;
    const int nextRoot;

    int _motion[4];       // Derived motion for each voice (bipolar, in semitones).
    DIREC _direction[4];  // "    "
    int _notesInCommon=0;
    const bool _show;  // for debugging
    PAStats* const _stats;

    //
    void figureMotion();
    int _CalcInCommon() const;

    // All rules return penalty, or zero for pass
    int rule4Same() const;
    int ruleForNoneInCommon(const Options&) const;
    int ruleForNoneInCommon56(const Options&) const;
    int ruleForNoneInCommonNorm(const Options&) const;
    int ruleForDoubling(const Options& options) const;
    int ruleForSpreading(const Options& options) const;
    int ruleForLeadingTone() const;
    int ruleForPara() const;
    int ruleForCross() const;                             // Vx in similar motion shouldn't cross.
    int ruleForInversions(const Options& options) const;  // Rule for two consecutive chords in first inversion.
    int ruleForJumpSize() const;
    int fakeRuleForDesc(const Options& options) const;  // Force descending melody for torture test!
    int ruleForSopranoJump(const Options& options) const;
    int ruleForDistanceMoved(const Options& options) const;

    bool IsNearestNote(const Options&, int Vx) const;  // True if the voice went to the nearest available slot
};
