
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
    ProgressionAnalyzer(const Chord4& C1, const Chord4& C2, bool fShow);
   // bool isLegal(const Options&) const;

    static const int PENALTY_FOR_REPEATED_CHORDS = {50};
    static const int AVG_PENALTY_PER_RULE = {100};
    static const int SLIGHTLY_HIGHER_PENALTY_PER_RULE = {110};
    static const int SLIGHTLY_LOWER_PENALTY_PER_RULE = {90};
    static const int MAX_PENALTY = { AVG_PENALTY_PER_RULE * 100};

    // 0 means perfect, negative numbers not allowed
    int getPenalty(const Options&) const;


    static void showAnalysis();

private:
    const Chord4& first;
    const Chord4& next;
    const int firstRoot;
    const int nextRoot;

    int magMotion[4];    // derived motion for each voice (magnitude)
    DIREC direction[4];  // "    "
    int notesInCommon;
    const bool show;  // for debugging

    //
    void figureMotion();
    int InCommon() const;

    // All rules return penalty, or zero for pass
    int Rule4Same() const;
    int RuleForNoneInCommon(const Options&) const;
 //   int RuleForNoneInCommonAndDoubling(const Options&) const;
    int ruleForDoubling(const Options& options) const;
    int RuleForLeadingTone() const;
    int RuleForPara() const;
    int RuleForCross() const;             // vx in similar motion shouldn't cross
    int RuleForConsecInversions(const Options& options) const;  // rule for two consec chords in first inversion
    int RuleForJumpSize() const ;
    int FakeRuleForDesc(const Options& options) const;  // force descending melody for torture test!

    bool IsNearestNote(const Options&, int Vx) const;  // True if the voice went to the nearest available slot
};

