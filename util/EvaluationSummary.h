
#pragma once

#include "MelodyEvaluator.h"

class EvaluatorResult {
    public:
    float score = 0;
    Styles rule = Styles::Disabled;
};

class EvaluationSummary {
    EvaluatorResult results[numStyles];
};