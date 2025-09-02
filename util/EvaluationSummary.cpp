

#include "EvaluationSummary.h"

#include "MelodyEvaluator.h"
#include "MelodyGenerator.h"

EvaluationSummary EvaluationSummary::fromRows(const MelodyRow* pRows, unsigned numRows, const MelodyMutateStyle& style) {
    EvaluationSummary ret;
    for (unsigned i = 0; i < numRows; ++i) {
        const MelodyRow& row = pRows[i];

        // TODO: we should really combine this... need a new test.
        // Actually: don't we just return the summary from the worst row?
        ret = fromRow(row, style);
    }
    return ret;
}

EvaluationSummary EvaluationSummary::fromRow(const MelodyRow& row, const MelodyMutateStyle& style) {
    MelodyEvaluator::getPenalty(row, style);

    EvaluationSummary ret;

    unsigned index = (unsigned int)(Styles::OnlySeekCenter);
    ret.results[index].rule = Styles::OnlySeekCenter;
    ret.results[index].score = MelodyEvaluator::nonCenteredPenalty(row, style);

    index = (unsigned int)(Styles::OnlySeekRange);
    ret.results[index].rule = Styles::OnlySeekRange;
    ret.results[index].score = MelodyEvaluator::pitchRangePenalty(row, style);

    index = (unsigned int)(Styles::OnlyDiscorageLeaps);
    ret.results[index].rule = Styles::OnlyDiscorageLeaps;
    ret.results[index].score = MelodyEvaluator::leapsPenalty(row, style);

    index = (unsigned int)(Styles::OnlyDissonant);
    ret.results[index].rule = Styles::OnlyDissonant;
    ret.results[index].score = MelodyEvaluator::disonnantPenalty(row, style);

    assert(int(Styles::OnlyDissonant) == (int(Styles::Disabled) - 1));

    std::function<bool(const EvaluatorResult&, const EvaluatorResult&)> f = [](const EvaluatorResult& a, const EvaluatorResult& b) {
        return a.score > b.score;
    };
    std::sort(ret.results, ret.results + int(Styles::OnlyDissonant) + 1, f);
    return ret;
}