
#pragma once

#include "MelodyEvaluator.h"

class EvaluatorResult {
    public:
    float score = 0;
    Styles rule = Styles::Disabled;
};

class EvaluationSummary {
public:
    /**
     * These should always be sorted, with the highest score first, at index 0.
     * some or all of the results my have rule == Styles::Disabled. If so, they 
     * should all be adjacent to each other, coming last.
     * 
     */
    EvaluatorResult results[numStyles];

    static EvaluationSummary fromRows(const MelodyRow*, unsigned numRows, const MelodyMutateStyle&);
    static EvaluationSummary fromRow(const MelodyRow&, const MelodyMutateStyle&);
};