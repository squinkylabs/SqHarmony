
#pragma once

class Chord4;
class Options;

enum DIREC {
    DIR_UP,
    DIR_SAME,
    DIR_DOWN
};

class ProgressionAnalyzer {
public:
    ProgressionAnalyzer(const Chord4* C1, const Chord4* C2, bool fShow);
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

    static void showAnalysis();

private:
    const Chord4* const first;
    const Chord4* const next;
    const int firstRoot;
    const int nextRoot;

    int _motion[4];    // Derived motion for each voice (bipolar, in semitones).
    DIREC direction[4];  // "    "
    int notesInCommon;
    const bool show;  // for debugging

    //
    void figureMotion();
    int InCommon() const;

    // All rules return penalty, or zero for pass
    int Rule4Same() const;
    int RuleForNoneInCommon(const Options&) const;
    int RuleForNoneInCommon56(const Options&) const;
    int RuleForNoneInCommonNorm(const Options&) const;
    int ruleForDoubling(const Options& options) const;
    int ruleForSpreading(const Options& options) const;
    int RuleForLeadingTone() const;
    int RuleForPara() const;
    int RuleForCross() const;                               // Vx in similar motion shouldn't cross.
    int RuleForInversions(const Options& options) const;    // Rule for two consec chords in first inversion.
    int RuleForJumpSize() const;
    int FakeRuleForDesc(const Options& options) const;  // Force descending melody for torture test!
    int RuleForSopranoJump(const Options& options) const; 

    bool IsNearestNote(const Options&, int Vx) const;  // True if the voice went to the nearest available slot
};
