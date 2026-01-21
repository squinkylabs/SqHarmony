
#pragma once

#include "MelodyEvaluator.h"

class EvaluatorResult {
public:
    EvaluatorResult(Styles st, float sc) : score(sc), rule(st) {}
     EvaluatorResult() {}
    float score = 0;
    Styles rule = Styles::Disabled;

    std::string toString() const;
    bool operator==(const EvaluatorResult&);
};

class EvaluationSummary {
public:
    EvaluationSummary() {}
    EvaluationSummary(EvaluatorResult a, EvaluatorResult b, EvaluatorResult c, EvaluatorResult d) {
        results[0] = a;
        results[1] = b;
        results[2] = c;
        results[3] = d;
    }
    /**
     * These should always be sorted, with the highest score first, at index 0.
     * some or all of the results my have rule == Styles::Disabled. If so, they
     * should all be adjacent to each other, coming last.
     *
     */
    EvaluatorResult results[numStyles];

    std::string toString() const;
    void assertValid() const;
    bool isValid() const;
    bool operator==(const EvaluationSummary&);
    void sort();

    static EvaluationSummary fromRows(const MelodyRow*, unsigned numRows, const MelodyMutateStyle&, unsigned worstRow);
    static EvaluationSummary fromRow(const MelodyRow&, const MelodyMutateStyle&);

    static void combine(EvaluationSummary& inOut, const EvaluationSummary& in);


private:
    bool _checkValid(bool doAssert) const;
};